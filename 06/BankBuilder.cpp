#include "BankBuilder.h"
#include "InterstellarBankCommission.h"

BankBuilder& BankBuilder::name(const std::string& newName) {
    bankName = newName;
    return *this;
}

BankBuilder& BankBuilder::monthlyCharge(double charge) {
    if (!std::isfinite(charge) || charge < 0)
        throw std::invalid_argument("Invalid monthly charge");
    editedFactory->monthlyCharge = charge;
    return *this;
}

BankBuilder& BankBuilder::transferCharge(double charge) {
    if (!std::isfinite(charge) || charge < 0)
        throw std::invalid_argument("Invalid transfer charge");
    editedFactory->transferCharge = charge;
    return *this;
}

BankBuilder& BankBuilder::interestRate(double rate) {
    if (!std::isfinite(rate) || rate < 0)
        throw std::invalid_argument("Invalid interest rate");
    editedFactory->interestRate = rate;
    return *this;
}

BankBuilder& BankBuilder::savingAccount() {
    editedFactory = &savingAccountFactory;
    return *this;
}

BankBuilder& BankBuilder::checkingAccount() {
    editedFactory = &checkingAccountFactory;
    return *this;
}

BankBuilder& BankBuilder::currencyAccount() {
    editedFactory = &currencyAccountFactory;
    return *this;
}

Bank& BankBuilder::createBank() {
    return commission.createBank(bankName, checkingAccountFactory,
                                 savingAccountFactory, currencyAccountFactory);
}
