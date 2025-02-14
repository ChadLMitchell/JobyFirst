//
//  Passenger.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#include <stdio.h>
#include "Passenger.hpp"
#include "PlaneQueue.hpp"

extern PlaneSpecification planeSpecifications[];


// This is called at the start of each flight to get the number of passengers.
// There is an alternative controlled by the passengerCountOption setting.
// The default value (0) is to always fly with a full plane.
// If passengerCountOption == 1 then each plane files with a randome number
// of passengers in the range [1 - maxPassengers].
long Passenger::getPassengerCount(Company aCompany, Simulation *theSimulation) {
    
    // if there are no settings or the option is 0, planes fly full
    if(theSimulation == nullptr || theSimulation->theSettings == nullptr ||
       theSimulation->theRandomGenerators == nullptr || theSimulation->theSettings->passengerCountOption == 0) {
        return planeSpecifications[aCompany].passenger_count;
    }
    // Get a random number of passengers in [1 - maxPassengers]
    return theSimulation->theRandomGenerators->getRandomPassengerCount(aCompany);
};

// This determines how long a delay there will be for a particular flight.
// Depending on the settings it may return 0 delay or some randome delay;
// The behavior is controlled by the maxPassengerDelay option.
// The default value (0) always gives 0 delay. But if maxPassengerDelay > 0
// then each time a plane is put in PlaneQueue it is given a delay for passengers arriving
// in the range [0 - maxPassengerDelay]. Planes that are grounded are also put into
// PlaneQueue, but are given a dealy of LONG_MAX so they never are assigned to a flight.
long Passenger::getPassengerDelay(long maxPassengerDelay, Simulation *theSimulation) {
    // If the maximum delay is not greater than 0 then there is no delay
    if(maxPassengerDelay <= 0 || theSimulation == nullptr ||
       theSimulation->theRandomGenerators == nullptr) {
        return 0;
    }
        // Get a random number of delays in [0 - maxPassengerDelay]
    return theSimulation->theRandomGenerators->getRandomPassengerDelay();
}
