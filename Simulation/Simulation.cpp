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

Simulation::Simulation(SimSettings someSettings):
theSimClock{}, theChargerQueue{}, theFlightStats{}, theChargerStats{} {
     theSettings = std::make_shared<SimSettings>(someSettings);
}
Simulation::~Simulation() {
    
}
std::vector<FinalStats> Simulation::run(bool verbose)
{
    auto startTimer = std::chrono::high_resolution_clock::now();
    theSimClock = std::make_shared<SimClock>(this, theSettings->simulationDuration);
    theChargerQueue = std::make_shared<ChargerQueue>(this, theSettings->chargerCount);
    thePlaneQueue = std::make_shared<PlaneQueue>(this);
    thePlaneQueue->generatePlanes(theSimClock->getTime(), theSettings->planeCount, theSettings->minPlanePerKind);
    theSimClock->addHandler(theChargerQueue);
    theSimClock->addHandler(thePlaneQueue);
    theSimClock->run(verbose);
    auto stopTimer = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTimer - startTimer);
    double secondsTaken = duration.count() / 1000000.0;
    std::cout << "Time taken by simulation: "
    << duration.count() << " microseconds ("
    << secondsTaken << " seconds)" << std::endl;

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
    for(FlightStats f: theFlightStats) {
        using namespace std;
        Company c{f.theCompany};
        flightCounts[c]++;
        totalFlightStats[c].duration += f.duration;
        totalFlightStats[c].passengerCount += f.passengerCount;
        totalFlightStats[c].faultCount += f.faultCount;
        totalFlightStats[c].passengerMiles += f.passengerMiles;
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
    for(ChargerStats cs: theChargerStats) {
        using namespace std;
        Company c{cs.theCompany};
        chargeCounts[c]++;
        totalChargerStats[c].duration += cs.duration;
        if(verbose) {
            cout << "Charge Duration: " << setw(10) << cs.duration
            << " Plane #" << left << setw(3) << cs.planeNumber
            << " " << companyName(cs.theCompany) << endl;
        }
    }
    if(verbose) {
        std::cout << std::endl;
    }


    // Now summarize the results
    // struct FinalStats {
    //      Company theCompany;
    //      long totalFlights;
    //      double averageTimePerFlight;
    //      double averageDistancePerFlight;
    //      long totalCharges;
    //      double averageTimeCharging;
    //      long totalFaults;
    //      long totalPassengerMiles;
    // };
    std::vector<FinalStats> returnValue{};
    const long seconds_per_hour = 60*60;
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
            averageTime * planeSpecifications[c].cruise_speed__mph / seconds_per_hour,
            chargeCounts[c], totalChargerStats[c].duration / chargeCountD,
            totalFlightStats[c].faultCount,
            totalFlightStats[c].passengerMiles};
        returnValue.push_back(stats);
    }
    
    std::cout << "Final simulated time: " << theSimClock->getTime() << " seconds (" <<
    theSimClock->getTime()/(60.0*60.0) << " hours)" << std::endl;
    std::cout << totalFlights << " flights and " << totalCharges << " charges" << std::endl;
    std::cout << std::endl;
    
    return returnValue;
}
