#include "Planet.h"

Planet::citizen_ret_type Planet::registerCitizen(citizen_ptr_type citizen) {
    bool inserted = false;
    citizens_it_type citizens_it;

    if (citizens.find(citizen->id()) != citizens.end())
        throw CannotRegisterCitizenException();

    std::tie(citizens_it, inserted) = citizens.insert({citizen->id(), citizen});
    if (!inserted) throw CannotRegisterCitizenException();
    return *(citizens_it->second);
}

Citizen& Planet::findCitizen(const std::string& idString) {
    return findCitizen(CitizenId(idString));
}

Citizen& Planet::findCitizen(CitizenId citizenId) {
    auto found = citizens.find(citizenId);
    if (found != citizens.end()) return *(found)->second;
    throw new CitizenNotFoundException;
}

Planet::citizen_ret_type Bynaus::registerCitizen(Citizen& citizen1,
                                                 Citizen& citizen2) {
    BynausCitizen* singleCitizen1 = dynamic_cast<BynausCitizen*>(&citizen1);
    BynausCitizen* singleCitizen2 = dynamic_cast<BynausCitizen*>(&citizen2);
    if (singleCitizen1 && singleCitizen2) {
        /* We don't want to register DoubleBynausCitizen formed from
         * the same BynausCitizen */

        if (singleCitizen1->id() == singleCitizen2->id())
            throw CannotRegisterCitizenException();

        citizen_ptr_type citizen =
            new DoubleBynausCitizen(singleCitizen1, singleCitizen2);
        return registerCitizen(citizen);
    }
    /* dynamic_cast failed */
    throw new CannotRegisterCitizenException();
}
