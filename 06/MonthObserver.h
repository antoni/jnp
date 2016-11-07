#ifndef MONTHOBSERVER_H
#define MONTHOBSERVER_H

/* Observer pattern.
 * Interface for notifying banks when new month begins */
class MonthObserver {
   public:
    virtual void notifyTimeChanged() = 0;
};

#endif /* MONTHOBSERVER_H */
