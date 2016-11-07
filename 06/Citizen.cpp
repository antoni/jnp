#include <stdexcept>
#include "Citizen.h"

std::string DoubleBynausCitizen::name() const {
    if (citizen1.get() == nullptr || citizen2.get() == nullptr)
        throw new std::invalid_argument(
            "Citizens forming DoubleBynausCitizen cannot be null");
    return (*citizen1)->name() + "&" + (*citizen2)->name();
}

/* Id of the next citizen to add */
/* By using a static field in the Citizen upperclass, we provide uniqueness
 * of the CitizenId's in the whole Galaxy */
CitizenId Citizen::currentId = 0;
