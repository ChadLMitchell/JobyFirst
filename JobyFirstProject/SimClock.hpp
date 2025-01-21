//
//  SimClock.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef SimClock_hpp
#define SimClock_hpp

#include <stdio.h>
#include <iostream>

class EventHandler {
protected:
    long nextEventTime;
    long eventCategory; // for expansion, not currently used
public:
    EventHandler();
    EventHandler(long nextEventTime);
    virtual ~EventHandler();
    long getNextEventTime();
    void setNextEventTime(long aTime);
    virtual bool handleEvent(long currentTime);
};

class SimClock {
    long endTime;
    long currentTime;
    bool needSort;
    std::vector<std::shared_ptr<EventHandler>> eventHandlers;
        // TO DO: consider using a list for performance later
    void sortHandlers();
public:
    SimClock(long endTime);
    ~SimClock();
    long getTime();
    bool addHandler(std::shared_ptr<EventHandler> aHandler);
    void markNeedSort();
    bool run();
};

bool testSimClock(long howLongSeconds);

extern std::shared_ptr<SimClock> theSimClock;
#endif /* SimClock_hpp */
