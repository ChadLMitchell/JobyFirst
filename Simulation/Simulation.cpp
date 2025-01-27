//
//  Simulation.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#include "Simulation.hpp"
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"
#include "SimSettings.hpp"
#include <iomanip>
#include <chrono>

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
Simulation::Simulation(SimSettings someSettings):
theSimClock{}, theChargerQueue{}, theFlightStats{}, theChargerStats{} {
    // Set up shared pointer to the settings for this simulation
    theSettings = std::make_shared<SimSettings>(someSettings);
}
Simulation::~Simulation() {
    
}


std::vector<FinalStats> Simulation::run(bool verbose)
{
    // Start a timer so we can report how long it takes to run
    auto startTimer = std::chrono::high_resolution_clock::now();
    theSimClock = std::make_shared<SimClock>(this, theSettings->simulationDuration);
 
    // Set up the environment
    theChargerQueue = std::make_shared<ChargerQueue>(this, theSettings->chargerCount);
    thePlaneQueue = std::make_shared<PlaneQueue>(this);
    thePlaneQueue->generatePlanes(theSimClock->getTime(), theSettings->planeCount, theSettings->minPlanePerKind, theSettings->maxPassengerDelay);
    theSimClock->addHandler(theChargerQueue);
    theSimClock->addHandler(thePlaneQueue);
    
    // Run the actual simulation
    theSimClock->run(verbose);
    
    // Display the run time
    auto stopTimer = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTimer - startTimer);
    double secondsTaken = duration.count() / 1000000.0;
    std::cout << "Time taken by simulation: "
    << duration.count() << " microseconds ("
    << secondsTaken << " seconds)" << std::endl;

    // Prepare to return the results
    
    // First summarize the flight stat data and charger stat data
    long flightCounts[maxCompany+1]{};
    FlightStats totalFlightStats[maxCompany+1]{};
    long chargeCounts[maxCompany+1]{};
    ChargerStats totalChargerStats[maxCompany+1]{};
    // initialize the totals
    for(auto c: allCompany) {
        totalFlightStats[c].theCompany = c;
        totalChargerStats[c].theCompany = c;
    }
    if(verbose) {
        std::cout << "***** List of flights *****" << std::endl;
    }
    // Total the statistics from each flight
    for(FlightStats f: theFlightStats) {
        using namespace std;
        Company c{f.theCompany};
        flightCounts[c]++;
        totalFlightStats[c].duration += f.duration;
        totalFlightStats[c].passengerCount += f.passengerCount;
        totalFlightStats[c].faultCount += f.faultCount;
        totalFlightStats[c].passengerMiles += f.passengerMiles;
        // In verbose mode we list each flight
        if(verbose) {
            cout << "Duration: " << setw(10) << f.duration
            << " Passengers: " << f.passengerCount
            << " Passenger Miles: " << setw(10) << f.passengerMiles
            << " Faults: " << setw(3) << f.faultCount
            << " Plane #" << left << setw(3) << f.planeNumber
            << " " << companyName(f.theCompany) << endl;
        }
    }
    
    if(verbose) {
        std::cout << std::endl;
        std::cout << "***** List of charges *****" << std::endl;
    }
    // Total the statistics from each charge
    for(ChargerStats cs: theChargerStats) {
        using namespace std;
        Company c{cs.theCompany};
        chargeCounts[c]++;
        totalChargerStats[c].duration += cs.duration;
        totalChargerStats[c].durationWithWait += cs.durationWithWait;
        // In verbose mode we list each charge
        if(verbose) {
            cout << "Charge Duration: " << setw(10) << cs.duration
            << "Duration+Wait: " << setw(10) << cs.durationWithWait
            << " Plane #" << left << setw(3) << cs.planeNumber
            << " " << companyName(cs.theCompany) << endl;
        }
    }
    if(verbose) {
        std::cout << std::endl;
    }


    // This struct is copied here from our .hpp for easy reference. Be sure to keep them in sync.
    // struct FinalStats {
    //      Company theCompany;
    //      long totalFlights;
    //      double averageTimePerFlight;
    //      double averageDistancePerFlight;
    //      long totalCharges;
    //      double averageTimeCharging;
    //      double averageTimeChargingWithWait;
    //      long totalFaults;
    //      long totalPassengerMiles;
    // };
    // Now summarize the results
    // We transfer from a total of the individual flights and charges to results
    // Some of them want grand totals and some of them want averages
    std::vector<FinalStats> returnValue{};
    extern PlaneSpecification planeSpecifications[];
    long totalFlights{0};
    long totalCharges{0};
    for(auto c: allCompany) {
        totalFlights += flightCounts[c];
        totalCharges += chargeCounts[c];
        double flightCountD = flightCounts[c]; // so we do floating point math
        if(flightCountD <= 0) flightCountD = 1.0; // so we avoid division by 0
        double chargeCountD = chargeCounts[c]; // so we do floating point math
        if(chargeCountD <= 0) chargeCountD = 1.0; // so we avoid division by 0
        double averageTime = totalFlightStats[c].duration/flightCountD;
        FinalStats stats{c,  flightCounts[c], averageTime,
            averageTime * planeSpecifications[c].cruise_speed__mph / secondsPerHourD,
            chargeCounts[c],
            totalChargerStats[c].duration / chargeCountD,
            totalChargerStats[c].durationWithWait / chargeCountD,
            totalFlightStats[c].faultCount,
            totalFlightStats[c].passengerMiles};
        returnValue.push_back(stats);
    }
    
    // Output a short summary of the overall simulation.
    // We let the caller output the more detailed statistics.
    std::cout << "Final simulated time: " << theSimClock->getTime() << " seconds (" <<
    theSimClock->getTime()/(secondsPerHourD) << " hours)" << std::endl;
    std::cout << totalFlights << " flights and " << totalCharges << " charges" << std::endl;
    std::cout << std::endl;
    
    return returnValue;
}

// How often do we show progress indicator (<= 0 means not at all)
// This decides it based on settings
long Simulation::getProgressInterval() {
    if(theSettings && theSettings->progressInterval > 1 &&
       (theSettings->simulationDuration > theSettings->progressMoreThanHours ||
        theSettings->planeCount > theSettings->progressMoreThanPlanes)) {
        return theSettings->progressInterval * secondsPerHour;
    }
    return 0;
}
