//
//  Passenger.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#include <stdio.h>
#include <random>
#include "Passenger.hpp"


std::random_device Passenger::a_random_device;
std::mt19937 Passenger::generator(a_random_device());

// This is called at the start of each flight to get the number of passengers.
// There is an alternative controlled by the passengerCountOption setting.
// The default value (0) is to always fly with a full plane.
// If passengerCountOption == 1 then each plane files with a randome number
// of passengers in the range [1 - maxPassengers].
long Passenger::getPassengerCount(long maxPassengers,  std::shared_ptr<SimSettings> theSettings) {
    if(theSettings == nullptr || theSettings->passengerCountOption == 0) {
        return maxPassengers;
    }
    std::uniform_int_distribution<> passengerDistribution(1, static_cast<int>(maxPassengers));
    return passengerDistribution(generator);
};

// This determines how long a delay there will be for a particular flight.
// Depending on the settings it may return 0 delay or some randome delay;
// The behavior is controlled by the maxPassengerDelay option.
// The default value (0) always gives 0 delay. But if maxPassengerDelay > 0
// then each time a plane is put in PlaneQueue it is given a delay for passengers arriving
// in the range [0 - maxPassengerDelay]. Planes that are grounded are also put into
// PlaneQueue, but are given a dealy of LONG_MAX so they never are assigned to a flight.
long Passenger::getPassengerDelay(std::shared_ptr<SimSettings> theSettings) {
    if(theSettings == nullptr || theSettings->maxPassengerDelay <= 0) {
        return 0;
    }
    std::uniform_int_distribution<> delayDistribution(0, static_cast<int>(theSettings->maxPassengerDelay));
    return delayDistribution(generator);
}
