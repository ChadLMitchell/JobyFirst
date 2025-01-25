//
//  SimSettings.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef SimSettings_hpp
#define SimSettings_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>

// These are used to help us convert from hours and minutes to seconds
const long secondsPerHour{60 * 60}; // Converting seconds to hours and hours to seconds
const double secondsPerHourD{60.0 * 60.0}; // For floating point calcuations
const long defaultTestClockSeconds{secondsPerHour * 3}; //3 hours
const long longTestClockSeconds{secondsPerHour * 30}; //30 hours
const long shortTestClockSeconds{60}; //1 minute

// These are settings for the simulator. We can pass a copy of these into the Simulator
// constructor to run with these. To use the defaults, create a SimSettings object with
// the default values.
struct SimSettings {
    
    int passengerCountOption = 0;
    // 0 = plane always flies full
    // 1 = randomly distribute number of passengers between 1 and max for each flight

    int faultOption = 0;
    // 0 = just count faults
    // 1 = fault grounds plane immediately for duration of simulation
    // 2 = fault grounds plane at the end of current flight

    long maxPassengerDelay = 0;
    // 0 = enough passengers are always available
    // > 0 = randomly distribute delay between 0 and max for each flight

    long simulationDuration = defaultTestClockSeconds; // default 3 hours
    
    long chargerCount = 3;
    
    long planeCount = 20;
    
    long minPlanePerKind = 1;
};

// enumeration of the possible company/kinds of planes
enum Company {
    Alpha = 0,
    Bravo,
    Charlie,
    Delta,
    Echo
};
// Allow iterating through the Company enumeration
const Company allCompany[] = {Alpha, Bravo, Charlie, Delta, Echo};
// Minimum and maximum elements of the Company enumeration
const Company minCompany{Alpha};
const Company maxCompany{Echo};
const long companyCount = maxCompany + 1;

// Reference to an array of company names
extern const char *companyNames[];
// Global function that takes a company enumeration and returns a company name
const char *companyName(Company c);

#endif /* SimSettings_hpp */
