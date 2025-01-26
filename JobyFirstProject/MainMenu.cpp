//
//  MainMenu.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/23/25.
//

#include "MainMenu.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>
#include <iomanip>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "TestsMenu.hpp"
#include "SettingsMenu.hpp"
#include "MenuGroupWithAllOption.hpp"
#include "Simulation.hpp"
#include "SimSettings.hpp"

using namespace std;

// Refer to the current settings to be used in creating simulations
extern SimSettings currentSettings;

// This function displays the curernt settings to cout
// Where a settign has several options, it displays a string describing the selected option
void outputSettings(const SimSettings &s)
{
    cout << "Simulation Settings:" << endl;
    cout << "Duration: " << s.simulationDuration << " seconds" << endl;
    cout << "Using " << s.chargerCount << " chargers and "  << s.planeCount << " planes" << endl;
    cout << "Minimum planes per kind: " << s.minPlanePerKind << endl;
    cout << "Passenger Count Option: "
    << (s.passengerCountOption == 0 ? "Planes always fly full" : "Passenger count random up to max") << endl;
    cout << "Passenger Option: "
    << (s.faultOption == 0 ? "Faults counted, but do not affect flights" : (s.faultOption == 01 ? "A fault grounds plane immediately" : "A fault grounds plane at end of current flight")) << endl;
    string delayString = "No delay for passengers";
    if(s.passengerCountOption > 0) {
        delayString = "When ready to fly, planes experience a random [0 - " + to_string(s.passengerCountOption) + "] second delay for passengers";
    }
    cout << "Passenger Delay Option: " << delayString << endl;
    cout << endl;
}

// This function displays the results of a simulation run.
// The results vector could be that returned directly from Simulation.run(),
// but it could also be summary of a series of simulations.
void outputResults(std::vector<FinalStats> results)
{
// For reference, the structure is copied here during development and to help understand the code
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
    
    // Display a header with two row column names
    cout << left << setw(17) << "Company"
    << left << setw(9) << "Total "
    << left << setw(13) << "Avgerage "
    << left << setw(14) << "Avgerage "
    << left << setw(9) << "Total  "
    << left << setw(13) << "Avgerage "
    << left << setw(17) << "Average Charge "
    << left << setw(8) << "Total "
    << left << setw(15) << "Total"
    << endl;
    cout << left << setw(17) << ""
    << left << setw(9) << "Flights "
    << left << setw(13) << "Time/Flight "
    << left << setw(14) << "Miles/Flight "
    << left << setw(9) << "Charges "
    << left << setw(13) << "Time/Charge "
    << left << setw(17) << "Time incl. Wait "
    << left << setw(8) << "Faults "
    << left << setw(15) << "Passenger Miles"
    << endl;

    // Display the results for each company
    for(auto r: results) {
        cout << setprecision(2) << fixed
        << left << setw(17) << companyName(r.theCompany)
        << left << setw(9) << r.totalFlights
        << left << setw(13) << r.averageTimePerFlight
        << left << setw(14) << r.averageDistancePerFlight
        << left << setw(9) << r.totalCharges
        << left << setw(13) << r.averageTimeCharging
        << left << setw(17) << r.averageTimeChargingWithWait
        << left << setw(8) << r.totalFaults
        << left << setw(15) << r.totalPassengerMiles
        << endl;
    }

}

// Run a simulation. This function uses the memuItem selector to run different simulations
// with the same function.
bool runSimulation(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Simulation");
    // Make a copy of the current settings so any changes we make here are
    // temporary and do not affect future simulations
    SimSettings runSettings = currentSettings;
    
    // If they selected one of the Stress Test options then we adjust the settings to match
    if(selector == 2) {
        runSettings.simulationDuration = secondsPerHour * 30; // 30 hours
    } else if(selector == 3) {
        runSettings.simulationDuration = secondsPerHour * 300; // 300 hours (12.5 days)
    } else if(selector == 4) {
        runSettings.simulationDuration = secondsPerHour * 3000; // 3000 hours (125 days)
    } else if(selector == 5) {
        runSettings.simulationDuration = secondsPerHour * 35040; // 35040 hours (4 years)
// Tested 300000 hours on a Mac, but since "long" is 4 bytes on Windows, that value would not work on
// Windows unless we carefull change everywhere a time is used in all the code to be "long long"
// which did not seem worth the couple hours effort and testing at this point.
//    } else if(selector == 6) {
//        runSettings.simulationDuration = longTestClockSeconds * 300000; // 300,000 hours
    }

    // Create and run the simulation
    Simulation aSimulation(runSettings);
    // If the selectorValue == 1 that was used to run a verbose simulation (useful for testing)
    std::vector<FinalStats> results = aSimulation.run(selector == 1 ? true : false);
    
    outputSettings(runSettings);
    cout << "Results for this simulation run:" << endl;
    outputResults(results);

    return false;
}

// Run multiple simulations (specified in runCount) and then average the results.
bool runMultiple(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Multiple Simulations");

    // Make a copy of the current settings so any changes we make here are
    // temporary and do not affect future simulations. This function does not
    // change the settings, but still good practice in case we change some later.
    SimSettings runSettings = currentSettings;

    const int runCount = 100;
    
    // Set up accumulators for the stats
    std::vector<FinalStats> accumulatedStats;
    for(auto c: allCompany) {
        FinalStats f{c};
        accumulatedStats.push_back(f);
    }

    // The Simulator function times individual simulations, but this will time the series
    auto startTimer = std::chrono::high_resolution_clock::now();
    
    // Repeat the simulation with the current parameters
    for(int run = 0; run < runCount; run++) {
        // run each simulation
        Simulation aSimulation(runSettings);
        std::vector<FinalStats> results = aSimulation.run(false);

        // accumulate the results
        // TO DO: Faily confident that none of these overflow the capacity of long and double as we
        // go, but with a higher runCount that could be a problem. Need to add checking to make
        // sure as we accumulate things that we do not lose data.
        for(auto c: allCompany) {
            //      long totalFlights;
            //      double averageTimePerFlight;
            //      double averageDistancePerFlight;
            //      long totalCharges;
            //      double averageTimeCharging;
            //      double averageTimeChargingWithWait;
            //      long totalFaults;
            //      long totalPassengerMiles;
            accumulatedStats[c].totalFlights += results[c].totalFlights;
            accumulatedStats[c].averageTimePerFlight += results[c].averageTimePerFlight;
            accumulatedStats[c].averageDistancePerFlight += results[c].averageDistancePerFlight;
            accumulatedStats[c].totalCharges += results[c].totalCharges;
            accumulatedStats[c].averageTimeCharging += results[c].averageTimeCharging;
            accumulatedStats[c].averageTimeChargingWithWait += results[c].averageTimeChargingWithWait;
            accumulatedStats[c].totalFaults += results[c].totalFaults;
            accumulatedStats[c].totalPassengerMiles += results[c].totalPassengerMiles;
        }
    }
    
    // Output the cumulative run time for the series of simulations
    auto stopTimer = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTimer - startTimer);
    double secondsTaken = duration.count() / 1000000.0;
    std::cout << "Total time taken by all simulations: "
    << duration.count() << " microseconds ("
    << secondsTaken << " seconds)" << std::endl;

    // turn totals into averages
    // TO DO: It might be interesting to also output some of the grand totals
    // but some of them would not be meaningful so we can't just call outputResults
    // here. Maybe we could just to see how close we come to overflowing long or double
    for(auto c: allCompany) {
        accumulatedStats[c].totalFlights /= runCount;
        accumulatedStats[c].averageTimePerFlight /= runCount;
        accumulatedStats[c].averageDistancePerFlight /= runCount;
        accumulatedStats[c].totalCharges /= runCount;
        accumulatedStats[c].averageTimeCharging /= runCount;
        accumulatedStats[c].averageTimeChargingWithWait /= runCount;
        accumulatedStats[c].totalFaults /= runCount;
        accumulatedStats[c].totalPassengerMiles /= runCount;
    }
    outputSettings(runSettings);
    cout << "Average results for " << runCount << " simulation runs:" << endl;
    outputResults(accumulatedStats);

    return false;
}

// This handles quitting the simulation by returning false when this is in MenuFuncPtr.
bool doQuit(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Quit");
    return true;
}

// The menu dwscriptions
vector<MenuItem> mainMenus {
    MenuItem('E', string{"Edit Settings"}, &editSettings, 0),
    MenuItem('R', string{"Run Simulation with Current Settings"}, &runSimulation, 0),
    MenuItem('A', string{"Average results from 100 Simulations"}, &runMultiple, 0),
    MenuItem('V', string{"Run Simulation Verbose with Current Settings"}, &runSimulation, 1),
    MenuItem('T', string{"Run Tests"}, &runTests, 0),
    MenuItem('-', string{""}, nullptr, 0),
    MenuItem(' ', string{"Stress Test Options:"}, nullptr, 0),
    MenuItem('1', string{"Run 30-hour Simulation)"}, &runSimulation, 2),
    MenuItem('2', string{"Run 300-hour (12.5 Day) Simulation"}, &runSimulation, 3),
    MenuItem('3', string{"Run 3,000-hour (125 Day) Simulation"}, &runSimulation, 4),
    MenuItem('4', string{"Run 35,040-hour (4 year) Simulation"}, &runSimulation, 5),
// The following option worked on Mac, but it would require all time variables in the entire
// project to carefully be changed to "long long" on Windows
//    MenuItem('5', string{"Run 300,000-hour (34.2 Year) Simulation"}, &runSimulation, 6),
    MenuItem('-', string{""}, nullptr, 0),
    MenuItem('Q', string{"Quit"}, &doQuit, 0)
};

// This is the external entry point to the menu system accessed by main()
MenuGroup mainMenu = MenuGroup(mainMenus);
