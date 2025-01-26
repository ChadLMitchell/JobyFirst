//
//  EventHandler.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#ifndef EventHandler_hpp
#define EventHandler_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <iostream>

/*
 *******************************************************************************************
 * class EventHandler
 * This is a generic parent class. No objects should be created from this class directly.
 * Children include PlaneQueue, ChargerQueue, Flight and a TestHander only used for testing.
 *
 * Any of these children can be added to a vector of eventHandlers in a SimClock object.
 * As nextEventTime arrves, the SimClock will call handleEvent() to process that event.
 *******************************************************************************************
 */
class EventHandler {
protected:
    long nextEventTime; // this is the next time this handler wants to be called
public:
    EventHandler(long nextEventTime);
    virtual ~EventHandler();
    
    // Get protected variable (for testing since children access it directly)
    long getNextEventTime();

    // Set protected variable(for testing since children access it directly)
    void setNextEventTime(long aTime);

    // When the CurrentTime reaches nextEventTime SimClock callse this to process the event.
    // If this function returns true, the handler is re-inserted into the SimClock vector.
    // If this function returns false, it is removed from the SimClock vector.
    // ChargerQueue and PlaneQueue always return true except when shutting down a simulation.
    // Flights may return false if the flight is no longer needed after the event.
    virtual bool handleEvent(long currentTime, bool closeOut);

    // For testing: count how may planes are in this handler
    virtual long countPlanes();

    // For testing: provide a description of the object
    virtual const std::string describe();
};

#endif /* EventHandler_hpp */
