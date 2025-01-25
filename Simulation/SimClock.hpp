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
#include <queue>
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
 * next without passing through the times in between. It requires all EventHandlers to be
 * able to pre-calculate when the next event they will care about will happen so the
 * SimClock can skip directly to the next time that matters.
 *
 * All handlers will be descendants of EventHandler.
 *
 * Once everything is set up, the simulation proceeds by calling the run() in this object.
 * That function will loop until time runs out for the simulation or until there are no
 * more EventHandlers listed.
 *******************************************************************************************
 */
class SimClock {
    Simulation *theSimulation; // Simulation object containing current simulation or nullptr
    long endTime; // When does this simulation end?
    long currentTime; // The current clock time
    bool needSort; // Set if another object might cause the handler queue to become unsorted.
                    // It is checked at the start of each clock loop inside run().
    std::vector<std::shared_ptr<EventHandler>> eventHandlers; // The sorged list of handlers
    
    // This function is private so only this object can call it at times that are safe
    void sortHandlers();
public:
    SimClock(Simulation *theSimulation, long endTime);
    ~SimClock();
    
    // Get the current clock time
    long getTime();
    
    // Add a handler to the vector, inserting it in sorted order
    void addHandler(std::shared_ptr<EventHandler> aHandler);
    
    // Remove and re-add this handler. Probably faster than a full sort
    void reSortHandler(std::shared_ptr<EventHandler> aHandler);
    
    // Set the private property indicating a sort is needed
    void markNeedSort();
    
    // For testing: check if the vector is properly sorted
    bool checkSort(); // for testing
    
    // The core loop of the simulation clock. If verbose, provide more details to cout during execution.
    bool run(bool verbose);
    
    // For testing: sum all the planes in all of the handlers
    long countPlanes();
};

// Test the class to validate some functionality
bool testSimClock(bool longTest);

#endif /* SimClock_hpp */
