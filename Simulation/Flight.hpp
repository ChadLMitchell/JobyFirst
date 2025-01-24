//
//  Flight.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef Flight_hpp
#define Flight_hpp

#include <stdio.h>
#include <vector>
#include <string>
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
    long startTime; // What time did this flight start?
    long endTime; // What time should this flight complete (if not interrupted)?
    long nextFaultTime; // When is the next fault going to happen?
    long passengerCount; // How many passengers on this flight?
    long faultCount; // How many faults have happened so far on this flight?
    std::shared_ptr<Plane> thePlane; // The plane assigned to this flight
    Simulation *theSimulation; // Simulation object containing current simulation or nullptr
public:
    Flight(Simulation *theSimulation, long startTime, long passengerCount, std::shared_ptr<Plane> aPlane);
    virtual ~Flight() override;

    // Handle events when the flight is done, the simulation is done or a fault occurs
    virtual bool handleEvent(long currentTime, bool closeOut) override;

    // For testing: for this child class it always returns 1
    virtual long countPlanes() override;

    // For testing: provide a description of this object
    virtual const std::string describe() override;
    
    // When the flight completes, record its information for simulation statistics
    void recordFlight();
};

#endif /* Flight_hpp */
