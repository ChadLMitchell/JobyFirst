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
#include <future>
#include <stdexcept>
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
    cout << "Duration: " << s.simulationDuration << (secondsPerMinute == 60 ? " seconds" : " minutes") << endl;
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
void outputResultsHeader()
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
    << left << setw(13) << "Average "
    << left << setw(14) << "Average "
    << left << setw(9) << "Total  "
    << left << setw(13) << "Average "
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
}
void outputResults(std::vector<FinalStats> results)
{
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
// This is almost the same as FinalStats except that we use double for several fields that were long
// When we average those long values we want to be able to display double values
struct AverageStats {
    Company theCompany; // Enum id of the plane's company
    double totalFlights; // How many flights for this plane type?
    double averageTimePerFlight; // Average time per flight
    double averageDistancePerFlight; // Average distance per flight
    double totalCharges; // How many charges did planes of this type receive?
    double averageTimeCharging; // What was the average time to charge?
    double averageTimeChargingWithWait; // What was the average time to charge including waiting time?
    double totalFaults; // How many faults were experienced by planes of this kind
    double totalPassengerMiles; // Total passenger miles (computed for each flight)
                                // You could compute them from other fields, but only if
                                // You are sure that each plane flies completely full.
                                // We have options where that is not true.
};
void outputAverageResults(std::vector<AverageStats> results)
{
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

// Set up the progress indicator if it seems like this may be a longg run
// This is done once per execution of the program to ensure that the monitor
// or terminal program supports '\r' to allow overwriting lines on the screen
// (some do not and on those monitors this would might annouyingly output
// what might be hundreds of rows of progress indication.
void setProgressIndicator(MenuGroup &thisMenuGroup, SimSettings &runSettings) {
    // Note that this decides based on runSettings, but it updates both runSettings and currentSettings
    // becasue we only want to set this once per execution of the program.

    // If we already set it this run of the program, don't do it again
    if(currentSettings.progressInterval != -1) { return; }
    
    // Don't bother to ask if it probably is too fast to need a progress indicator
    // We need these passed in from the actual settings used to do the simulation
    if(runSettings.simulationDuration <= secondsPerHour * runSettings.progressMoreThanHours &&
       runSettings.planeCount <= runSettings.progressMoreThanPlanes ) {
        return;
    }
    
    // Confirm that '\r' works on their screen
    cout << endl;
    cout << "Line #1" << flush;
    cout << "\rLine #2" << endl;
    cout << "How many numbered lines do you see ('Line #1' and 'Line #2' or just 'Line #2')?" << endl;
    long userResponse = thisMenuGroup.getNumberFromUser("Please enter '1' or '2' (Entering 1 turns on a progress indicator): ");
    // Set both runSettings (for the current simulation) and currentSettings (so we don't ask again)
    if(userResponse == 1) {
        // Show progress every 100 hours for any simulation with duration more than progressMoreThanHours  or
        // planeCountt more than progressMoreThanPlanes.
        runSettings.progressInterval = runSettings.progressIntervalValue;
        currentSettings.progressInterval = currentSettings.progressIntervalValue;
    } else {
        // Do not show progress
        runSettings.progressInterval = 0;
        currentSettings.progressInterval = 0;
    }
    cout << endl;
}

bool continueWithLongSimulation = false;
// Implement a menu that confirms the user wants to continue with long simulation
bool selectContinueLong(int selector, MenuGroup &thisMenuGroup) {
    continueWithLongSimulation = (selector == 1);
    return true;
}
vector<MenuItem> continueLongMenus {
    MenuItem('Y', string{"Yes, continue with long simulation"}, &selectContinueLong, 1),
    MenuItem('N', string{"No, I will choose another option"}, &selectContinueLong, 0),
};
MenuGroup continueLongMenu = MenuGroup(continueLongMenus);

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
    } else if(selector == 6) {
        // Make sure the user is read for a long run
        cout << "This stress test could take 10 to 45 minutes depending on the computer" << endl;
        cout << "Do you want to continue?" << endl;
        continueLongMenu.runMenu();
        if(!continueWithLongSimulation) { return false; }
        cout << "This may take a while..." << endl;
        
        runSettings.simulationDuration = secondsPerHour * 35040; // 35040 hours (4 years)
        runSettings.planeCount = 1000; // 1000 planes
        runSettings.chargerCount = 150; // 150 chargers
        runSettings.maxPassengerDelay = secondsPerHour;
        runSettings.minPlanePerKind = 100; // 100 minimum of each kind
            // This means the first 500 will be allocated randomly, but constrained to have
            // 100 plane each. The last 500 planes will be completely randomly allocated.
    }

    // Set the progress indicator option (if needed)
    // We pass in runSettings so it decides based on the simulation that will run
    // but this function will also set currentSettings since we set it per execution of the program
    setProgressIndicator(thisMenuGroup, runSettings);

    // Create and run the simulation
    Simulation aSimulation(runSettings);
    // If the selectorValue == 1 that was used to run a verbose simulation (useful for testing)
    std::vector<FinalStats> results = aSimulation.run(selector == 1 ? true : false);
    
    outputSettings(runSettings);
    cout << "Results for this simulation run:" << endl;
    outputResultsHeader();
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

 
    // Set the progress indicator option (if needed)
    // We pass in runSettings so it decides based on the simulation that will run
    // but this function will also set currentSettings since we set it per execution of the program
    setProgressIndicator(thisMenuGroup, runSettings);

    const int runCount = 100;
    
   // Set up accumulators for the stats
    std::vector<AverageStats> accumulatedStats;
    for(auto c: allCompany) {
        AverageStats f{c};
        accumulatedStats.push_back(f);
    }

    // The Simulator function times individual simulations, but this will time the series
    auto startTimer = std::chrono::high_resolution_clock::now();

    std::array<std::future<std::vector<FinalStats>>, runCount> asyncFunctions;
    std::array<std::shared_ptr<Simulation>, runCount> simulations;
    
    // Launch the simulations with the current parameters asynchronously
    for(int run = 0; run < runCount; run++) {
        // run each simulation
        simulations[run] = std::make_shared<Simulation>(runSettings);
        asyncFunctions[run] = simulations[run]->runAsync(false);
    }
    for(int run = 0; run < runCount; run++) {
        if(!asyncFunctions[run].valid()) {
            throw std::runtime_error("The future for simulation #" + std::to_string(run) + " is not valid.");
        }
        std::vector<FinalStats> results = asyncFunctions[run].get();
        // accumulate the results
        // TO-DO: Faily confident that none of these overflow the capacity of long and double as we
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
    // TO-DO: It might be interesting to also output some of the grand totals
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
    outputResultsHeader();
    outputAverageResults(accumulatedStats);

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
    MenuItem('5', string{"Run 4-year, 1000 Plane, 150 Chargers, up to 1 hour wait for passengers"}, &runSimulation, 6),
    MenuItem('-', string{""}, nullptr, 0),
    MenuItem('Q', string{"Quit"}, &doQuit, 0)
};

// This is the external entry point to the menu system accessed by main()
MenuGroup mainMenu = MenuGroup(mainMenus);
