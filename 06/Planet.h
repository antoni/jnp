#ifndef PLANET_H
#define PLANET_H

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <string>
#include "Citizen.h"
#include "Exceptions.h"

class DefaultCitizenRegisterInterface {
   protected:
    /* We are using pointers to avoid object slicing when downcasting */
    using citizen_ptr_type = Citizen*;
    using citizen_ret_type = Citizen&;
    using citizens_map_type = std::unordered_map<CitizenId, citizen_ptr_type>;
    using citizens_it_type = citizens_map_type::iterator;
    using citizen_entry_type = std::pair<CitizenId, citizen_ptr_type>;

   public:
    /* Registers citizen by name */
    virtual citizen_ret_type registerCitizen(const std::string& name) = 0;

   protected:
    /* Registers citizen taking Citizen subclass pointer as argument */
    virtual citizen_ret_type registerCitizen(citizen_ptr_type c) = 0;
};

class Planet : protected DefaultCitizenRegisterInterface {
   protected:
    citizens_map_type citizens;

    virtual citizen_ret_type registerCitizen(citizen_ptr_type citizen);
 
  public:
    Planet(){};
    virtual ~Planet() {
        std::for_each(
            citizens.begin(), citizens.end(),
            [](citizen_entry_type entry) -> void { delete entry.second; });
    };

    virtual citizen_ret_type registerCitizen(const std::string& name) = 0;

    Citizen& findCitizen(const std::string& idString);
    Citizen& findCitizen(CitizenId citizenId);
};

class Earth : public Planet {
   private:
    citizen_ret_type registerCitizen(citizen_ptr_type citizen) {
        return Planet::registerCitizen(citizen);
    }

   public:
    citizen_ret_type registerCitizen(const std::string& name) {
        return registerCitizen(new EarthCitizen(name));
    }
};

class Qonos : public Planet {
   private:
    citizen_ret_type registerCitizen(citizen_ptr_type citizen) {
        return Planet::registerCitizen(citizen);
    }

   public:
    citizen_ret_type registerCitizen(const std::string& name) {
        return registerCitizen(new QonosCitizen(name));
    }
};

class Bynaus : public Planet {
   private:
    citizen_ret_type registerCitizen(citizen_ptr_type citizen) {
        return Planet::registerCitizen(citizen);
    }

   public:
    citizen_ret_type registerCitizen(const std::string& name) {
        return registerCitizen(new BynausCitizen(name));
    }

    citizen_ret_type registerCitizen(Citizen& citizen1, Citizen& citizen2);
};

#endif /* PLANET_H */
