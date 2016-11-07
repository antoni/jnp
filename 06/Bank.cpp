#include <iostream>
#include "Bank.h"

/* Observer pattern */
void Bank::notifyTimeChanged() {
    capitalizeAccountInterests();
    chargeAccountMonthlyCharges();
}

void Bank::capitalizeAccountInterests() {
    for (auto& account_it : accounts) account_it->capitalizeInterests();
}

void Bank::chargeAccountMonthlyCharges() {
    for (auto& account_it : accounts) account_it->chargeMonthlyCharge();
}

CheckingAccount& Bank::openCheckingAccount(Citizen& citizen) {
    auto account = checkingAccountFactory.makeAccount(accounts.size(), citizen,
                                                      *this, Currency::ENC);
    accounts.emplace_back(std::move(account));
    return dynamic_cast<CheckingAccount&>(*accounts.back());
}

SavingAccount& Bank::openSavingAccount(Citizen& citizen) {
    auto account = savingAccountFactory.makeAccount(accounts.size(), citizen,
                                                    *this, Currency::ENC);
    accounts.emplace_back(std::move(account));
    return dynamic_cast<SavingAccount&>(*accounts.back());
}

CurrencyAccount& Bank::openCurrencyAccount(Citizen& citizen,
                                           Currency currency) {
    auto account = currencyAccountFactory.makeAccount(accounts.size(), citizen,
                                                      *this, currency);
    accounts.emplace_back(std::move(account));
    return dynamic_cast<CurrencyAccount&>(*accounts.back());
}

Account* Bank::getAccount(number_type number) {
    if (number >= accounts.size()) throw NoSuchAccountException();
    return accounts[number].get();
}
