//
//  Flight.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#include "SimSettings.hpp"
#include "Flight.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"

/*
 *******************************************************************************************
 * class Flight
 * This represents a single flight and contains a plane object..
 *
 * This is a child of eventHandler. As such, it will be added to the SimClock queue.
 * It will respond to events that complete the flight or that represent faults.
 * After a flight it transfers the plane to theChargerQueue or thePlaneQueue.
 *
 * A Simulation object may contain zero or more Flight objects.
 * If the Flight object is in a Simulation it has a pointer to that Simulation object.
 * For testing, a Flight may have a nullptr for theSimulation property.
 *******************************************************************************************
 */
Flight::Flight(Simulation *theSimulation, long startTime, long passengerCount, std::shared_ptr<Plane> aPlane):
// Calculate the endtime based on startTime and the time a plane will go on a full charge
// The next faultTime is the start time of the flight plus the plane's current fault interval. If the nextFaultTime
// is beyond the end of the flight, then at the end of the flight we will adjust the plane's nextFault interval to
// subtract the time already used by the flight.
EventHandler(LONG_MAX),theSimulation{theSimulation}, startTime{startTime}, endTime{startTime+aPlane->calcTimeOnFullCharge__seconds()}, nextFaultTime{startTime+aPlane->getNextFaultInterval()}, passengerCount{passengerCount},thePlane{aPlane} {
    // Set our nextEventTime to the end of the flight or the time of our plane's next fault, whichever happens first
    nextEventTime = std::min(endTime, nextFaultTime);
    faultCount = 0;
}
Flight::~Flight() {
}
// As a child of EventHandler, Flight has a nextEventTime. Each time the SimClock
// reaches the nextEventTime, it will call handleEvent(). If handleEvent returns false,
// the SimClock will remove this object from the simulation. That will happen if the
// flight is completed or interrupted by a fatal fault. But if a fault event
// happens first and is not fatal, then this will return true to put us back on the
// SimClock for the next event.
// Handle events when the flight is done, the simulation is done or a fault occurs
bool Flight::handleEvent(long currentTime, bool closeOut) {
    // get the option for how we handle faults
    // we do it here since we need it several times below
    int faultOption = 0;
    if(theSimulation && theSimulation->theSettings) {
        faultOption = theSimulation->theSettings->faultOption;
    }
    if(currentTime == nextFaultTime) {
        // We hit a fault interval so handle the fault
        faultCount++;
        // get a new next fault time
        nextFaultTime = currentTime + thePlane->createFaultInterval();
        // The default is to record the fault and keep going
        if(faultOption == 1) { // if the option is 1 then the fault grounds the plane immediately
            recordFlight(); // record the portion of the flight completed
            if(theSimulation && theSimulation->thePlaneQueue) {
                // "this" will be deleted so the plane will be owned by the plane queue
                // We ground it by giving it an infinite delay
                // Pass false because we are calling PlaneQueue.addPlane() from Flight.handleEvent, not from PlaneQueue.handleEvent
                theSimulation->thePlaneQueue->addPlane(LONG_MAX, thePlane, false);
            }
            return false; // do not keep us in the event queue
        } else { // record fault and continue
            // The code for createFaultInterval always gives a number > 1 but it another fault
            // may still happen before the end of the flight so we need to do this min() again.
            nextEventTime = std::min(endTime, nextFaultTime);
            if(nextEventTime > currentTime) {
                return true; // add us back into the the event queue and continue the flight
            // If the nextEventTime is not in the future, that means that our current endTime
            // for the flight just happens to be at the same time as the fault we just
            // processed. We process the fault first, but by not returning on the previous line
            // we will drop through and process the end of the flight below,
            }
        }
    }
    // finish the flight
    endTime = currentTime; // update flight end time in case we ended early
 
    // Developer note: what to decrement below from the fault time is tricky and was handled incorrectly at first.
    // It was hard to figure out, but produced clear errors in the time stream. If we deducted the entire flight
    // time from a fault that started partway into this flight, we could end up with a too short or even negative
    // new interval. And processing a negative fault interval would later cause the next flight of that plane to
    // to decide that it should have processed that next fault before it started the flight. Fortunately the
    // SimClock detects and reports attempts to go backwards in time.

    // We used up the duration of the flight from the next fault interval for this plane.
    // We may have had a fault during this flight so our current interval may not have started at the beginning of the flight
    long startOfCurrentFaultInterval = nextFaultTime - thePlane->getNextFaultInterval();
    // Knowing when the current fault interval started its timing we can know how much of this flight it was active
    // and remove that from the tiem for the fault to maifest.
    thePlane->decrementNextFaultInterval(endTime - startOfCurrentFaultInterval);

    // Record the flight into the Simulation record of dompleted flights
    recordFlight();
    if(faultOption == 2 && faultCount > 0) {
        // faultOption == 2 means we ground flight with a fault afer the flight completes
        if(theSimulation && theSimulation->thePlaneQueue) {
            // We ground it by giving it an infinite delay
            // Pass false because we are calling PlaneQueue.addPlane() from Flight.handleEvent, not from PlaneQueue.handleEvent
            theSimulation->thePlaneQueue->addPlane(LONG_MAX, thePlane, false);
        }
        // if there is no simulation this plane will be done anyway
   } else {
        // Otherwise try to put it back on a charger
        if(theSimulation && theSimulation->theChargerQueue) {
            // "this" will be deleted so the plane will be owned by the battery queue
            // Pass false because we are calling PlaneQueue.addPlane() from Flight.handleEvent, not from PlaneQueue.handleEvent
            theSimulation->theChargerQueue->addPlane(currentTime, thePlane, false);
        }
    }
    // by returning false "this" will be removed from the eventHandler queue
    // and owned by the Charger or Plane queue. The only event a flight would
    // receive that does not result it in being removed from the SimClock
    // happens above with a a fault event with the faultOption != 1.
    return false; // do not keep us in the event queue
}
// For testing: for this child class it always returns 1
long Flight::countPlanes() {
    return 1;
}
// For testing: provide a description of this object
const std::string Flight::describe() {
    std::string description = "Flight for plane #" + thePlane->describe() + " endTime: " + std::to_string(endTime) + " nextFaultTime: " + std::to_string(nextFaultTime);
    return description;
}
// When the flight completes, record its information for simulation statistics
void Flight::recordFlight() {
    // We can only record the flight if there is a Simulation in which to record it.
    if(theSimulation) {
        // Calculate the duration
        long flightDuration = endTime-startTime;
        // Calculate passenger miles. Technically we could calculate this at the end when we sum up the data,
        // but we woudl still need to calculate it on a flight by flight basis so we do it here.
        double passengerMiles = flightDuration * passengerCount * thePlane->getMilesPerHour();
        passengerMiles /= secondsPerHourD;
        // Create a flightStats object
        FlightStats someStats{thePlane->getCompany(),thePlane->getPlaneNumber(), flightDuration,passengerCount,faultCount, passengerMiles};
        // Add it to the vector of saved flights
        theSimulation->theFlightStats.push_back(someStats);
    }
}

