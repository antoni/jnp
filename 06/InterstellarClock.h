#ifndef INTERSTELLAR_CLOCK_H
#define INTERSTELLAR_CLOCK_H

using Date = unsigned long long;
using Time = unsigned short;

// Czas międzyplanetarny liczymy z dokładnością do godziny.
// Nowoutworzony zegar międzyplanetarny będzie wskazywał czas 0 i dzień 0.
class InterstellarClock {
   public:
    // Doba międzyplanetarna ma 20 godzin (0-19).
    static const unsigned HOURS_IN_DAY = 20;

    // Miesiąc międzyplanetarny ma 30 dni.
    static const unsigned DAYS_IN_MONTH = 30;

    virtual ~InterstellarClock() {}

    // Zwraca liczbę godzin od początku założenia ZFP.
    unsigned long long hours() const;

    // Zwraca bieżącą godzinę (0-19).
    Time time() const;

    // Zwraca bieżący datę, tj. dzień od początku założenia ZFP.
    Date date() const;

    // Przesuwa zegar o godzinę.
    virtual InterstellarClock& nextHour();

    // Przesuwa zegar o dobę.
    virtual InterstellarClock& nextDay();

    // Przesuwa zegar o miesiąc (miesiąc ma 30 dni).
    virtual InterstellarClock& nextMonth();

   private:
    // liczba godzin od początku założenia ZFP
    unsigned long long _hours;
};

#endif /* INTERSTELLAR_CLOCK_H */
