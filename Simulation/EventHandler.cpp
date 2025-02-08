//
//  EventHandler.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#include "EventHandler.hpp"

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
int EventHandler::NextHandlerNumber = 1; // We use this static member to keep track of next number to assign
EventHandler::EventHandler(long nextEventTime):  nextEventTime{nextEventTime} {
    handlerNumber = NextHandlerNumber++;
}

EventHandler::~EventHandler() {
}

// Set protected variable (for testing since children access it directly)
void EventHandler::setNextEventTime(long aTime) {
    nextEventTime = aTime;
}

// When the CurrentTime reaches nextEventTime SimClock callse this to process the event.
// If this function returns true, the handler is re-inserted into the SimClock vector.
// If this function returns false, it is removed from the SimClock vector.
// ChargerQueue and PlaneQueue always return true except when shutting down a simulation.
// Flights may return false if the flight is no longer needed after the event.
bool EventHandler::handleEvent(long currentTime, bool closeOut) {
    // The generic default handler just askes that it be removed from the vector
    return false;
}

// For testing: count how may planes are in this handler
long EventHandler::countPlanes() {
    // The generic default handler contains no planes
    return 0;
}

// For testing: provide a description of the object
const std::string EventHandler::describe() {
    std::string description = "Generic EventHandler";
    return description;
}
