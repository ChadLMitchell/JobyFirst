//
//  SimClock.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef SimClock_hpp
#define SimClock_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "EventHandler.hpp"
#include "Simulation.hpp"

/*
 *******************************************************************************************
 * Class SimClock
 * This manages a vector of EventHandlers. The vector is kept sorted so that the handler
 * with the earliest time is at the end ready to be popped off.
 *
 * This uses a "quantum" clock meaning that the time jumps from one meaningful time to the
 * next without passing through the times in between. It required all EventHandlers to be able
 * to pre-calculate when the next event they will care about will happen so the SimClock
 * can skip directly to the next time that matters.
 *
 * All handlers will be descendants of EventHandler.
 *
 * Once everything is set up, the simulation proceeds by calling the run() in this object.
 * never wait for passengers, then this is a place to create the initial collection of planes.
 * It is also a place to put planes that are grounded due to faults if that option is chosen.
 *
 * This is a child of eventHandler. As such, it will be added to the SimClock queue.
 *
 * A Simulation object will contain exactly one SimClock.
 * If the PlaneQueue object is in a Simulation it has a pointer to that Simulation object.
 * For testing, a PlaneQueue may have a nullptr for theSimulation property.
 *******************************************************************************************
 */
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
