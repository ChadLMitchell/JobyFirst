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

/*
 *******************************************************************************************
 * Class PlaneQueue
 * This manages a vector of planes waiting for passengers. If the options are that planes
 * never wait for passengers, then this is a place to create the initial collection of planes.
 * It is also a place to put planes that are grounded due to faults if that option is chosen.
 *
 * This is a child of eventHandler. As such, it will be added to the SimClock queue.
 *
 * A Simulation object will contain exactly one PlaneQueue.
 * If the PlaneQueue object is in a Simulation it has a pointer to that Simulation object.
 * For testing, a PlaneQueue may have a nullptr for theSimulation property.
 *******************************************************************************************
 */
PlaneQueue::PlaneQueue(Simulation *theSimulation, std::shared_ptr<RandomGenerators> theRandomGenerators):
// Set our nextEventTime initially to LONG_MAX so we can be in the SimClock handler list, but
// not receive events until something happens to activate us such as adding planes that
// are ready to fly.
EventHandler(LONG_MAX),theSimulation{theSimulation}, theRandomGenerators{theRandomGenerators}, verboseTesting{}, planesWaiting{} {
}
PlaneQueue::~PlaneQueue() {
}

// As a child of EventHandler, PlaneQueue has a nextEventTime. Each time the SimClock
// reaches the nextEventTime, it will call handleEvent(). If handleEvent returns false,
// the SimClock will remove this object from the simulation, but there should always be
// one PlaneQueue object in the SimClock. If there are no planes in the queue we will
// set nextEventTime to LONG_MAX so we are never asked to process events (until
// something changes).
bool PlaneQueue::handleEvent(long currentTime, bool closeOut) {
    // If the simulation is over, we are done. We currently do not log or process anyting that was in the
    // vector waiting when the simulation ends.
    if(closeOut) {
        return false;
    }
    if(planesWaiting.empty()) {
        // Detect an error that should not happen
        std::cout << "PlaneQueue::handleEvent should not be called with no planess in use" <<  std::endl;
        std::cout << "  Curent time: " << currentTime << std::endl;
        std::cout << "  Next Event time: " << nextEventTime << std::endl;
        std::cout << std::endl;
        return false;
    }
    while(!planesWaiting.empty() && lastPlane().nextFlightTime <= currentTime) {
        // While we have any plane that is ready to fly and has hit its assigned passenger wait time,
        // remove it from the wait queue and put it into a new flight.
        // First, get the next plane ready to go. Planes with equal nextFlightTime are stored so they
        // will be handled FIFO.
        std::shared_ptr<Plane> thePlane = lastPlane().thePlane;
        // Remove the plane from the queue because it will be handed to a Flight object
#if SORTED_PLANE_QUEUE_TYPE == 2
        auto endPtr = planesWaiting.end();
        endPtr--;
        planesWaiting.erase(endPtr);
#else
        planesWaiting.pop_back();
#endif
        if(theSimulation && theSimulation->theSimClock) {
            // Create a flight object containing the plane and add it to theSimClock
            theSimulation->theSimClock->addHandler(std::make_shared<Flight>(theSimulation,
                                                                            currentTime, Passenger::getPassengerCount(thePlane->getCompany(),theSimulation),thePlane));
        } else if(verboseTesting) {
            // If testing and being verbose, explain what we would have done if part of an actual simulation.
            std::cout << "Would add flight for " << thePlane->describe() << "to SimClock if full simulation" << std::endl;
        }
    }
    if(planesWaiting.empty()) {
        // No planes waiting now so keep us in SimClock, but do not pass events to us until something changes
        nextEventTime = LONG_MAX;
    } else  {
        // Update our next time to the plane at the back of the vector (the next one that will be ready)
        nextEventTime = lastPlane().nextFlightTime;
    }
    return true;
}

// For testing: total how may planes are in the wait queue
long PlaneQueue::countPlanes() {
    return static_cast<long>(planesWaiting.size());
}

// For testing: describe the object and the count od planes in the object
const std::string PlaneQueue::describe() {
#if 0 // for extra testing
    verboseTesting = true;
    describeQueue((theSimulation && theSimulation->theSimClock) ? theSimulation->theSimClock->getTime() : 0);
#endif
    std::string description = "Plane Queue containing " + std::to_string(countPlanes()) + " planes";
    return description;
}


// Is vector empty?
bool PlaneQueue::isEmpty() {
    return planesWaiting.empty();
}

// Insert a plane into the proper position in the waiting vector so the plane ready
// soonest is at the back wit equal ready times being FIFO.
// After adding the plane during a handleEvent cycle, if it is immediatley ready
// the handleEvent function will notice that and move it to a Flight. If outside
// our own handleEvent function then it would be put into a flight when it is next called.
// If we are called from the theChargerQueue eventHandler, optimize by not trying to resort
// theChargerQueue in the event queue because it will be reinserted into the right place
// after the eventHandler completes.

void PlaneQueue::addPlane(long delayUntil, std::shared_ptr<Plane> aPlane, bool fromEventHandler) {
    if(verboseTesting) {
        std::cout << "Adding " << aPlane->describe() << " with delay until " << delayUntil << std::endl;
    }
    // Keep planes sorted with soonest ready time at the end.
# if SORTED_PLANE_QUEUE_TYPE == 2 // multiset always already sorted
    PlaneQueueItem queueItem = PlaneQueueItem{aPlane, delayUntil};
    planesWaiting.insert(queueItem);
#else
    auto planePtr = begin(planesWaiting);
    while(planePtr != end(planesWaiting) &&  planePtr->nextFlightTime > delayUntil) {
        planePtr++;
    }
    PlaneQueueItem queueItem = PlaneQueueItem{aPlane, delayUntil};
    planesWaiting.insert(planePtr, queueItem);
#endif
    // if our nextFlightTime time has changed, we need to be resorted in the SimClock
    // If we are from PlaneQueue.eventHandler, we will be resorted after the handler so no need to do it now
    if(nextEventTime > queueItem.nextFlightTime) {
        nextEventTime = queueItem.nextFlightTime;
        if(!fromEventHandler && theSimulation && theSimulation->theSimClock && theSimulation->thePlaneQueue) {
                theSimulation->theSimClock->reSortHandler(theSimulation->thePlaneQueue);
        }
    }

}

// Generate "count" planes semi-randomly. Make sure at least "minOfEachKind" are generated
// for each kind. This algorithm can only work correctly if the options meet this condition:
//      count >= minOfEachKind * numberOfKinds.
void PlaneQueue::generatePlanes(long currentTime, long count, long minOfEachCompany, long maxPassengerDelay) {
    
    // Set up an array of how many minimum are needed of each kind
    long neededOfCompany[companyCount]{};
    // Put the company id into the array for later use
    for(long i = 0; i < companyCount; i++) {
        neededOfCompany[i] = minOfEachCompany;
    }
    // Calculate how many planes need to be allocated before we have the minimum covered
    long totalCompanyStillNeeded{minOfEachCompany *companyCount};
    
    // Allocate planes to random Companies, with constraints
    std::vector<Company> companyChoices(count);
    for(long planesAllocated = 0; planesAllocated < count; planesAllocated++) {
        // Select a random company.
        long thisCompany = theRandomGenerators ? theRandomGenerators->getRandomCompany() : rand() % companyCount;
        // As long as we need some minimums, make sure we do those first
        // Otherwise just go with the intial random choice
        if(totalCompanyStillNeeded > 0) {
            long originalCompany = thisCompany; // save it to test for infinite loop
            // Now adjust the selection until we find a Companythat still needs planes
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
        long waitForPassengers = Passenger::getPassengerDelay(maxPassengerDelay, theSimulation);
        // actually add the random plane
        addPlane(waitForPassengers, std::make_shared<Plane>(planeSpecifications[thisChoice],
                                                            theSimulation ? theSimulation->theRandomGenerators : nullptr), false);
    }
#if !(SORTED_PLANE_QUEUE_TYPE == 2)
    // Make sure the plane list is sorted
    if(!is_sorted(begin(planesWaiting), end (planesWaiting), [](PlaneQueueItem lhs, PlaneQueueItem rhs) {
        return lhs.nextFlightTime > rhs.nextFlightTime;
    })) {
        std::cout << "***error: Plane queue is not sorted after adding planes." << std::endl;
    }
#endif
}

// For testing: remove the next Plane ready from the vector independent of timing
// and return it to the caller.
std::shared_ptr<Plane> PlaneQueue::removeNextPlane() {
    if(planesWaiting.empty()) {
        // no planes to remove
        return nullptr;
    }
#if SORTED_PLANE_QUEUE_TYPE == 2
    auto endPtr = planesWaiting.end();
    endPtr--;
    std::shared_ptr<Plane> returnValue = endPtr->thePlane;
    planesWaiting.erase(endPtr);
#else
    // Get a pointer to the plane and remove it from this object
    std::shared_ptr<Plane> returnValue = planesWaiting.back().thePlane;
    planesWaiting.pop_back();

    // if our earliest nextFlightTime time has changed, we need to be resorted in the SimClock
    if(nextEventTime != planesWaiting.back().nextFlightTime) {
        nextEventTime = planesWaiting.back().nextFlightTime; // adjust the next time for our queue
        if(theSimulation && theSimulation->theSimClock && theSimulation->thePlaneQueue) {
            theSimulation->theSimClock->reSortHandler(theSimulation->thePlaneQueue);
        }
    }
#endif
    return returnValue;
}

// For testing: ask some actions to be sent to cout
void PlaneQueue::setVerboseTesting(bool newValue) {
    verboseTesting = newValue;
}

// For testing: describe detailed current state of the vector.
// This is more detailed than the override of the generic describe method
// While describe() gives counts of planes, this lists them.
void PlaneQueue::describeQueue(long currentTime) {
    std::cout << "*** PlaneQueue Status" << std::endl;
    std::cout << "Current Time: " << currentTime << std::endl;
    // List the planes waitint
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

// Get status information in a vector so automated testing can check things
std::vector<PlaneQueueStatusItem> PlaneQueue::getQueueStatus() {
    // Set up a result vector
    std::vector<PlaneQueueStatusItem> result{};
    // Push information on current waiting planes into the vector
   for(auto aPlaneQueueItem: planesWaiting) {
        PlaneQueueStatusItem anItem{aPlaneQueueItem.thePlane->getPlaneNumber(), aPlaneQueueItem.thePlane->getCompany(), aPlaneQueueItem.nextFlightTime};
        result.push_back(anItem);
    }
    return result;

}

// Test the way this class behaves with non-zero passenger wait times
bool testPlaneQueueForWaits() {
    const int testPlanes{5};
    const long ofEachKind{1};
    const long maxWait{20};

    bool returnValue = true;
    long currentTime = 0;
    std::shared_ptr<RandomGenerators> theRandomGenerators = std::make_shared<RandomGenerators>(nullptr);
    PlaneQueue aQueue(nullptr, theRandomGenerators);
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
bool testPlaneQueueForMinimumPerKind() {
    const long testPlanes{10};
    const long ofEachKindArray[]{0, 1, 2};
    const long repeatGenerations{5}; // How many times to try each option

    bool returnValue = true;
    long errorCount = 0;
    long currentTime = 0;
    std::cout << "***** Starting test of PlaneQueue Plane Allocation *****" << std::endl;
    
    for(auto ofEach: ofEachKindArray) {
        std::cout << "With the minimum of each kind == " << ofEach << ":" << std::endl;
        for(auto repetition = 0; repetition < repeatGenerations; repetition++) {
            // Set up a PlaneQueue and populate it with planes
            std::shared_ptr<RandomGenerators> theRandomGenerators = std::make_shared<RandomGenerators>(nullptr);
            PlaneQueue aQueue(nullptr, theRandomGenerators);
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
                // If there are not enough of a kind, generate an error message and increment the errorCount
                if(kindCounts[kind] < ofEach) {
                    std::cout << "***** error: " << kindCounts[kind] << " planes is < the required " << ofEach << std::endl;
                    errorCount++;
                }
            }
        }
        std::cout << std::endl;
    }
    std::cout << "Completed test of PlaneQueue Plane Allocation" << std::endl;
    if(errorCount == 0) {
        std::cout << "No errors detected" << std::endl;
    } else {
        std::cout << "***** " << errorCount << " errors detected" << std::endl;
    }
    std::cout << std::endl;
    return returnValue;
}
