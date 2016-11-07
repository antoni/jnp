#include "AccountFactory.h"

void AccountFactory::setRates(account_ptr_type& account) const {
    account->monthlyCharge = monthlyCharge;
    account->transferCharge = transferCharge;
    account->interestRate = interestRate;
}

AccountFactory::account_ptr_type CheckingAccountFactory::makeAccount(
    Account::number_type number, const Citizen& citizen, Bank& bank,
    Currency currency) const {
    auto account =
        account_ptr_type(new CheckingAccount(number, citizen, bank, currency));
    setRates(account);
    return account;
}

AccountFactory::account_ptr_type SavingAccountFactory::makeAccount(
    Account::number_type number, const Citizen& citizen, Bank& bank,
    Currency currency) const {
    auto account =
        account_ptr_type(new SavingAccount(number, citizen, bank, currency));
    setRates(account);
    return account;
}

AccountFactory::account_ptr_type CurrencyAccountFactory::makeAccount(
    Account::number_type number, const Citizen& citizen, Bank& bank,
    Currency currency) const {
    auto account =
        account_ptr_type(new CurrencyAccount(number, citizen, bank, currency));
    setRates(account);
    return account;
}
