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
#include <string>


// These are settings for the simulator. We can pass a copy of these into the Simulator
// constructor to run with these. To use the defaults, create a SimSettings object with
// the default values.
struct SimSettings {
    
    int passengerCountOption = 0;
    // 0 = plane always flies full
    // 1 = randomly distribute number of passengers between 1 and max for each flight

    int maxPassengerDelay = 0;
    // 0 = enough passengers are always available
    // > 0 = randomly distribute delay between 1 and max for each flight

    int faultOption = 0;
    // 0 = just count faults
    // 1 = fault grounds plane immediately for duration of simulation
    // 2 = fault grounds plane at the end of current flight

    long simulationDuration = 60*60*3; // default 3 hours
    
    long chargerCount = 3;
    
    long planeCount = 20;
    
    long minPlanePerKind = 1;
};

// These are used to help us convert from hours and minutes to seconds
const long defaultTestClockSeconds{60*60*3}; //3 hours
const long shortTestClockSeconds{60}; //1 minute
const long longTestClockSeconds{60*60*30}; //30 hours

#endif /* SimSettings_hpp */
