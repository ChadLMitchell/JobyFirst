//
//  ChargerQueue.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#include "ChargerQueue.hpp"
#include "Plane.hpp"
#include "Flight.hpp"
#include "PlaneQueue.hpp"
#include "Passenger.hpp"

/*
 *******************************************************************************************
 * class ChargerQueue
 * This manages a queue of planes waiting for chargers.
 * It also manages a vector of chargers assigned to planes.
 * After planes fly, they move to this class to recharge unless they are grounded.
 *
 * This is a child of eventHandler. As such, it will be added to the SimClock queue.
 *
 * A Simulation object will contain exactly one ChargerQueue object.
 * If the ChargerQueue object is in a Simulation it has a pointer to that Simulation object.
 * For testing, a ChargerQueue may have a nullptr for theSimulation property.
 *******************************************************************************************
 */
ChargerQueue::ChargerQueue(Simulation *theSimulation, long chargerCount):
// Set our nextEventTime initially to LONG_MAX so we can be in the SimClock handler list, but
// not receive events until something happens to activate us such as adding planes that
// need to be charged.
EventHandler(LONG_MAX), theSimulation{theSimulation}, chargerCount{chargerCount}, verboseTesting{false},chargers{}, planesWaiting{} {
}
ChargerQueue::~ChargerQueue() {
}

// As a child of EventHandler, ChargerQueue has a nextEventTime. Each time the SimClock
// reaches the nextEventTime, it will call handleEvent(). If handleEvent returns false,
// the SimClock will remove this object from the simulation, but there should always be
// one ChargerQueue object in the SimClock. If there are no planes in the queue nor
// active chargers, we will set nextEventTime to LONG_MAX so we are never asked to
// process events (until something changes).
bool ChargerQueue::handleEvent(long currentTime, bool closeOut) {
    if(closeOut) {
        // If we are closing out the simualtion, mark all chargers as done
        for(auto aCharger: chargers) {
            aCharger.timeDone = currentTime;
            if(theSimulation) {
                // if we are in a simulation, log this charge
                ChargerStats someStats{aCharger.thePlane->getCompany(),
                    aCharger.thePlane->getPlaneNumber(),
                    currentTime - aCharger.timeStarted, currentTime - aCharger.timeStartedIncludingWait};
                theSimulation->theChargerStats.push_back(someStats);
            }
        }
        // The simulation is over so we do not need to keep "this" object in the Event queue
        return false;
    }
    if(chargers.empty()) {
        // Detect an error that should not happen
        std::cout << "ChargerQueue::handleEvent should not be called with no chargers in use" <<  std::endl;
        return false;
    }
    // Handle any planes that are now fully charged
    while(!chargers.empty() && chargers.back().timeDone <= currentTime) {
        // We keep the chargers sorted with the first to be done at the back of the vector
        // If the charger that will be done next is ready, remove the plane from the charger
        std::shared_ptr<Plane> thePlane = chargers.back().thePlane;
        if(theSimulation) {
            // log this charge as completed
            Charger aCharger = chargers.back();
            ChargerStats someStats{aCharger.thePlane->getCompany(),
                aCharger.thePlane->getPlaneNumber(),
                currentTime - aCharger.timeStarted, currentTime - aCharger.timeStartedIncludingWait};
            theSimulation->theChargerStats.push_back(someStats);
        }

        // Remove the object charger from the vector. The object will be discarded, but we already saved a pointer to the plane.
        chargers.pop_back();
        if(theSimulation && theSimulation->thePlaneQueue) {
            // If we are in a simulation get the passenger delay setting, otherwise default to 0.
            long maxPassengerDelay = 0;
            if(theSimulation->theSettings) { maxPassengerDelay = theSimulation->theSettings->maxPassengerDelay; }
            // Add the plane to the plane queue, ready to fly, asking a static Passenger class function to assign an actual
            // delay for this plane at this time. If maxPassengerDelay > 0 it will be set to some value in [0 - maxPassengerDelay].
            theSimulation->thePlaneQueue->addPlane(currentTime + Passenger::getPassengerDelay(maxPassengerDelay),thePlane);
        } else if(verboseTesting) {
            // if we are not in a simulation and are being verbose, mention what we would have done if we could
            std::cout << "Would add flight for " << thePlane->describe() << "to thePlaneQueue if Sim full simulation" << std::endl;
        }
    }
    // If there are chargers available, move planes from waiting queue to charger vector
    while(!planesWaiting.empty() && chargers.size() < chargerCount) {
        // Remove a plane from the waiting queue, alwasy manages FIFO
        WaitingPlane aWaitingPlane = planesWaiting.front();
        planesWaiting.pop();
        // Create a charger option and add the plane to the vector of chargers being used
        addCharger(currentTime, aWaitingPlane.timeStarted, aWaitingPlane.thePlane);
    }
    if(chargers.empty()) {
        // If there are chargers in use (possible if all planes are in flight or waiting for passengers) then set our
        // nextEventTime to a value that assures our handleEvent will not be called until something changes elsewhere.
        nextEventTime = LONG_MAX;
    } else  {
        // Otherwise set our next time to the last charger in the queue (the first to be done)
        nextEventTime = chargers.back().timeDone;
    }
    // When we return true, the SimClock event handlers (which already removed us from it's vector of handlers) will insert us back in at
    // the right place keeping all the handlers sosrted.
    return true;
}

// For testing: total how may planes are in the wait queue or a charger
long ChargerQueue::countPlanes() {
    return static_cast<long>(chargers.size() + planesWaiting.size());
}

// For testing: describe the object and counts of the queue and vector
const std::string ChargerQueue::describe() {
    std::string description = "Charger Queue with " + std::to_string(chargers.size()) + " planes on chargers and " + std::to_string(planesWaiting.size()) + " planes waiting";
    return description;
}

// Are the vector and chargers both empty?
bool ChargerQueue::isEmpty() {
    return planesWaiting.empty() && chargers.empty();
}

// If a charger is available, assign the plane, immediately. Otherwise add it to the wait
// queue which is managed FIFO.
// When a charger is done, the plane will be moved back to the PlaneQueue. From there
// the plane will be put into a flight as soon as it has passengers available.
void ChargerQueue::addPlane(long currentTime, std::shared_ptr<Plane> aPlane) {
    if(chargers.size() >= chargerCount) {
        WaitingPlane aWaitingPlane = WaitingPlane{currentTime, aPlane};
        planesWaiting.push(aWaitingPlane);
    } else {
        addCharger(currentTime, currentTime, aPlane);
    }
}
void ChargerQueue::addCharger(long currentTime, long startedWaiting, std::shared_ptr<Plane> aPlane) {
    if(chargers.size() >= chargerCount) {
        // Catch an error. This should never be called if all chargers are full.
        std::cout <<"error: trying to add too many chargers " << std::endl;
        return;
    } else {
        // Keep chargers sorted with soonest time at the end.
        // First calculate when this plane will be charged.
        long timeToCharged = currentTime + aPlane->calcTimeToCharge__seconds();
        // Insert the new charger into the first location that will keep it sorted.
        auto chargerPtr = begin(chargers);
        while(chargerPtr != end(chargers) &&  chargerPtr->timeDone > timeToCharged) {
            chargerPtr++;
        }
        // Create a charger object for the plane to keep track of starting and ending time of the charge
        Charger aCharger = Charger{currentTime, startedWaiting, timeToCharged, aPlane};
        // Insert the charger into the righ tplace in the chargers vector
        chargers.insert(chargerPtr, aCharger);
        
        if(nextEventTime != chargers.back().timeDone) {
            // If our earliest charger done time has changed, we need to be resorted in the SimClock
            // This could happen when the first charger is put into use, but also if a plane is put
            // on a charger that charges so much faster than other planes charging that it will be
            // done first.
            nextEventTime = chargers.back().timeDone; // adjust the next time for our queue
            // Ask the theSimClock to re-sort this in it's queue (sorted vector).
            if(theSimulation && theSimulation->theSimClock && theSimulation->theChargerQueue) {
                theSimulation->theSimClock->reSortHandler(theSimulation->theChargerQueue);
            }
        }
    }
}
// Indicate that we are testing and want information in cout about ongoing actions
void ChargerQueue::setVerboseTesting(bool newValue) {
    verboseTesting = newValue;
}

// For testing: describe detailed current state of the queues
// This is more detailed than the override of the generic describe method
// While describe() gives counts of planes, this lists them.
void ChargerQueue::describeQueues(long currentTime) {
    std::cout << "<<< ChargerQueue Status >>>" << std::endl;
    std::cout << "Current Time: " << currentTime << std::endl;
    // List the planes on chargers
    if(chargers.size() == 0) {
        std::cout << "No planes on a charger" << std::endl;
   } else {
        std::cout << "Planes on Chargers" << std::endl;
        for(auto aCharger: chargers) {
            std::cout << "    " << aCharger.thePlane->describe() << " done at " << aCharger.timeDone << std::endl;
        }
    }
    // List the planes waiting for chargers
    if(planesWaiting.empty()) {
        std::cout << "No planes waiting for a Charger" << std::endl;
   } else {
        std::cout << "Planes waiting for a Charger" << std::endl;
        // To view a queue we must pull everything off of it (and then put it all back)
        // This is not at all effecient, but we only do it for testing
        // Regular usage operates with normal queue functionality
        std::vector<WaitingPlane> tempPlanes{};
        // pull planes off planesWaiting, describe them and save them in tempPlanes
        while(!planesWaiting.empty()) {
            WaitingPlane aWaitingPlane = planesWaiting.front();
            planesWaiting.pop();
            std::cout << "    " << "Waiting " << currentTime - aWaitingPlane.timeStarted
            << (secondsPerMinute == 60 ? " seconds: " : " minutes: ") << aWaitingPlane.thePlane->describe() << std::endl;
            tempPlanes.push_back(aWaitingPlane);
        }
        // restore planes to tempQueue
        for(auto planeToRestore: tempPlanes) {
            planesWaiting.push(planeToRestore);
        }
    }
    std::cout << std::endl;
}
// Get status information in a vector so automated testing can check things
std::vector<ChargerQueueStatusItem> ChargerQueue::getQueueStatus() {
    // Set up a result vector
    std::vector<ChargerQueueStatusItem> result{};
    // Push information on current chargers into the vector
    for(auto aCharger: chargers) {
        ChargerQueueStatusItem anItem{true,aCharger.thePlane->getPlaneNumber()};
        result.push_back(anItem);
    }
    // Push information on current waiting planes into the vector

    // To view a queue we must pull everything off of it (and then put it all back)
    // This is not at all effecient, but we only do it for testing
    // Regular usage operates with normal queue functionality
    std::vector<WaitingPlane> tempPlanes{};
    // pull planes off planesWaiting, describe them and save them in tempPlanes
    while(!planesWaiting.empty()) {
        WaitingPlane aWaitingPlane = planesWaiting.front();
        planesWaiting.pop();
        ChargerQueueStatusItem anItem{false,aWaitingPlane.thePlane->getPlaneNumber()};
        result.push_back(anItem);
        tempPlanes.push_back(aWaitingPlane);
    }
    // restore planes to tempQueue
    for(auto planeToRestore: tempPlanes) {
        planesWaiting.push(planeToRestore);
    }
    return result;
}

// This does a longer test of the ChargerQueue class outputing verbose information
// as it "simulates" being part of a simulation.
bool testChargerQueueLong() {
    const int testChargers{3};
    const int testPlanes{5};

    bool returnValue = true;
    long currentTime = 0;
    
    // Create a ChargerQueue object
    ChargerQueue aQueue(nullptr, testChargers);
    std::cout << " ***** Starting long test of ChargerQueue Class  *****" << std::endl;
    // Add some planes to the object
    for(auto i=0; i<testPlanes; i++){
        std::shared_ptr<Plane> aPlane = Plane::getRandomPlane();
        std::cout << "Adding " << aPlane->describe() << std::endl;
        aQueue.addPlane(currentTime, aPlane);
    }
    // List the current current planes in the object
    aQueue.describeQueues(currentTime);
    while(!aQueue.isEmpty()) {
        // Process events for the queue until all planes are processed
        currentTime = aQueue.getNextEventTime();
        std::cout << "After handling event at time " << currentTime << std::endl;
        aQueue.handleEvent(currentTime, false);
        // After each clock "tick", output the current planes in the object
        aQueue.describeQueues(currentTime);
    }
    std::cout << "Long test of ChargerQueue Class completed" << std::endl;
    std::cout << std::endl;
    return returnValue;
}

// This does a shorter test of the ChargerQueue class. It collects informataion about the status and actions of the object into an vector
// as it "simulates" being part of a simulation as it "simulates" being part of a simulation.
// For now this then outputs to cout a summary of the results, but later we could do more automated
// testing using expected values of the result vector for known inputs.
bool testChargerQueueShort() {
    const int testChargers{3};
    const int testPlanes{5};

    bool returnValue = true;
    long currentTime = 0;
    // Create a ChargerQueue object
    ChargerQueue aQueue(nullptr, testChargers);
    aQueue.setVerboseTesting(true);
    std::cout << " ***** Starting short test of ChargerQueue Class  *****" << std::endl;
    // Add some planes to the object
    for(auto i=0; i<testPlanes; i++){
        std::shared_ptr<Plane> aPlane = Plane::getRandomPlane();
        aQueue.addPlane(currentTime, aPlane);
    }

    // Get the current status of the plans in a vector
    std::vector<ChargerQueueStatusItem> queueStatus = aQueue.getQueueStatus();
    bool doingOnChargers = true;
    
    // At this point we could do automated comparisons to expected values and even do events and check again
    // For now we will just list the planes in the object
    std::cout << "Planes on chargers:" << std::endl;
    for(auto statusItem: queueStatus) {
        // TO-DO Instead of printing, we could feed in a specific set of planes and then confirm that the results are as expected.
        // That would be more of an automated test like some of the others. We could also do some time jumps and confirm those results.
        if(doingOnChargers && !statusItem.onCharger) {
            std::cout << "Planes waiting for chargers:" << std::endl;
            doingOnChargers = false;
        }
        std::cout << "    Plane #" << statusItem.planeNum << std::endl;

    }
    std::cout << "Short test of ChargerQueue Class completed" << std::endl;
    std::cout << std::endl;
    return returnValue;
}
