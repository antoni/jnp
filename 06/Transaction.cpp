#include "Transaction.h"
#include "Account.h"

std::string Transfer::name() const {
    return "TRANSFER (" + description + ") FROM: " + from.stringId() + " TO: " +
           to.stringId();
}

std::ostream& operator<<(std::ostream& stream, const Transaction& t) {
    return stream << t.date << " " << t.amount << " " << t.name();
}
