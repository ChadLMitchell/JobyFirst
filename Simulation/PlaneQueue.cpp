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
        std::cout << std::endl;
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
    if(verboseTesting) {
        std::cout << "Adding " << aPlane->describe() << " with delay until " << delayUntil << std::endl;
    }
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
void PlaneQueue::generatePlanes(long currentTime, long count, long minOfEachCompany, long maxPassengerDelay) {
    
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
        long waitForPassengers = Passenger::getPassengerDelay(maxPassengerDelay);
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
           if(verboseTesting) {
               std::cout << "    " << aPlaneQueueItem.thePlane->describe() << " next fight time " << aPlaneQueueItem.nextFlightTime << std::endl;
           } else {
               std::cout << "    " << aPlaneQueueItem.thePlane->describe() << std::endl;
           }
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

bool testPlaneQueueWaits() {
    const int testPlanes{5};
    const long ofEachKind{1};
    const long maxWait{20};

    bool returnValue = true;
    long currentTime = 0;
    PlaneQueue aQueue(nullptr);
    aQueue.setVerboseTesting(true);
    std::cout << " ***** Starting Test of PlaneQueue Waits *****" << std::endl;
    aQueue.generatePlanes(currentTime, testPlanes, ofEachKind, maxWait);
    aQueue.describeQueue(currentTime);
    while(!aQueue.isEmpty()) {
        currentTime = aQueue.getNextEventTime();
        std::cout << std::endl;
        std::cout << "Handle event at time " << currentTime << std::endl;
        aQueue.handleEvent(currentTime, false);
        aQueue.describeQueue(currentTime);
    }
    std::cout << "Completed Test of PlaneQueue Waits" << std::endl;
    std::cout << std::endl;
    return returnValue;
}
bool testPlaneQueueMinimumPerKind() {
    const long testPlanes{10};
    const long ofEachKindArray[]{0, 1, 2};
    const long repeatGenerations{5}; // How many times to try each option

    bool returnValue = true;
    long currentTime = 0;
    std::cout << "***** Starting test of PlaneQueue Plane Allocation *****" << std::endl;
    
    for(auto ofEach: ofEachKindArray) {
        std::cout << "With the minimum of each kind == " << ofEach << ":" << std::endl;
        for(auto repetition = 0; repetition < repeatGenerations; repetition++) {
            // Set up a PlaneQueue and populate it with planes
            PlaneQueue aQueue(nullptr);
            aQueue.generatePlanes(currentTime, testPlanes, ofEach, 0);
            // Get the list of planes
            std::vector<PlaneQueueStatusItem> queueStatus = aQueue.getQueueStatus();
            // Count the kinds of planes
            long kindCounts[companyCount]{};
            for(auto statusItem: queueStatus) {
                kindCounts[statusItem.planeCompany]++;
            }
            // Output the kind counts
            std::cout << "Plane counts generated for test #" << repetition + 1 << ":" << std::endl;
            for(auto kind: allCompany) {
                std::cout << "    " << kindCounts[kind]
                << " from " << companyName(kind) << std::endl;
            }
        }
        std::cout << std::endl;
    }
    std::cout << "Completed test of PlaneQueue Plane Allocation" << std::endl;
    std::cout << std::endl;
    return returnValue;
}
