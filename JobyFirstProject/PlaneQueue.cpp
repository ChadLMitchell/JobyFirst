//
//  PlaneQueue.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#include "PlaneQueue.hpp"
#include "Flight.hpp"
#include "Passenger.hpp"

PlaneQueue::PlaneQueue(Simulation *theSimulation):
theSimulation{theSimulation}, verboseTesting{}, planesWaiting{} {
}
PlaneQueue::~PlaneQueue() {
}
bool PlaneQueue::handleEvent(long currentTime, bool closeOut) {
    if(closeOut) {
        return false;
    }
    if(planesWaiting.empty()) {
        std::cout << "PlaneQueue::handleEvent should not be called with no planess in use" <<  std::endl;
        std::cout << "  Curent time: " << currentTime << std::endl;
        std::cout << "  Next Event time: " << nextEventTime << std::endl;
        return false;
    }
    while(!planesWaiting.empty() && planesWaiting.back().nextFlightTime <= currentTime) {
        std::shared_ptr<Plane> thePlane = planesWaiting.back().thePlane;
        planesWaiting.pop_back();
        if(theSimulation && theSimulation->theSimClock) {
            theSimulation->theSimClock->addHandler(std::make_shared<Flight>(
                                         theSimulation,
              currentTime, Passenger::getPassengerCount(thePlane->getMaxPassengerCount(),theSimulation->theSettings),thePlane));
        } else if(verboseTesting) {
            std::cout << "Would add flight for " << thePlane->describe() << "to SimClock if full simulation" << std::endl;
        }
    }
    if(planesWaiting.empty()) {
        nextEventTime = LONG_MAX;
    } else  {
        nextEventTime = planesWaiting.back().nextFlightTime;
    }
    return true;
}
long PlaneQueue::countPlanes() {
    return planesWaiting.size();
}
const std::string PlaneQueue::describe() {
    std::string description = "Plane Queue containing " + std::to_string(countPlanes()) + " planes";
    return description;
}
bool PlaneQueue::isEmpty() {
    return planesWaiting.empty();
}
void PlaneQueue::addPlane(long delayUntil, std::shared_ptr<Plane> aPlane) {
    auto planePtr = begin(planesWaiting);
    while(planePtr != end(planesWaiting) &&  planePtr->nextFlightTime > delayUntil) {
        planePtr++;
    }
    planesWaiting.insert(planePtr, PlaneQueueItem(aPlane, delayUntil));
    
    // if our nextFlightTime time has changed, we need to be resorted in the SimClock
    if(nextEventTime != planesWaiting.back().nextFlightTime) {
        nextEventTime = planesWaiting.back().nextFlightTime; // adjust the next time for our queue
        if(theSimulation && theSimulation->theSimClock && theSimulation->thePlaneQueue) {
            theSimulation->theSimClock->reSortHandler(theSimulation->thePlaneQueue);
        }
    }

}
void PlaneQueue::generatePlanes(long currentTime, long count, long minOfEachKind) {
// TO DO: create planes with constraints on minOfEachKind
// TO DO: use settings to select value for delayUntil
    // ********************************************************
    
    for(long i = 0; i < count; i++) {
        addPlane(0, Plane::getRandomPlane());
    }

}
std::shared_ptr<Plane> PlaneQueue::removeNextPlane() {
    if(planesWaiting.empty()) {
        return nullptr;
    }
    std::shared_ptr<Plane> returnValue = planesWaiting.back().thePlane;
    planesWaiting.pop_back();

    // if our earliest nextFlightTime time has changed, we need to be resorted in the SimClock
    if(nextEventTime != planesWaiting.back().nextFlightTime) {
        nextEventTime = planesWaiting.back().nextFlightTime; // adjust the next time for our queue
        if(theSimulation && theSimulation->theSimClock && theSimulation->thePlaneQueue) {
            theSimulation->theSimClock->reSortHandler(theSimulation->thePlaneQueue);
        }
    }
    return returnValue;
}
void PlaneQueue::setVerboseTesting(bool newValue) {
    verboseTesting = newValue;
}
void PlaneQueue::describeQueue(long currentTime) {
    std::cout << "*** PlaneQueue Status" << std::endl;
    std::cout << "Current Time: " << currentTime << std::endl;
    if(planesWaiting.size() == 0) {
        std::cout << "No planes in PlaneQueue" << std::endl;
   } else {
        std::cout << "Planes in PlaneQueue" << std::endl;
       for(auto aPlaneQueueItem: planesWaiting) {
            std::cout << "    " << aPlaneQueueItem.thePlane->describe() << " next fight time " << aPlaneQueueItem.nextFlightTime << std::endl;
        }
    }
    std::cout << std::endl;
}
std::vector<PlaneQueueStatusItem> PlaneQueue::getQueueStatus() {
    std::vector<PlaneQueueStatusItem> result{};
    for(auto aPlaneQueueItem: planesWaiting) {
        result.push_back(PlaneQueueStatusItem(aPlaneQueueItem.thePlane->getPlaneNumber(),aPlaneQueueItem.nextFlightTime));
    }
    return result;

}

bool testPlaneQueueLong() {
    const int testPlanes{5};

    bool returnValue = true;
    long currentTime = 0;
    PlaneQueue aQueue(nullptr);
    std::cout << "Starting long test of PlaneQueue" << std::endl;
    for(auto i=0; i<testPlanes; i++){
        std::shared_ptr<Plane> aPlane = Plane::getRandomPlane();
        std::cout << "Adding " << aPlane->describe() << std::endl;
        aQueue.addPlane(currentTime, aPlane);
    }
    aQueue.describeQueue(currentTime);
    while(!aQueue.isEmpty()) {
        currentTime = aQueue.getNextEventTime();
        std::cout << std::endl;
        std::cout << "After handling event at time " << currentTime << std::endl;
        aQueue.handleEvent(currentTime, false);
        aQueue.describeQueue(currentTime);
    }
    
    return returnValue;
}
bool testPlaneQueueShort() {
    const int testPlanes{5};

    bool returnValue = true;
    long currentTime = 0;
    PlaneQueue aQueue(nullptr);
    aQueue.setVerboseTesting(true);
    std::cout << "Starting short test of PlaneQueue" << std::endl;
    for(auto i=0; i<testPlanes; i++){
        std::shared_ptr<Plane> aPlane = Plane::getRandomPlane();
        aQueue.addPlane(currentTime, aPlane);
    }

    std::vector<PlaneQueueStatusItem> queueStatus = aQueue.getQueueStatus();
    std::cout << "Planes on PlanesQueue:" << std::endl;
    for(auto statusItem: queueStatus) {
        // TO DO Instead of printing, we could feed in a specific set of planes and then confirm that the results are as expected.
        // That would be more of an automated test like some of the others. We could also do some time jumps and confirm those results.
        std::cout << "    Plane #" << statusItem.planeNumber << " next flight time: " << statusItem.nextFlightTime << std::endl;

    }
    
    return returnValue;
}
