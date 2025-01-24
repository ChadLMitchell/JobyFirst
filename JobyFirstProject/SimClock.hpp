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
#include "EventHandler.hpp"
#include "Simulation.hpp"

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
    bool checkSort(); // for testing
    bool run(bool verbose);
    long countPlanes();
};

bool testSimClock(long howLongSeconds);

#endif /* SimClock_hpp */
