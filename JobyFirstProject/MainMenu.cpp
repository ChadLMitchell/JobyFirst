//
//  MainMenu.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/23/25.
//

#include "MainMenu.hpp"
#include <iostream>
#include <iomanip>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "TestsMenu.hpp"
#include "SettingsMenu.hpp"
#include "MenuGroupWithAllOption.hpp"
#include "Plane.hpp"
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"
#include "SimSettings.hpp"

using namespace std;

extern SimSettings currentSettings;

void outputResults(std::vector<FinalStats> results)
{
    // Output the results
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
    
    // Statistics from this simulation
    cout << left << setw(16) << "Company"
    << left << setw(14) << "Total Flights "
    << left << setw(17) << "Avg. Time/Flight "
    << left << setw(18) << "Avg. Dist./Flight "
    << left << setw(14) << "Total Charges "
    << left << setw(17) << "Avg. Time/Charge "
    << left << setw(13) << "Total Faults "
    << left << setw(21) << "Total Passenger Miles"
    << endl;
    for(auto r: results) {
        cout << left << setw(16) << companyName(r.theCompany)
        << left << setw(14) << r.totalFlights
        << left << setw(17) << r.averageTimePerFlight
        << left << setw(18) << r.averageDistancePerFlight
        << left << setw(14) << r.totalCharges
        << left << setw(17) << r.averageTimeCharging
        << left << setw(13) << r.totalFaults
        << left << setw(21) << r.totalPassengerMiles
        << endl;
    }

}
bool runSimulation(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Simulation");
    SimSettings runSettings = currentSettings;
    if(selector == 2) {
        runSettings.simulationDuration = longTestClockSeconds; // 30 hours
    } else if(selector == 3) {
        runSettings.simulationDuration = longTestClockSeconds * 10; // 300 hours
    } else if(selector == 4) {
        runSettings.simulationDuration = longTestClockSeconds * 100; // 3000 hours
    } else if(selector == 5) {
        runSettings.simulationDuration = longTestClockSeconds * 1000; // 3000 hours
    }
    
    Simulation aSimulation(runSettings);
    std::vector<FinalStats> results = aSimulation.run(selector == 1 ? true : false);
    
    outputResults(results);

    return false;
}
bool runMultiple(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Multiple Simulations");
    SimSettings runSettings = currentSettings;

    const int runCount = 100;
    
    // Set up accumulators for the stats
    std::vector<FinalStats> accumulatedStats;
    for(auto c: allCompany) {
        FinalStats f{c};
        accumulatedStats.push_back(f);
    }

    auto startTimer = std::chrono::high_resolution_clock::now();
    for(int run = 0; run < runCount; run++) {
        // run each simulation
        Simulation aSimulation(runSettings);
        std::vector<FinalStats> results = aSimulation.run(false);

        // accumulate the results
        for(auto c: allCompany) {
            //      long totalFlights;
            //      double averageTimePerFlight;
            //      double averageDistancePerFlight;
            //      long totalCharges;
            //      double averageTimeCharging;
            //      long totalFaults;
            //      long totalPassengerMiles;
            accumulatedStats[c].totalFlights += results[c].totalFlights;
            accumulatedStats[c].averageTimePerFlight += results[c].averageTimePerFlight;
            accumulatedStats[c].averageDistancePerFlight += results[c].averageDistancePerFlight;
            accumulatedStats[c].totalCharges += results[c].totalCharges;
            accumulatedStats[c].averageTimeCharging += results[c].averageTimeCharging;
            accumulatedStats[c].totalFaults += results[c].totalFaults;
            accumulatedStats[c].totalPassengerMiles += results[c].totalPassengerMiles;
        }
    }
    auto stopTimer = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stopTimer - startTimer);
    std::cout << "Total time taken by all simulations: " << duration.count() << " microseconds" << std::endl;

    // turn totals into averages
    for(auto c: allCompany) {
        accumulatedStats[c].totalFlights /= runCount;
        accumulatedStats[c].averageTimePerFlight /= runCount;
        accumulatedStats[c].averageDistancePerFlight /= runCount;
        accumulatedStats[c].totalCharges /= runCount;
        accumulatedStats[c].averageTimeCharging /= runCount;
        accumulatedStats[c].totalFaults /= runCount;
        accumulatedStats[c].totalPassengerMiles /= runCount;
    }
    cout << "Average results for " << runCount << " simulation runs:" << endl;
    
    outputResults(accumulatedStats);

    return false;
}
bool doQuit(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Quit");
    return true;
}

vector<MenuItem> mainMenus {
    MenuItem('T', string{"Run Tests"}, &runTests, 0),
    MenuItem('S', string{"Edit Settings"}, &editSettings, 0),
    MenuItem('R', string{"Run Simulation (3 hours)"}, &runSimulation, 0),
    MenuItem('V', string{"Run Simulation Verbose (3 hours)"}, &runSimulation, 1),
    MenuItem('2', string{"Run 30-hour Simulation)"}, &runSimulation, 2),
    MenuItem('3', string{"Run 300-hour Simulation"}, &runSimulation, 3),
    MenuItem('4', string{"Run 3000-hour Simulation"}, &runSimulation, 4),
    MenuItem('5', string{"Run 30000-hour Simulation"}, &runSimulation, 5),
    MenuItem('6', string{"Average results from 100 3-hour Simulations"}, &runMultiple, 6),
    MenuItem('Q', string{"Quit"}, &doQuit, 0)
};
MenuGroup mainMenu = MenuGroup(mainMenus);
