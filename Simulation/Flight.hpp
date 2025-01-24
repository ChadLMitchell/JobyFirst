//
//  Flight.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef Flight_hpp
#define Flight_hpp

#include <stdio.h>
#include "SimClock.hpp"
#include "Plane.hpp"

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
class Flight: public EventHandler {
    long startTime;
    long endTime;
    long nextFaultTime;
    long passengerCount;
    long faultCount;
    std::shared_ptr<Plane> thePlane;
    Simulation *theSimulation;
public:
    Flight(Simulation *theSimulation, long startTime, long passengerCount, std::shared_ptr<Plane> aPlane);
    virtual ~Flight() override;
    virtual bool handleEvent(long currentTime, bool closeOut) override;
    virtual long countPlanes() override;
    virtual const std::string describe() override;
    void recordFlight();
};

#endif /* Flight_hpp */
