//
//  ChargerQueue.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#include "ChargerQueue.hpp"

ChargerQueue::ChargerQueue(long chargerCount):
        EventHandler(), chargerCount{chargerCount}, chargers{}, planesWaiting{} {
}
ChargerQueue::~ChargerQueue() {
}
bool ChargerQueue::handleEvent(long currentTime) {
    // DO HANDLE EVENT ********************************************
    return false;
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
        
        
        if(theSimClock && nextEventTime != chargers.back().timeDone) {// if our earliest charger done time has changed, we need to be resorted
            nextEventTime = chargers.back().timeDone;
            theSimClock->markNeedSort();
        }
    }
}

std::shared_ptr<ChargerQueue> theChargerQueue = nullptr;
