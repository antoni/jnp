#include <iomanip>
#include <string>
#include <sstream>
#include "Amount.h"
#include "Exceptions.h"

Amount& Amount::operator+=(const Amount& other) {
    if (other.currency != currency) throw InvalidCurrencyExpcetion();
    amount += other.amount;
    return *this;
}

Amount& Amount::operator-=(const Amount& other) {
    if (other.currency != currency) throw InvalidCurrencyExpcetion();
    amount -= other.amount;
    return *this;
}

Amount Amount::operator-() { return Amount(-amount, currency); }

Amount::operator std::string() const {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << amount << currency;
    return stream.str();
}

bool Amount::operator==(const std::string& stringValue) const {
    return std::string(*this) == stringValue;
}

std::ostream& operator<<(std::ostream& stream, const Amount& a) {
    auto old_precision = stream.precision();
    stream << std::setprecision(2);
    stream << std::fixed;
    stream << a.amount << a.currency;
    stream.precision(old_precision);
    return stream;
}
