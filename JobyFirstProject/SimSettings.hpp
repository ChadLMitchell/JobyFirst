//
//  SimSettings.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef SimSettings_hpp
#define SimSettings_hpp

#include <stdio.h>

extern int passengerCountOption;
// 0 = plane always flies full
// 1 = randomly distribute number of passengers between 1 and max for each flight

extern int faultOption;
// 0 = just count faults
// 1 = fault grounds plan for duration

extern long simulationDuration;

#endif /* SimSettings_hpp */
