#ifndef AMOUNT_H
#define AMOUNT_H

#include <ostream>
#include <string>
#include "Currency.h"

class Amount {
   public:
    double amount;
    Currency currency;

    Amount(double amount, Currency currency)
        : amount(amount), currency(currency){};

    Amount& operator+=(const Amount& other);
    Amount& operator-=(const Amount& other);
    Amount operator-();
    operator std::string() const;
    bool operator==(const std::string& stringValue) const;
};

std::ostream& operator<<(std::ostream& stream, const Amount& a);

#endif /* AMOUNT_H */
