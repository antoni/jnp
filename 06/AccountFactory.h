#ifndef ACCOUNT_FACTORY_H
#define ACCOUNT_FACTORY_H

#include "Account.h"
#include "Currency.h"

class AccountFactory {
    friend class BankBuilder;

   public:
    using account_ptr_type = std::unique_ptr<Account>;

   private:
    double monthlyCharge;
    double transferCharge;
    double interestRate;

   protected:
    void setRates(account_ptr_type& a) const;

   public:
    virtual account_ptr_type makeAccount(Account::number_type number,
                                         const Citizen& citizen, Bank& bank,
                                         Currency currency) const = 0;
};

class CheckingAccountFactory : public AccountFactory {
   public:
    account_ptr_type makeAccount(Account::number_type number,
                                 const Citizen& citizen, Bank& bank,
                                 Currency currency) const;
};

class SavingAccountFactory : public AccountFactory {
   public:
    account_ptr_type makeAccount(Account::number_type number,
                                 const Citizen& citizen, Bank& bank,
                                 Currency currency) const;
};

class CurrencyAccountFactory : public AccountFactory {
   public:
    account_ptr_type makeAccount(Account::number_type number,
                                 const Citizen& citizen, Bank& bank,
                                 Currency currency) const;
};

#endif /* ACCOUNT_FACTORY_H */
