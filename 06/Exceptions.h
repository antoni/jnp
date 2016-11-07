#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

class CitizenNotFoundException : public std::exception {
   public:
    CitizenNotFoundException() = default;
    virtual const char* what() const noexcept {
        return "Citizen was not found on the planet.";
    }
};

class NoSuchAccountException : public std::exception {
   public:
    NoSuchAccountException() = default;
    virtual const char* what() const noexcept {
        return "Trying to perform a transaction on a nonexisting account.";
    }
};

class NoSuchBankException : public std::exception {
   public:
    NoSuchBankException() = default;
    virtual const char* what() const noexcept {
        return "Trying to find a nonexisting bank.";
    }
};

class CannotRegisterCitizenException : public std::exception {
   public:
    CannotRegisterCitizenException() = default;
    virtual const char* what() const noexcept {
        return "Could not register new citizen.";
    }
};

class InvalidCurrencyExpcetion : public std::exception {
    virtual const char* what() const noexcept { return "Invalid currency"; }
};

class InsufficientFundsException : public std::exception {
    virtual const char* what() const noexcept {
        return "Insufficient funds to perform an operation";
    }
};

class NoCurrencyChosenException : public std::exception {
    virtual const char* what() const noexcept {
        return "Trying to edit an exchange rate, but no currency has been "
               "chosen!";
    }
};

class NoAccountChosenException : public std::exception {
    virtual const char* what() const noexcept {
        return "Trying to edit an account, but no account type has been "
               "chosen!";
    }
};

#endif /* EXCEPTIONS_H */
