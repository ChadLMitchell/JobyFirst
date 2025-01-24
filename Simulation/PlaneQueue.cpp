//
//  PlaneQueue.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#include "PlaneQueue.hpp"
#include "Flight.hpp"
#include "Passenger.hpp"

extern PlaneSpecification planeSpecifications[];

PlaneQueue::PlaneQueue(Simulation *theSimulation):
EventHandler(LONG_MAX),theSimulation{theSimulation}, verboseTesting{}, planesWaiting{} {
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
    return static_cast<long>(planesWaiting.size());
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
    PlaneQueueItem queueItem = PlaneQueueItem{aPlane, delayUntil};
    planesWaiting.insert(planePtr, queueItem);
    
    // if our nextFlightTime time has changed, we need to be resorted in the SimClock
    if(nextEventTime != planesWaiting.back().nextFlightTime) {
        nextEventTime = planesWaiting.back().nextFlightTime; // adjust the next time for our queue
        if(theSimulation && theSimulation->theSimClock && theSimulation->thePlaneQueue) {
            theSimulation->theSimClock->reSortHandler(theSimulation->thePlaneQueue);
        }
    }

}
// Generate "count" planes semi-randomly. Make sure at least "minOfEachKind" are generated
// for each kind. This algorithm can only work correctly if
//      count >= minOfEachKind * numberOfKinds.
void PlaneQueue::generatePlanes(long currentTime, long count, long minOfEachCompany) {
    
    // Set up an array of how many minimum are needed of each kind
    long neededOfCompany[maxCompany + 1]{};
    for(long i = 0; i <= maxCompany; i++) {
        neededOfCompany[i] = minOfEachCompany;
    }
    long totalCompanyStillNeeded{minOfEachCompany * (maxCompany + 1)};
    
    // Allocate planes to random Companies, with constraints
    std::vector<Company> companyChoices(count);
    for(long planesAllocated = 0; planesAllocated < count; planesAllocated++) {
        // initally they can be totally random.
        long thisCompany = rand() % (maxCompany + 1);
        // As long as we need some minimums, make sure we do those
        if(totalCompanyStillNeeded > 0) {
            // if so, adjust the selection until we find a Company needed
            long originalCompany = thisCompany; // save it to test for infinite loop
            while(neededOfCompany[thisCompany] <= 0) {
                // increase it, wrapping around
                // Warning, if this code is not right it could create an infinite loop
                thisCompany = (thisCompany + 1) % (maxCompany + 1);
                if(thisCompany == originalCompany) {
                    // We have wrapped around and not found what we needed so stop trying
                    std::cout << "Cleared infinite loop. planesAllocated: " << planesAllocated <<
                    " count: " << count <<
                    " totalCompanyStillNeeded: " << totalCompanyStillNeeded << std::endl;
                    break;
                }
            }
        }
        // Mark the Company used, if it still needs it
        if(neededOfCompany[thisCompany] > 0) {
            neededOfCompany[thisCompany]--;
            totalCompanyStillNeeded--;
        }
        // Remember the choice
        companyChoices[planesAllocated] = allCompany[thisCompany];
    }
    // Prefetch the maximum passenger delay
    long maxDelay = 0;
    if(theSimulation && theSimulation->theSettings && theSimulation->theSettings->maxPassengerDelay > 0) {
        maxDelay = theSimulation->theSettings->maxPassengerDelay;
    }
    // We do the actual allocation of planes as a separate step in case we want to add more processing first
    for(long choiceIndex: companyChoices) {
        Company thisChoice = companyChoices[choiceIndex];
        // set up this plane's wait for passengers
        long waitForPassengers = 0;
        if(maxDelay > 0) {
            waitForPassengers = rand() % maxDelay;
        }
        // actually add the random plane
        addPlane(waitForPassengers, std::make_shared<Plane>(planeSpecifications[thisChoice]));
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
        PlaneQueueStatusItem anItem{aPlaneQueueItem.thePlane->getPlaneNumber(),aPlaneQueueItem.nextFlightTime};
        result.push_back(anItem);
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
