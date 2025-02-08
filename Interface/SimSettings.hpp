//
//  SimSettings.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef SimSettings_hpp
#define SimSettings_hpp

#define SORTED_EVENT_QUEUE_TYPE 2
// 0 = vector
// 1 = list
// 2 = multiset
#define SORTED_PLANE_QUEUE_TYPE 1
// 0 = vector
// 1 = list
// 2 = multiset
#define SORTED_CHARGER_QUEUE_TYPE 2
// 0 = vector
// 1 = list
// 2 = multiset

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>

// These are used to help us convert from hours and minutes to seconds
const long secondsPerMinute{60}; // Converting seconds to minutes and minutes to seconds
// Despite labels still saying "seconds", changing this allows using a minute based clock
// secondsPerMinute == 60: clock granularity is seconds
// secondsPerMinute == 1: clock granularity is minutes
// Other examples:
// secondsPerMinute == 2: clock granularity is half minutes (30 seconds)
// secondsPerMinute == 10: clock granularity is 6 seconds
// secondsPerMinute == 30: clock granularity is 2 seconds
// The user output automatically adjusts if you set this to 60 or to 1, but otherwise is incorrect

const long secondsPerHour{secondsPerMinute * 60}; // Converting seconds to hours and hours to seconds
const double secondsPerHourD{secondsPerMinute * 60.0}; // For floating point calcuations
const long defaultTestClockSeconds{secondsPerHour * 3}; //3 hours

// How do we seed the random number generators?
const long useRandomSeed = 100;
// If this is non-zero, use this number to seed all random number generators
// Otherwise use time(0) or (time(0) & INT_MAX)

/*
 *******************************************************************************************
 * Struct SimSettings
 * These are settings for the simulator. We can pass a copy of these into the Simulator
 * constructor to run with these. To use the defaults, create a SimSettings object with
 * the default values. To run with modified settings, create a copy of this struct and
 * modify it before passing it to a Simulator object.
 *******************************************************************************************
 */
struct SimSettings {
    
    // How long with this simulation run (in simulated seconds)?
    long simulationDuration = defaultTestClockSeconds; // default 3 hours
    
    // How may chargers will we have for this simulation?
    long chargerCount = 3;
    
    // How many planes will we have for this simulation?
    long planeCount = 20;
    
    // When we "randomly" allocate planes, do we require a minimum number of each plane.
    // If this is set to 0, we may sometimes have none of one or more kinds.
    // If this is set to planeCount / companyCount (rounded down) then we will have an even or
    // equal distribution of evey kind of planes, still randomly allocated. This can be set
    // at any value [0 - planeCount / companyCount]
    long minPlanePerKind = 1; // Default is 1 we always have statistics for all kinds of planes
    
    // Do we always fill the planes or are they sometimes parially filled?
    int passengerCountOption = 0;
    // 0 = plane always flies full
    // 1 = randomly distribute number of passengers between 1 and max for each flight
    
    // Is there a delay getting passengers when a plane is ready to fly?
    long maxPassengerDelay = 0;
    // 0 means passengers are always available when we have a plane ready
    // > 0 means passengers may not be available when a plane is ready.
    // In the later case, when a plane is ready it will wait a random
    // amount of time from 0 to maxPassengerDelay until starting the next
    // flight.
    
    // How do we handle faults? Do we just count them or do they affect operations?
    int faultOption = 0;
    // 0 = just count faults
    // 1 = fault grounds plane immediately for duration of simulation
    // 2 = fault grounds plane at the end of current flight for duration of simulation

    // Do we show progress as the simulation proceeds?
    int progressInterval = -1; // in hours, 0 == do not show, -1 == not yet set
    // If they are on a monitor that does not honor '\r' this will fill their screen with
    // rows of progress. But if their monitor honors '\r' this will show progress.
    
    // If the simulation passes eitehr of these two limits, then it may show probress if
    // progressInterval != -1
    const int progressMoreThanHours = 1000; // only show progress indicator if duration more than this
    const int progressMoreThanPlanes = 100; // only show progress indicator if planes more than this
    const int progressIntervalValue = 100; // default for how often we show progress, if we show it at all
};

/*
 *******************************************************************************************
 * Companies (types of planes)
 * Later we could make these options as well, but for now they are hard wired into the
 * simulation so we provide these constants to work with them. It is easy to change these
 * here, but they are not currently user settable options like those above.
 *******************************************************************************************
 */
// enumeration of the possible companies (kinds of planes)
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
