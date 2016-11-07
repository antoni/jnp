#include "Currency.h"

std::ostream& operator<<(std::ostream& stream, Currency currency) {
    switch (currency) {
        case Currency::ENC:
            stream << "ENC";
            break;
        case Currency::BIC:
            stream << "BIC";
            break;
        case Currency::DIL:
            stream << "DIL";
            break;
        case Currency::LIT:
            stream << "LIT";
            break;
    }
    return stream;
}
