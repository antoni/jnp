#include "InterstellarBankCommission.h"
#include "MonthObserver.h"
#include "Bank.h"
#include "BankBuilder.h"

void InterstellarBankCommission::registerBankObserver(Bank* bank) {
    MonthObserver* observer = dynamic_cast<MonthObserver*>(bank);
    interstellarClock().registerObserver(observer);
}

InterstellarBankCommission& gkb() {
    static InterstellarBankCommission commission;
    return commission;
}

BankBuilder InterstellarBankCommission::bankApplication() {
    return BankBuilder(*this);
}

Bank& InterstellarBankCommission::getBank(number_type number) {
    if (number >= banks.size()) throw NoSuchBankException();
    return *banks[number];
}
