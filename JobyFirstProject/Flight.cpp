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

Flight::Flight(Simulation *theSimulation, long startTime, long passengerCount, std::shared_ptr<Plane> aPlane):
EventHandler(LONG_MAX),theSimulation{theSimulation}, startTime{startTime}, endTime{startTime+aPlane->calcTimeOnFullCharge__seconds()}, nextFaultTime{startTime+aPlane->getNextFaultInterval()}, passengerCount{passengerCount},thePlane{aPlane} {
    nextEventTime = std::min(endTime, nextFaultTime);
    faultCount = 0;
}
Flight::~Flight() {
}
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
        if(faultOption != 1) { // record fault and continue
            nextEventTime = std::min(endTime, nextFaultTime);
            if(nextEventTime > currentTime) {
                return true; // add us back into the the event queue and continue the flight
            // but if we happen to be done with the flight at the same time as the fault, then process the flight completeion
            }
        } else {
            recordFlight();
            if(theSimulation && theSimulation->thePlaneQueue) {
                // "this" will be deleted so the plane will be owned by the plane queue
                // We ground it by giving it an infinite delay
                theSimulation->thePlaneQueue->addPlane(LONG_MAX, thePlane);
            }
            return false; // do not keep us in the event queue
        }
    }
    // finished the flight
    endTime = currentTime; // update flight end time in case we ended early
    // We used up the duration of the flight from the next fault interval for this plane.
    // We may have had a fault during this flight so our current interval may not have started at the beginning of the flight
    long startOfCurrentFaultInterval = nextFaultTime - thePlane->getNextFaultInterval();
    thePlane->decrementNextFaultInterval(endTime - startOfCurrentFaultInterval);

    recordFlight();
    if(faultOption == 1 && faultCount > 0) {
        // faultOption == 1 means we ground flight with faults afer the flight completes
        if(theSimulation && theSimulation->thePlaneQueue) {
            // We ground it by giving it an infinite delay
            theSimulation->thePlaneQueue->addPlane(LONG_MAX, thePlane);
        }
   } else {
        // Otherwise put it back on the charger
       if(theSimulation && theSimulation->theChargerQueue) {
            // "this" will be deleted so the plane will be owned by the battery queue
            theSimulation->theChargerQueue->addPlane(currentTime, thePlane);
        }
    }
    // by returning false "this" will be removed from the eventHandler queue
    // and owned by the Charger or Plane queue
    return false; // do not keep us in the event queue
}
long Flight::countPlanes() {
    return 1;
}
const std::string Flight::describe() {
    std::string description = "Flight for plane #" + thePlane->describe() + " endTime: " + std::to_string(endTime) + " nextFaultTime: " + std::to_string(nextFaultTime);
    return description;
}
void Flight::recordFlight() {
    long flightDuration = endTime-startTime;
    double passengerMiles = flightDuration * passengerCount * thePlane->getMilesPerHour();
    passengerMiles /= 3600.0;
    if(theSimulation) {
        theSimulation->theFlightStats.push_back(FlightStats(thePlane->getCompany(),thePlane->getPlaneNumber(), flightDuration,passengerCount,faultCount, passengerMiles));
    }
}

