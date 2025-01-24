//
//  Simulation.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#ifndef Simulation_hpp
#define Simulation_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include "Plane.hpp"
#include "SimSettings.hpp"


/*
 *******************************************************************************************
 * Struct FlightStats
 * These are collected internal to the simulation at the end of each flight.
 * Then are stored in the theFlightStats vector and are used at the end of the simulation
 * to generate the statistics returned from the simulation run.
 * *******************************************************************************************
 */
struct FlightStats {
    Company theCompany; // Enum id of the plane's company
    int planeNumber; // Plane number to help with debugging
    long duration; // How long was the flight. If time runs out, the partial time is logged.
    long passengerCount; // How many passengers
    long faultCount; // How many faults during the flight
    double passengerMiles; // Calculate the passenger miles on a per flight basis since
                        // they cannot be calculated from an aggregate number of passengers and miles
};
/*
 *******************************************************************************************
 * Struct ChargerStats
 * These are collected internal to the simulation at the end of each charge of a plane.
 * Then are stored in the theChargerStats vector and are used at the end of the simulation
 * to generate the statistics returned from the simulation run.
 * *******************************************************************************************
 */
struct ChargerStats {
    Company theCompany; // Enum id of the plane's company
    int planeNumber; // Plane number to help with debugging
    long duration; // How long on the charger. If time runs out, partial time is logged.
    long durationWithWait; // How long on the charger plus wait time. If time runs out, partial time is logged.
};
// These are for the final statistics returned by each simulator run.
// There will be one row for each company/type of plane.
/*
 *******************************************************************************************
 * Struct FinalStats
 * These are for the final statistics returned by each simulator run.
 * run() will return a vector of this struct with one row for each company/type of plane.
 * *******************************************************************************************
 */
struct FinalStats {
    Company theCompany; // Enum id of the plane's company
    long totalFlights; // How many flights for this plane type?
    double averageTimePerFlight; // Average time per flight
    double averageDistancePerFlight; // Average distance per flight
    long totalCharges; // How many charges did planes of this type receive?
    double averageTimeCharging; // What was the average time to charge?
    double averageTimeChargingWithWait; // What was the average time to charge including waiting time?
    long totalFaults; // How many faults were experienced by planes of this kind
    double totalPassengerMiles; // Total passenger miles (computed for each flight)
                                // You could compute them from other fields, but only if
                                // You are sure that each plane flies completely full.
                                // We have options where that is not true.
};

/*
 *******************************************************************************************
 * Class Simulation
 * To run a simulation, pass a SimSettings object to the constructor for a Simulation.
 * Then call run() on the Simulation. After it runs, you can display or process the results
 * that are returned in a vector of FinalStats.
 *
 * If you call run(true) then it will use "cout" to share some details about the progress
 * of the Simulation including a list of all the Flights and Charges during the simulation.
 * *******************************************************************************************
 */
class SimClock; // Forward reference since they reference each other
class ChargerQueue; // Forward reference since they reference each other
class PlaneQueue; // Forward reference since they reference each other
class Simulation {
   
protected:
    // When constructed, a Simulation constructs one ovject of each of the following three
    // Class objects. They are each passed a pointer to this Simulation so they have access
    // to each other and to the other protected class members below. So those classes need
    // to be friends. It is safe to pass a simple pointer to this class object because they
    // are created by it and the only persistent pointers to them are here so if this class
    // object is deleted they will also be deleted. If members of these classes are created
    // outside a simulation (for testing), they will each individually behave correctly
    // id pased a nullptr passed for the Simulation when they are constructed.
    // Forward references since these classes reference each other
    friend class SimClock;
    friend class ChargerQueue;
    friend class PlaneQueue;
    std::shared_ptr<SimClock> theSimClock;
    std::shared_ptr<ChargerQueue> theChargerQueue;
    std::shared_ptr<PlaneQueue> thePlaneQueue;
    // When some of those three classes create any Flight objects, they forward the pointer
    // to this Simulation so it can also access the procted members of this class.
    friend class Flight;

    // Shared settings for this instance of the Simulation
    std::shared_ptr<SimSettings> theSettings;

    // These two vectors are used by the friend classes to accumulate statistics about
    // the run of this Simulation. The Simulation constructor initializes them as empty.
    std::vector<FlightStats> theFlightStats;
    std::vector<ChargerStats> theChargerStats;

public:
    Simulation(SimSettings someSettings);
    ~Simulation();
    
    // This function runs the simulation and returns the results
    std::vector<FinalStats> run(bool verbose);
};

#endif /* Simulation_hpp */
