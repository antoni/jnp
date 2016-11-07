#ifndef BANK_H
#define BANK_H

#include <unordered_map>
#include <vector>
#include "AccountFactory.h"
#include "Citizen.h"
#include "ExchangeTable.h"
#include "MonthObserver.h"

class Bank : public MonthObserver {
    friend class InterstellarBankCommission;

   public:
    using number_type = std::size_t;

   private:
    using account_ptr_type = AccountFactory::account_ptr_type;

    number_type number;
    number_type nextAccountNumber;
    std::string name;
    CheckingAccountFactory checkingAccountFactory;
    SavingAccountFactory savingAccountFactory;
    CurrencyAccountFactory currencyAccountFactory;
    ExchangeTable rateTable;
    std::vector<account_ptr_type> accounts;
    using accounts_it_type = std::vector<account_ptr_type>::iterator;

    void capitalizeAccountInterests();
    void chargeAccountMonthlyCharges();

   protected:
    Bank(number_type number, const std::string& name,
         CheckingAccountFactory checkingAF, SavingAccountFactory savingAF,
         CurrencyAccountFactory currencyAF)
        : number(number),
          nextAccountNumber(0),
          name(name),
          checkingAccountFactory(checkingAF),
          savingAccountFactory(savingAF),
          currencyAccountFactory(currencyAF){};

    void notifyTimeChanged() override;

   public:
    CheckingAccount& openCheckingAccount(Citizen& citizen);
    SavingAccount& openSavingAccount(Citizen& citizen);
    CurrencyAccount& openCurrencyAccount(Citizen& citizen, Currency currency);
    ExchangeTable& exchangeTable() { return rateTable; }

    number_type id() { return number; }
    Account* getAccount(number_type number);
};

#endif /* BANK_H */
