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
#include "Simulation.hpp"

class EventHandler {
protected:
    long nextEventTime;
public:
    EventHandler();
    EventHandler(long nextEventTime);
    virtual ~EventHandler();
    long getNextEventTime();
    void setNextEventTime(long aTime);
    virtual bool handleEvent(long currentTime);
    virtual long countPlanes();
};
class SimClock {
    Simulation *theSimulation;
    long endTime;
    long currentTime;
    bool needSort;
    std::vector<std::shared_ptr<EventHandler>> eventHandlers;
        // TO DO: consider using a list for performance later
    void sortHandlers();
public:
    SimClock(Simulation *theSimulation, long endTime);
    ~SimClock();
    long getTime();
    void addHandler(std::shared_ptr<EventHandler> aHandler);
    void reSortHandler(std::shared_ptr<EventHandler> aHandler);
    void markNeedSort();
    bool run();
    long countPlanes();
};

bool testSimClock(long howLongSeconds);

#endif /* SimClock_hpp */
