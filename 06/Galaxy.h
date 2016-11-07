#ifndef GALAXY_H
#define GALAXY_H

#include "Planet.h"

/* Static planet objects */

static Earth& earth() {
    static Earth earth;
    return earth;
}

static Qonos& qonos() {
    static Qonos qonos;
    return qonos;
}

static Bynaus& bynaus() {
    static Bynaus bynaus;
    return bynaus;
}

#endif /* GALAXY_H */
