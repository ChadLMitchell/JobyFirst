//
//  Passenger.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#ifndef Passenger_hpp
#define Passenger_hpp

#include <stdio.h>
#include <iostream>
#include "SimSettings.hpp"

class Passenger {
    Passenger() = delete;
public:
    static long getPassengerCount(long maxPassengers, std::shared_ptr<SimSettings> theSettings);
    static long getPassengerDelay(std::shared_ptr<SimSettings> theSettings);
};

#endif /* Passenger_hpp */
