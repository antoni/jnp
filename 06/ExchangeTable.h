#ifndef EXCHANGETABLE_H
#define EXCHANGETABLE_H

#include <cmath>
#include <unordered_map>
#include <utility>
#include "Currency.h"
#include "Exceptions.h"

class ExchangeRatePair : private std::pair<double, double> {
   public:
    /* Selling and buying rates are by default both equal 1.0 */
    ExchangeRatePair() : std::pair<double, double>({1.0, 1.0}){};

    ExchangeRatePair& buyingRate(double rate) {
        this->first = rate;
        return *this;
    }

    ExchangeRatePair& sellingRate(double rate) {
        this->second = rate;
        return *this;
    }

    double getBuyingRate() { return this->first; }
    double getSellingRate() { return this->second; }
};

class ExchangeTable {
   private:
    std::unordered_map<Currency, ExchangeRatePair, EnumCurrencyHash>
        exchange_table;
    ExchangeRatePair* editedExchangePair;

   public:
    ExchangeTable& exchangeRate(Currency currency) {
        if (currency == Currency::ENC)
            throw std::invalid_argument("Can't set exchange rate of ENC!");

        auto currency_it = exchange_table.find(currency);
        if (currency_it == exchange_table.end()) {
            std::tie(currency_it, std::ignore) =
                exchange_table.insert({currency, ExchangeRatePair()});
        }
        editedExchangePair = &(currency_it->second);
        return *this;
    }

    ExchangeTable& buyingRate(double rate) {
        if (editedExchangePair == nullptr) throw NoCurrencyChosenException();
        if (!std::isfinite(rate) || rate <= 0)
            throw std::invalid_argument("Invalid buying rate");

        editedExchangePair->buyingRate(rate);
        return *this;
    }

    ExchangeTable& sellingRate(double rate) {
        if (editedExchangePair == nullptr) throw NoCurrencyChosenException();
        if (!std::isfinite(rate) || rate <= 0)
            throw std::invalid_argument("Invalid selling rate");

        editedExchangePair->sellingRate(rate);
        return *this;
    }

    double getBuyingRate(Currency currency) {
        auto currency_it = exchange_table.find(currency);
        if (currency_it == exchange_table.end())
            std::tie(currency_it, std::ignore) =
                exchange_table.insert({currency, ExchangeRatePair()});

        return currency_it->second.getBuyingRate();
    }

    double getSellingRate(Currency currency) {
        auto currency_it = exchange_table.find(currency);
        if (currency_it == exchange_table.end())
            std::tie(currency_it, std::ignore) =
                exchange_table.insert({currency, ExchangeRatePair()});

        return currency_it->second.getSellingRate();
    }
};

#endif /* EXCHANGETABLE_H */
