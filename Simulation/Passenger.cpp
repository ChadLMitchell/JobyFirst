//
//  Passenger.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#include "Passenger.hpp"

long Passenger::getPassengerCount(long maxPassengers,  std::shared_ptr<SimSettings> theSettings) {
    // for now, always load each plane full
    return maxPassengers;
    // TO DO implement other strategies such as having a distribution of passengers.
    // Base it on settings in the simulation.
    // But have a default in case theSettings == nullptr;
};
long Passenger::getPassengerDelay(std::shared_ptr<SimSettings> theSettings) {
    // for now, never wait for passengers
    return 0;
    // TO DO implement other strategies such as having a distribution of passengers.
    // Base it on settings in the simulation.
    // But have a default in case theSettings == nullptr;
}
