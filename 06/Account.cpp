#include "Account.h"
#include "Bank.h"
#include "InterstellarBankCommission.h"
#include <cstddef>
#include <sstream>

Account::Account(number_type number, const Citizen& accountHolder, Bank& bank,
                 Currency c)
    : number(number),
      accountHolder(accountHolder),
      bank(bank),
      accountBalance(0, c) {}

Amount Account::balance() const { return accountBalance; }

Account::id_type Account::id() { return AccountId(this); }

Account::AccountHistory Account::history() const {
    return AccountHistory(accountHistory);
}

std::string Account::stringId() const {
    return std::to_string(bank.id()) + "-" + std::to_string(number);
}

void Account::capitalizeInterests() {
    if (interestRate == 0 || accountBalance.amount <= 0)
        return;

    double to_add = interestRate * accountBalance.amount / 100;
    auto added_amount = Amount(to_add, accountBalance.currency);
    accountHistory.push_back(transaction_ptr_type(new Interest(added_amount)));
    accountBalance.amount += to_add;
}

void Account::chargeFee(double amount) {
    if (amount == 0) return;

    double to_substract = amount;

    // Charges are always specified in ENC
    if (accountBalance.currency != Currency::ENC) {
        to_substract /=
            bank.exchangeTable().getBuyingRate(accountBalance.currency);
    }

    accountHistory.push_back(
        transaction_ptr_type(new Charge(Amount(-amount, Currency::ENC))));
    accountBalance.amount -= to_substract;
}

void Account::revertFee(double amount) {
    if (amount == 0) return;

    double to_add = amount;

    // Charges are always specified in ENC
    if (accountBalance.currency != Currency::ENC) {
        to_add /= bank.exchangeTable().getBuyingRate(accountBalance.currency);
    }

    accountHistory.pop_back();
    accountBalance.amount += to_add;
}

void Account::chargeMonthlyCharge() { chargeFee(monthlyCharge); }

void Account::chargeTransferFee() { chargeFee(transferCharge); }

void Account::transfer(double amount, id_type id) {
    transfer(Amount(amount, accountBalance.currency), id, std::string());
}

void Account::transfer(double amount, id_type id,
                       const std::string& description) {
    Account::transfer(Amount(amount, accountBalance.currency), id, description);
}

void Account::transfer(Amount amount, id_type id) {
    transfer(amount, id, std::string());
}

void Account::transfer(Amount amount, id_type id,
                       const std::string& description) {
    // By default only accept transfers with matching currency
    if (amount.currency != accountBalance.currency)
        throw InvalidCurrencyExpcetion();
    if (!std::isfinite(amount.amount) || amount.amount <= 0)
        throw std::invalid_argument("Trying to transfer an invalid amount");
    if (amount.amount > accountBalance.amount)
        throw InsufficientFundsException();

    // Convert to ENC if target account currency is different
    Amount transferred = amount;
    if (id.account->accountBalance.currency != accountBalance.currency) {
        auto buying_rate =
            bank.exchangeTable().getBuyingRate(amount.currency);
        transferred = Amount(amount.amount * buying_rate, Currency::ENC);
    }

    // Add transfer to history
    auto transfer = transaction_ptr_type(new Transfer(-amount, *this,
                                 *id.account, description));
    accountHistory.push_back(std::move(transfer));

    bool charged = false;
    try {
        chargeTransferFee();
        charged = true;
        // Send money
        auto recipient_transfer = Transfer(transferred, *this,
                                           *id.account, description);
        id.account->acceptTransfer(recipient_transfer, transferred);
    } catch (...) {
        if (charged) revertFee(transferCharge);
        accountHistory.pop_back();
        throw;
    }

    accountBalance -= amount;
}

void Account::acceptTransfer(Transfer t, Amount transferred) {
    // Add transaction to history
    accountHistory.push_back(transaction_ptr_type(new Transfer(t)));

    // Convert to account currency
    double to_add = transferred.amount;
    if (transferred.currency != accountBalance.currency) {
        to_add /= bank.exchangeTable().getSellingRate(accountBalance.currency);
    }

    accountBalance.amount += to_add;
}

void DepositWithdrawAccount::deposit(double amount) {
    deposit(Amount(amount, accountBalance.currency));
}

void DepositWithdrawAccount::deposit(Amount amount) {
    if (amount.currency != accountBalance.currency)
        throw InvalidCurrencyExpcetion();
    if (!std::isfinite(amount.amount) || amount.amount <= 0)
        throw std::invalid_argument("Trying to deposit an invalid amount");

    accountHistory.push_back(transaction_ptr_type(new Deposit(amount)));
    accountBalance += amount;
}

void DepositWithdrawAccount::withdraw(double amount) {
    withdraw(Amount(amount, accountBalance.currency));
}

void DepositWithdrawAccount::withdraw(Amount amount) {
    if (amount.currency != accountBalance.currency)
        throw InvalidCurrencyExpcetion();
    if (!std::isfinite(amount.amount) || amount.amount <= 0)
        throw std::invalid_argument("Trying to withdraw an invalid amount");
    if (amount.amount > accountBalance.amount)
        throw InsufficientFundsException();

    accountHistory.push_back(transaction_ptr_type(new Withdrawal(-amount)));
    accountBalance -= amount;
}

CheckingAccount::CheckingAccount(number_type number,
                                 const Citizen& accountHolder, Bank& bank,
                                 Currency c)
    : DepositWithdrawAccount(number, accountHolder, bank, c) {
    if (c != Currency::ENC) throw InvalidCurrencyExpcetion();
}

SavingAccount::SavingAccount(number_type number, const Citizen& accountHolder,
                             Bank& bank, Currency c)
    : Account(number, accountHolder, bank, c) {
    if (c != Currency::ENC) throw InvalidCurrencyExpcetion();
}

CurrencyAccount::CurrencyAccount(number_type number,
                                 const Citizen& accountHolder, Bank& bank,
                                 Currency c)
    : DepositWithdrawAccount(number, accountHolder, bank, c) {
    if (c == Currency::ENC) throw InvalidCurrencyExpcetion();
}

void CurrencyAccount::deposit(Amount amount) {
    if (!std::isfinite(amount.amount) || amount.amount <= 0)
        throw std::invalid_argument("Trying to deposit an invalid amount");

    double to_add = amount.amount;

    // Check the currency and convert if necessary
    if (amount.currency == Currency::ENC) {
        to_add /= bank.exchangeTable().getSellingRate(accountBalance.currency);
    } else if (amount.currency != accountBalance.currency) {
        throw InvalidCurrencyExpcetion();
    }

    accountHistory.push_back(transaction_ptr_type(new Deposit(amount)));
    accountBalance.amount += to_add;
}

void CurrencyAccount::withdraw(Amount amount) {
    if (!std::isfinite(amount.amount) || amount.amount <= 0)
        throw std::invalid_argument("Trying to withdraw an invalid amount");

    double to_substract = amount.amount;

    // Check the currency and convert if necessary
    if (amount.currency == Currency::ENC) {
        to_substract /= bank.exchangeTable().getBuyingRate(accountBalance.currency);
    } else if (amount.currency != accountBalance.currency) {
        throw InvalidCurrencyExpcetion();
    }
    if (to_substract > accountBalance.amount)
        throw InsufficientFundsException();

    accountHistory.push_back(transaction_ptr_type(new Withdrawal(-amount)));
    accountBalance.amount -= to_substract;
}

Account::AccountId::operator std::string() const { return account->stringId(); }

Account::AccountId::AccountId(const std::string& id) { setAccount(id); }

Account::AccountId::AccountId(const char* str) {
    if (str == nullptr)
        throw std::invalid_argument(
            "Constructing account id from a null pointer");
    setAccount(str);
}

void Account::AccountId::setAccount(const std::string& id) {
    InterstellarBankCommission::number_type bank_number;
    Bank::number_type account_number;
    std::istringstream stream;
    auto split = id.find_first_of("-");

    stream.str(id.substr(0, split));
    stream >> bank_number;
    if (!stream)
        throw std::invalid_argument("Passed string is not a valid account id");

    stream.str(id.substr(split + 1));
    stream >> account_number;
    if (!stream)
        throw std::invalid_argument("Passed string is not a valid account id");

    account = gkb().getBank(bank_number).getAccount(account_number);
}

std::ostream& operator<<(std::ostream& stream, const Account::AccountId& id) {
    return stream << std::string(id);
}

std::ostream& operator<<(std::ostream& stream,
                         const Account::AccountHistory& hist) {
    for (auto& it : hist.history) {
        stream << *it << std::endl;
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Account& account) {
    stream << "ACCOUNT: " << account.stringId() << std::endl;
    stream << "balance: " << account.accountBalance << std::endl;
    stream << "history:" << std::endl;
    stream << account.history();
    return stream;
}
