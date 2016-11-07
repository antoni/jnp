#ifndef BANK_BUILDER_H
#define BANK_BUILDER_H

#include <list>
#include "AccountFactory.h"
#include "Bank.h"

class InterstellarBankCommission;

class BankBuilder {
    friend class InterstellarBankCommission;

   private:
    std::string bankName;
    CheckingAccountFactory checkingAccountFactory;
    SavingAccountFactory savingAccountFactory;
    CurrencyAccountFactory currencyAccountFactory;
    AccountFactory* editedFactory;
    InterstellarBankCommission& commission;

   protected:
    BankBuilder(InterstellarBankCommission& commission)
        : bankName(),
          checkingAccountFactory(),
          savingAccountFactory(),
          currencyAccountFactory(),
          editedFactory(&checkingAccountFactory),
          commission(commission){};

   public:
    BankBuilder& name(const std::string& newName);

    BankBuilder& monthlyCharge(double charge);
    BankBuilder& transferCharge(double charge);
    BankBuilder& interestRate(double rate);

    BankBuilder& savingAccount();
    BankBuilder& checkingAccount();
    BankBuilder& currencyAccount();

    Bank& createBank();
};

#endif /* BANK_BUILDER_H */
