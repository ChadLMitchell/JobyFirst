//
//  Passenger.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#ifndef Passenger_hpp
#define Passenger_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <random>

#include "SimSettings.hpp"


/*
 *******************************************************************************************
 * class Passenger
 * This virtual class provides access to two routines that manage passenger information.
 * If there is later a need to model passenger behavior, this could become a regular class.
 *******************************************************************************************
 */
class Passenger {
    Passenger() = delete;

    static std::random_device a_random_device;
    static std::mt19937 generator;
public:
    // This provides the number of passengers on a flight. It takes the maximum number
    // and the settings and determines if it should return that maximum number or a
    // random number between 1 and the maximum.
    static long getPassengerCount(long maxPassengers, std::shared_ptr<SimSettings> theSettings);

    // This determines how long a delay there will be for a particular flight.
    // Depending on the settings it may return 0 delay or some randome delay
    static long getPassengerDelay(std::shared_ptr<SimSettings> theSettings);
};

#endif /* Passenger_hpp */
