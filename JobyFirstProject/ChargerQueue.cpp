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
    if(chargers.empty()) {
        nextEventTime = LONG_MAX;
    } else  {
        nextEventTime = chargers.back().timeDone;
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
        nextEventTime = chargers.back().timeDone; // adjust the next time for our queue
        
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
    if(chargers.size() == 0) {
        std::cout << "No planes planes on a charger" << std::endl;
   } else {
        std::cout << "Planes on Chargers" << std::endl;
        for(auto aCharger: chargers) {
            std::cout << "    " << aCharger.thePlane->describe() << " done at " << aCharger.timeDone << std::endl;
        }
    }
    if(planesWaiting.empty()) {
        std::cout << "No planes waiting for a Charger" << std::endl;
   } else {
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
    }
    std::cout << std::endl;
}
bool ChargerQueue::isEmpty() {
    return planesWaiting.empty() && chargers.empty();
}
std::vector<ChargerQueueStatusItem> ChargerQueue::getQueueStatus() {
    std::vector<ChargerQueueStatusItem> result{};
    for(auto aCharger: chargers) {
        result.push_back(ChargerQueueStatusItem(true,aCharger.thePlane->getPlaneNumber()));
    }

    // To view a queue we must pull everything off of it (and then put it all back)
    std::vector<std::shared_ptr<Plane>> tempPlanes{};
    // pull planes off planesWaiting, describe them and save them in tempPlanes
    while(!planesWaiting.empty()) {
        std::shared_ptr<Plane> aPlane = planesWaiting.front();
        planesWaiting.pop();
        result.push_back(ChargerQueueStatusItem(false,aPlane->getPlaneNumber()));
        tempPlanes.push_back(aPlane);
    }
    // restore planes to tempQueue
    for(auto planeToRestore: tempPlanes) {
        planesWaiting.push(planeToRestore);
    }
    return result;
}

const int testChargers{3};
const int testPlanes{5};

bool testChargerQueueLong() {
    bool returnValue = true;
    long currentTime = 0;
    ChargerQueue aQueue(nullptr, testChargers);
    std::cout << "Starting long test of ChargerQueue" << std::endl;
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
bool testChargerQueueShort() {
    bool returnValue = true;
    long currentTime = 0;
    ChargerQueue aQueue(nullptr, testChargers);
    std::cout << "Starting short test of ChargerQueue" << std::endl;
    for(auto i=0; i<testPlanes; i++){
        std::shared_ptr<Plane> aPlane = Plane::getRandomPlane();
        aQueue.addPlane(currentTime, aPlane);
    }

    std::vector<ChargerQueueStatusItem> queueStatus = aQueue.getQueueStatus();
    bool doingOnChargers = true;
    std::cout << "Planes on chargers:" << std::endl;
    for(auto statusItem: queueStatus) {
        // TO DO Instead of printing, we could feed in a specific set of planes and then confirm that the results are as expected.
        // That would be more of an automated test like some of the others. We could also do some time jumps and confirm those results.
        if(doingOnChargers && !statusItem.onCharger) {
            std::cout << "Planes waiting for chargers:" << std::endl;
            doingOnChargers = false;
        }
        std::cout << "    Plane #" << statusItem.planeNum << std::endl;

    }
    
    return returnValue;
}
