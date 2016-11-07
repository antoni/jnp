#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <cstddef>
#include <memory>
#include <list>
#include "Amount.h"
#include "Citizen.h"
#include "Currency.h"
#include "Exceptions.h"
#include "Transaction.h"

class Bank;

class Account {
    friend class AccountFactory;
    friend class Bank; /* to charge fees privately */
   public:
    using number_type = std::size_t;
    virtual ~Account(){};

   private:
    void capitalizeInterests();
    void chargeMonthlyCharge();
    void chargeTransferFee();
    /* amount in ENC */
    void chargeFee(double amount);
    void revertFee(double amount);

   protected:
    using transaction_ptr_type = std::unique_ptr<Transaction>;
    using history_container = std::list<transaction_ptr_type>;

    number_type number;
    const Citizen& accountHolder;
    Bank& bank;
    Amount accountBalance;
    double monthlyCharge;
    double transferCharge;
    double interestRate;
    history_container accountHistory;

    virtual void acceptTransfer(Transfer t, Amount transferred);

   public:
    class AccountId {
        friend class Account;
        Account* account;
        AccountId(Account* account) : account(account){};
        operator std::string() const;
        void setAccount(const std::string& str);

       public:
        AccountId(const std::string& str);
        AccountId(const char* str);
        friend std::ostream& operator<<(std::ostream& stream,
                                        const AccountId& id);
    };

    class AccountHistory {
        friend class Account;
        const history_container& history;
        AccountHistory(const history_container& history) : history(history){};
        friend std::ostream& operator<<(std::ostream& stream,
                                        const AccountHistory& hist);
    };

    using id_type = AccountId;
    using history_type = AccountHistory;

    Account(number_type number, const Citizen& accountHolder, Bank& bank,
            Currency c);

    Amount balance() const;
    id_type id();
    std::string stringId() const;

    void transfer(Amount amount, id_type id);
    void transfer(double amount, id_type id);
    void transfer(double amount, id_type id, const std::string& description);
    virtual void transfer(Amount amount, id_type id,
                          const std::string& description);

    AccountHistory history() const;

    friend std::ostream& operator<<(std::ostream& stream,
                                    const Account& account);
};

class DepositWithdrawAccount : public Account {
   public:
    virtual void deposit(Amount amount);
    virtual void withdraw(Amount amount);
    void deposit(double amount);
    void withdraw(double amount);

    DepositWithdrawAccount(number_type number, const Citizen& accountHolder,
                           Bank& bank, Currency c)
        : Account(number, accountHolder, bank, c){};
};

class CheckingAccount : public DepositWithdrawAccount {
   public:
    CheckingAccount(number_type number, const Citizen& accountHolder,
                    Bank& bank, Currency c);
};

class SavingAccount : public Account {
   public:
    SavingAccount(number_type number, const Citizen& accountHolder, Bank& bank,
                  Currency c);
};

class CurrencyAccount : public DepositWithdrawAccount {
   public:
    CurrencyAccount(number_type number, const Citizen& accountHolder,
                    Bank& bank, Currency c);

    using DepositWithdrawAccount::deposit;
    using DepositWithdrawAccount::withdraw;
    void deposit(Amount amount) override;
    void withdraw(Amount amount) override;
};

#endif /* ACCOUNT_H */
