//
//  Flight.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#include "SimSettings.hpp"
#include "Flight.hpp"
#include "ChargerQueue.hpp"

Flight::Flight(Simulation *theSimulation, long startTime, long passengerCount, std::shared_ptr<Plane> aPlane):
theSimulation{theSimulation}, startTime{startTime}, endTime{startTime+aPlane->calcTimeOnFullCharge__seconds()}, nextFaultTime{startTime+aPlane->getNextFaultInterval()}, passengerCount{passengerCount},thePlane{aPlane},EventHandler(std::min(endTime, nextFaultTime)) {
    faultCount = 0;
}
Flight::~Flight() {
}
bool Flight::handleEvent(long currentTime) {
    if(currentTime == nextFaultTime) {
        faultCount++;
        nextFaultTime = thePlane->createFaultInterval(); // reset the next fault interval
        int faultOption = 0;
        if(theSimulation && theSimulation->theSettings) {
            faultOption = theSimulation->theSettings->faultOption;
        }
        if(faultOption == 0) { // record fault and continue
            nextEventTime = std::min(endTime, nextFaultTime);
            if(nextFaultTime > currentTime) {
                return true; // add us back into the the event queue and continue the flight
                // but if we happen to be done with the flight at the same time as the fault, then process the flight completeion
            }
        } else {
            // Add Plane to Plane Queue **************************************************** TO DO
            recordFlight();
            return false; // do not keep us in the event queue
        }
    }
    // finished the flight
    endTime = currentTime; // in case we ended early
    recordFlight();
    if(theSimulation && theSimulation->theChargerQueue) {
        theSimulation->theChargerQueue->addPlane(currentTime, thePlane); // "this" will be deleted so the plane now owned by the battery queue
    }
    return false; // do not keep us in the event queue
}
long Flight::countPlanes() {
    return 1;
}
void Flight::recordFlight() {
    completedFlights.push_back(FlightStats(thePlane->getCompany(),endTime-startTime,passengerCount,faultCount));
}

std::vector<FlightStats> completedFlights{};

