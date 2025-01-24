//
//  SimSettings.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef SimSettings_hpp
#define SimSettings_hpp

#include <stdio.h>

struct SimSettings {
    
    int passengerCountOption = 0;
    // 0 = plane always flies full
    // 1 = randomly distribute number of passengers between 1 and max for each flight

    int faultOption = 0;
    // 0 = just count faults
    // 1 = fault grounds plan for duration

    long simulationDuration = 60*60*3; // default 3 hours
    
    long chargerCount = 3;
    
    long planeCount = 20;
    
    long minPlanePerKind = 1;
};

#endif /* SimSettings_hpp */
