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
    long neededOfCompany[companyCount]{};
    for(long i = 0; i < companyCount; i++) {
        neededOfCompany[i] = minOfEachCompany;
    }
    
    long totalCompanyStillNeeded{minOfEachCompany *companyCount};
    
    // Allocate planes to random Companies, with constraints
    std::vector<Company> companyChoices(count);
    for(long planesAllocated = 0; planesAllocated < count; planesAllocated++) {
        // initally they can be totally random.
        long thisCompany = rand() % companyCount;
        // As long as we need some minimums, make sure we do those first
        if(totalCompanyStillNeeded > 0) {
            // if so, adjust the selection until we find a Company needed
            long originalCompany = thisCompany; // save it to test for infinite loop
            while(neededOfCompany[thisCompany] <= 0) {
                // increase it, wrapping around
                // Warning, if this code is not right it could create an infinite loop
                thisCompany = (thisCompany + 1) % companyCount;
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

    // We do the actual allocation of planes as a separate step in case we want to add more processing first
    for(long thisChoice: companyChoices) {
        // set up this plane's wait for passengers
        long waitForPassengers = 0;
        if(theSimulation) {
            waitForPassengers = Passenger::getPassengerDelay(theSimulation->theSettings);
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
        PlaneQueueStatusItem anItem{aPlaneQueueItem.thePlane->getPlaneNumber(), aPlaneQueueItem.thePlane->getCompany(), aPlaneQueueItem.nextFlightTime};
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
bool testPlaneQueueRandomMinimum() {
    const long testPlanes{20};
    const long ofEachKind[]{0, 1, 2, 3, 4};

    bool returnValue = true;
    long currentTime = 0;
    std::cout << "Starting test of PlaneQueue" << std::endl;
    
    for(auto ofEach: ofEachKind) {
        std::cout << "With the minimum of each kind == " << ofEach << ":" << std::endl;
        // Set up a PlaneQueue and populate it with planes
        PlaneQueue aQueue(nullptr);
        aQueue.setVerboseTesting(true);
        aQueue.generatePlanes(currentTime, testPlanes, ofEach);
        // Get the list of planes
        std::vector<PlaneQueueStatusItem> queueStatus = aQueue.getQueueStatus();
        // Count the kinds of planes
        long kindCounts[companyCount]{};
       // Output the list the planes
        std::cout << "Planes on PlanesQueue:" << std::endl;
        for(auto statusItem: queueStatus) {
            kindCounts[statusItem.planeCompany]++;
            std::cout << "    Plane #" << statusItem.planeNumber
            << " from " << companyName(statusItem.planeCompany)
            << " next flight time: " << statusItem.nextFlightTime << std::endl;
        }
        // Output the kind counts
        std::cout << "Planes counts:" << std::endl;
        for(auto kind: allCompany) {
            std::cout << "    " << kindCounts[kind]
            << " from " << companyName(kind) << std::endl;
        }
        std::cout << std::endl;
    }
    return returnValue;
}
