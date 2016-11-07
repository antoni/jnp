#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Amount.h"
#include "Citizen.h"
#include "ObservableInterstellarClock.h"

#include <ostream>

class Account;

class Transaction {
   protected:
    virtual std::string name() const = 0;

   public:
    const Amount amount;
    const Date date;
    Transaction(Amount amount)
        : amount(amount), date(interstellarClock().date()){};
    Transaction(const Transaction& t) noexcept : amount(t.amount),
                                                 date(t.date){};
    Transaction(Transaction&& t) noexcept : amount(t.amount), date(t.date){};

    virtual ~Transaction(){};

    friend std::ostream& operator<<(std::ostream& stream, const Transaction& t);
};

class Deposit : public Transaction {
   protected:
    std::string name() const override { return "DEPOSIT"; }

   public:
    Deposit(Amount amount) : Transaction(amount){};
};

class Withdrawal : public Transaction {
   protected:
    std::string name() const override { return "WITHDRAWAL"; }

   public:
    Withdrawal(Amount amount) : Transaction(amount){};
};

class Transfer : public Transaction {
   protected:
    std::string name() const override;
    const Account& from;
    const Account& to;
    std::string description;

   public:
    Transfer(Amount amount, const Account& from, const Account& to,
             const std::string& description)
        : Transaction(amount), from(from), to(to), description(description){};
};

class Charge : public Transaction {
   protected:
    std::string name() const override { return "CHARGE"; }

   public:
    Charge(Amount amount) : Transaction(amount){};
};

class Interest : public Transaction {
   protected:
    std::string name() const override { return "INTEREST"; }

   public:
    Interest(Amount amount) : Transaction(amount){};
};

std::ostream& operator<<(std::ostream& stream, const Transaction& t);

#endif /* TRANSACTION_H */
