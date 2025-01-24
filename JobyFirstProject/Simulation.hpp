//
//  Simulation.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#ifndef Simulation_hpp
#define Simulation_hpp

#include <stdio.h>
#include <iostream>
#include "Plane.hpp"
#include "SimSettings.hpp"

class SimClock;
class ChargerQueue;
class PlaneQueue;

struct FlightStats {
    Company theCompany;
    int planeNumber;
    long duration;
    long passengerCount;
    long faultCount;
    double passengerMiles;
};
struct ChargerStats {
    Company theCompany;
    int planeNumber;
    long duration;
};
struct FinalStats {
    Company theCompany;
    long totalFlights;
    double averageTimePerFlight;
    double averageDistancePerFlight;
    long totalCharges;
    double averageTimeCharging;
    long totalFaults;
    double totalPassengerMiles;
};

class Simulation {
   
public:
    std::shared_ptr<SimSettings> theSettings;
    std::shared_ptr<SimClock> theSimClock;
    std::shared_ptr<ChargerQueue> theChargerQueue;
    std::shared_ptr<PlaneQueue> thePlaneQueue;
    std::vector<FlightStats> theFlightStats;
    std::vector<ChargerStats> theChargerStats;

    Simulation(SimSettings someSettings);
    ~Simulation();
    std::vector<FinalStats> run(bool verbose);
    

    
    
};

#endif /* Simulation_hpp */
