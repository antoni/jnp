#ifndef CITIZEN_H
#define CITIZEN_H

#include <iostream>
#include <memory>
#include <string>
#include "Exceptions.h"

class CitizenId {
   public:
    using citizen_id_type = unsigned long long;

   protected:
    citizen_id_type citizenId;

   public:
    /* Constructor intended to be used only from findCitizen(std::string) method
     */
    CitizenId(const std::string& idString) {
        try {
            citizenId = std::stoull(idString);
        } catch (...) {
            throw CitizenNotFoundException();
        }
    };

    CitizenId(citizen_id_type id) : citizenId(id){};
    CitizenId(){};

    CitizenId& operator++() {
        citizenId++;
        return *this;
    }

    CitizenId& operator++(int) {
        citizenId++;
        return *this;
    }

    CitizenId& operator=(citizen_id_type value) {
        citizenId = value;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& stream, const CitizenId& id) {
        stream << id.id();
        return stream;
    }

    bool operator==(const CitizenId& other) const { return other.id() == id(); }
    operator std::string() const { return std::to_string(citizenId); }
    citizen_id_type id() const { return citizenId; }
};

/* Template specialization for std::hash required to use std::unordered_*
 * data structures */
namespace std {
template <>
struct hash<CitizenId> {
    size_t operator()(const CitizenId& citizenId) const {
        return std::hash<CitizenId::citizen_id_type>()(citizenId.id());
    }
};
}

class Citizen {
    friend class DoubleBynausCitizen;

   public:
    static CitizenId currentId;

   protected:
    std::string citizenName;
    CitizenId citizenId;

   public:
    Citizen(){};
    Citizen(const std::string& name) : citizenName(name), citizenId(currentId) {
        if (name.empty()) throw CannotRegisterCitizenException();

        currentId++;
    };

    bool operator==(const Citizen& other) const { return other.id() == id(); }
    bool operator!=(const Citizen& other) const { return !(*this == other); }

    virtual const CitizenId& id() const { return citizenId; }
    virtual std::string name() const { return citizenName; }
    virtual ~Citizen() {}
};

class EarthCitizen : public Citizen {
   public:
    using Citizen::id;
    using Citizen::Citizen;
};

class QonosCitizen : public Citizen {
   public:
    using Citizen::id;
    using Citizen::Citizen;
};

/* Base BynausCitzen by default has inherited std::string constructor */
class BynausCitizen : public Citizen {
   public:
    using Citizen::id;
    using Citizen::Citizen;
};

/* Composite pattern: DoubleBynausCitizen can be composed of two Bynaus citizens
 * (or any of its subclasses */
class DoubleBynausCitizen : public BynausCitizen {
   private:
    std::shared_ptr<BynausCitizen*> citizen1;
    std::shared_ptr<BynausCitizen*> citizen2;

   public:
    using Citizen::id;

    DoubleBynausCitizen(BynausCitizen* c1, BynausCitizen* c2)
        : citizen1(std::make_shared<BynausCitizen*>(c1)),
          citizen2(std::make_shared<BynausCitizen*>(c2)) {
        this->citizenId = Citizen::currentId;
        Citizen::currentId++;
    };

    virtual std::string name() const;
};

#endif /* CITIZEN_H */
