//
//  ChargerQueue.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#include "ChargerQueue.hpp"
#include "Plane.hpp"
#include "Flight.hpp"

ChargerQueue::ChargerQueue(Simulation *theSimulation, long chargerCount):
        EventHandler(LONG_MAX), theSimulation{theSimulation}, chargerCount{chargerCount}, chargers{}, planesWaiting{} {
}
ChargerQueue::~ChargerQueue() {
}
bool ChargerQueue::handleEvent(long currentTime) {
    if(chargers.empty()) {
        std::cout << "ChargerQueue::handleEvent should not be called with no chargers in use" <<  std::endl;
        return false;
    }
    // Handle any planes that are now fully charged
    while(!chargers.empty() && chargers.back().timeDone == currentTime) {
        std::shared_ptr<Plane> thePlane = chargers.back().thePlane;
        chargers.pop_back();
        if(theSimulation && theSimulation->theSimClock) {
            theSimulation->theSimClock->addHandler(std::make_shared<Flight>(theSimulation, currentTime + thePlane->calcTimeOnFullCharge__seconds(), thePlane->calcPassengerCount(),thePlane));
        }
    }
    // move planes from waiting queue to charger vector
    while(!planesWaiting.empty() && chargers.size() < chargerCount) {
        addPlane(currentTime, planesWaiting.front());
        planesWaiting.pop();
    }
    return true;
}
void ChargerQueue::addPlane(long currentTime, std::shared_ptr<Plane> aPlane) {
    if(chargers.size() >= chargerCount) {
        planesWaiting.push(aPlane);
    } else {
        // Keep chargers sorted with soonest time at the end.
        // Insert the new charger into the first location to keep it sorted.
        long timeToCharged = currentTime + aPlane->calcTimeToCharge__seconds();
        auto chargerPtr = begin(chargers);
        while(chargerPtr != end(chargers) &&  chargerPtr->timeDone > timeToCharged) {
            chargerPtr++;
        }
        chargers.insert(chargerPtr, Charger(timeToCharged, aPlane));
        
        
        // if our earliest charger done time has changed, we need to be resorted
        if(nextEventTime != chargers.back().timeDone) {
            if(theSimulation && theSimulation->theSimClock && theSimulation->theChargerQueue) {
                nextEventTime = chargers.back().timeDone;
                theSimulation->theSimClock->reSortHandler(theSimulation->theChargerQueue);
            }
        }
    }
}
