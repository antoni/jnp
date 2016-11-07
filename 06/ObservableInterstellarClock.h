#ifndef OBSERVABLEINTERSTELLARCLOCK_H
#define OBSERVABLEINTERSTELLARCLOCK_H

#include <list>
#include <vector>
#include "InterstellarClock.h"
#include "MonthObserver.h"

/* Observer pattern.
 * Interface for (un)registering and notifying observers */
class MonthSubject {
   protected:
    using observer_ptr_type = MonthObserver*;
    /* Capitalize accout interests and charge montly charges if
     * we are at day 0, time 0 */

    /* Time when the day starts */
    static const unsigned DAY_START_TIME = 0;

    /* Day of the month when fees are charged */
    static const unsigned CHARGE_DAY = 0;

   public:
    virtual void notifyObservers() = 0;
    virtual void registerObserver(observer_ptr_type observer) = 0;
    virtual void unregisterObserver(observer_ptr_type observer) = 0;
};

class ObservableInterstellarClock : public InterstellarClock,
                                    public MonthSubject {
   private:
    std::vector<observer_ptr_type> observers;

    virtual void notifyObservers() override;

   public:
    ObservableInterstellarClock(){};
    virtual ~ObservableInterstellarClock(){};

    virtual void registerObserver(observer_ptr_type observer) override;
    virtual void unregisterObserver(observer_ptr_type observer) override;

    virtual ObservableInterstellarClock& nextHour() override;
};

ObservableInterstellarClock& interstellarClock();

#endif /* OBSERVABLEINTERSTELLARCLOCK_H */
