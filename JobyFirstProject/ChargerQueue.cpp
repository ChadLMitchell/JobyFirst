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
EventHandler(LONG_MAX), theSimulation{theSimulation}, chargerCount{chargerCount}, verboseTesting{false},chargers{}, planesWaiting{} {
}
ChargerQueue::~ChargerQueue() {
}
bool ChargerQueue::handleEvent(long currentTime) {
    if(chargers.empty()) {
        std::cout << "ChargerQueue::handleEvent should not be called with no chargers in use" <<  std::endl;
        return false;
    }
    // Handle any planes that are now fully charged
    while(!chargers.empty() && chargers.back().timeDone <= currentTime) {
        std::shared_ptr<Plane> thePlane = chargers.back().thePlane;
        chargers.pop_back();
        if(theSimulation && theSimulation->theSimClock) {
            theSimulation->theSimClock->addHandler(std::make_shared<Flight>(theSimulation, currentTime + thePlane->calcTimeOnFullCharge__seconds(), thePlane->calcPassengerCount(),thePlane));
        } else if(verboseTesting) {
            std::cout << "Would add flight for " << thePlane->describe() << "to SimClock" << std::endl;
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
        // Insert the new charger into the first location that will keep it sorted.
        // We pull from the end so equal next Time produces FiFO
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
void ChargerQueue::describeQueues(long currentTime) {
    std::cout << "*** ChargerQueue Status" << std::endl;
    std::cout << "Current Time: " << currentTime << std::endl;
    std::cout << "Planes on Chargers" << std::endl;
    for(auto aCharger: chargers) {
        std::cout << "    " << aCharger.thePlane->describe() << " done at " << aCharger.timeDone << std::endl;
    }
    std::cout << "Planes waiting for a Charger" << std::endl;
    // To view a queue we must pull everything off of it (and then put it all back)
    std::vector<std::shared_ptr<Plane>> tempPlanes{};
    // pull planes off planesWaiting, describe them and save them in tempPlanes
    while(!planesWaiting.empty()) {
        std::shared_ptr<Plane> aPlane = planesWaiting.front();
        planesWaiting.pop();
        std::cout << "    " << aPlane->describe() << std::endl;
        tempPlanes.push_back(aPlane);
    }
    // restore planes to tempQueue
    for(auto planeToRestore: tempPlanes) {
        planesWaiting.push(planeToRestore);
    }

    // This code is to make sure we restored the planes correctly
    std::cout << "Repeat planes waiting for a Charger" << std::endl;
    // pull planes off planesWaiting, describe them and save them in tempPlanes
    while(!planesWaiting.empty()) {
        std::shared_ptr<Plane> aPlane = planesWaiting.front();
        planesWaiting.pop();
        std::cout << "    " << aPlane->describe() << std::endl;
        tempPlanes.push_back(aPlane);
    }
    // restore planes to tempQueue
    for(auto planeToRestore: tempPlanes) {
        planesWaiting.push(planeToRestore);
    }
}
bool ChargerQueue::isEmpty() {
    return planesWaiting.empty() && chargers.empty();
}

const int testChargers{3};
const int testPlanes{5};

bool testChargerQueue(bool verbose) {
    bool returnValue = true;
    long currentTime = 0;
    ChargerQueue aQueue(nullptr, testChargers);
    std::cout << "Starting test of ChargerQueue" << std::endl;
    for(auto i=0; i<testPlanes; i++){
        std::shared_ptr<Plane> aPlane = Plane::getRandomPlane();
        std::cout << "Adding " << aPlane->describe() << std::endl;
        aQueue.addPlane(currentTime, aPlane);
    }
    aQueue.describeQueues(currentTime);
    while(!aQueue.isEmpty()) {
        currentTime = aQueue.getNextEventTime();
        std::cout << std::endl;
        std::cout << "After handling event at time " << currentTime << std::endl;
        aQueue.handleEvent(currentTime);
        aQueue.describeQueues(currentTime);
    }
    
    return returnValue;
}
