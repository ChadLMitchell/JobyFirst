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

#include "SimSettings.hpp"
#include "Simulation.hpp"


/*
 *******************************************************************************************
 * class Passenger
 * This virtual class provides access to two static functions that manage passenger information.
 * If there is later a need to model passenger behavior, this could become a regular class.
 *******************************************************************************************
 */
class Passenger {
    Passenger() = delete;

public:
    // This provides the number of passengers on a flight. It takes the maximum number
    // and the settings and determines if it should return that maximum number or a
    // random number between 1 and the maximum.
    static long getPassengerCount(Company aCompany, Simulation *theSimulation);

    // This determines how long a delay there will be for a particular flight.
    // Depending on the settings it may return 0 delay or some random delay
    static long getPassengerDelay(long maxPassengerDelay, Simulation *theSimulation);
};

#endif /* Passenger_hpp */
