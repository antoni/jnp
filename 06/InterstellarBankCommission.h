#ifndef INTERSTELLAR_BANK_COMMISSION_H
#define INTERSTELLAR_BANK_COMMISSION_H

#include "Bank.h"
#include "ObservableInterstellarClock.h"
#include <unordered_map>

class BankBuilder;

class InterstellarBankCommission {
    friend class BankBuilder;

   private:
    using bank_ptr_type = std::unique_ptr<Bank>;
    std::vector<bank_ptr_type> banks;

    /* Helper method to register bank observer to be notified about time changes
     */
    void registerBankObserver(Bank* bank);

    /* Creates bank and registers it to be notified when new month begins */
    template <typename... Args>
    Bank& createBank(Args&&... args) {
        auto bank =
            bank_ptr_type(new Bank(banks.size(), std::forward<Args>(args)...));
        banks.emplace_back(std::move(bank));
        registerBankObserver(banks.back().get());
        return *banks.back();
    }

   public:
    using number_type = std::size_t;

    /* Builder pattern */
    BankBuilder bankApplication();

    Bank& getBank(number_type number);
};

InterstellarBankCommission& gkb();

#endif /* INTERSTELLAR_BANK_COMMISSION_H */
