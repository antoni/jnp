#include <algorithm>
#include "MonthObserver.h"
#include "ObservableInterstellarClock.h"

void ObservableInterstellarClock::notifyObservers() {
    if (time() == DAY_START_TIME &&
        date() % ObservableInterstellarClock::DAYS_IN_MONTH == CHARGE_DAY)
        for (auto& observer : observers) observer->notifyTimeChanged();
}

void ObservableInterstellarClock::registerObserver(observer_ptr_type observer) {
    observers.emplace_back(observer);
}

void ObservableInterstellarClock::unregisterObserver(
    observer_ptr_type observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer),
                    observers.end());
}

ObservableInterstellarClock& ObservableInterstellarClock::nextHour() {
    InterstellarClock::nextHour();
    notifyObservers();
    return *this;
}

ObservableInterstellarClock& interstellarClock() {
    static ObservableInterstellarClock clock;
    return clock;
}
