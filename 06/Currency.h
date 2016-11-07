#ifndef CURRENCY_H
#define CURRENCY_H

#include <cstddef>
#include <ostream>

enum class Currency {
    ENC = 0, /* energy credits */
    BIC = 1, /* binary credit */
    DIL = 2, /* dilitium */
    LIT = 3, /* litium */
};

std::ostream& operator<<(std::ostream& stream, Currency currency);

struct EnumCurrencyHash {
    std::size_t operator()(Currency c) const {
        return static_cast<std::size_t>(c);
    }
};

#endif /* CURRENCY_H */
