//
//  ChargerQueue.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef ChargerQueue_hpp
#define ChargerQueue_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include "SimClock.hpp"
#include "Plane.hpp"

/*
 *******************************************************************************************
 * Struct Charger
 * This is used to store information about active chargers.
 * In addition to the plane, we need the time charging is done.
 * And for statistics, we also need when it started so we can caluclate duration.
 *******************************************************************************************
 */
struct Charger {
    long timeStarted;
    long timeStartedIncludingWait;
    long timeDone;
    std::shared_ptr<Plane> thePlane;
};
/*
 *******************************************************************************************
 * Struct WaitingPlane
 * This is used to store information about planes waiting for chargers.
 * We need this information to calculate how long it waited.
 *******************************************************************************************
 */
struct WaitingPlane {
    long timeStarted;
    std::shared_ptr<Plane> thePlane;
};

/*
 *******************************************************************************************
 * Struct ChargerQueueStatusItem
 * This is used to return information about the state of the Charger Queue for testing.
 *******************************************************************************************
 */
struct ChargerQueueStatusItem { // for testing
    bool onCharger;
    int planeNum;
};
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
class ChargerQueue: public EventHandler {
    Simulation *theSimulation; // Simulation object containing current simulation or nullptr
    long chargerCount; // How many chargers in this simulation
    bool verboseTesting; // For testing, provides more details to cout
    std::vector<Charger> chargers; // Zero or more chargers (<= chargerCount)
    std::queue<WaitingPlane> planesWaiting; // Planes waiting for a charger
public:
    ChargerQueue(Simulation *theSimulation, long chargerCount);
    virtual ~ChargerQueue() override;
    
    // As a child of EventHandler, ChargerQueue has a nextEventTime. Each time the SimClock
    // reaches the nextEventTime, it will call handleEvent(). If handleEvent returns false,
    // the SimClock will remove this object from the simulation, but there should always be
    // one ChargerQueue object in the SimClock. If there are no planes in the queue nor
    // active chargers, we will set nextEventTime to LONG_MAX so we are never asked ton
    // process events (until something changes).
    virtual bool handleEvent(long currentTime, bool closeOut) override;
    
    // For testing: total how may planes are in the wait queue or a charger
    virtual long countPlanes() override;

    // For testing: describe the object and counts of the queue and vector
    virtual const std::string describe() override;

    // Are the vector and chargers both empty?
    bool isEmpty();

    // If a charger is available, assign the plane,
    // otherwise add it to the queue which is handled FIFO.
    // When a charger is done, the plane is put in a flight or if we have passenger delays,
    // When the SimClock currentTime >= delayUntil the next call to handleEvent will put
    // the plane back into the planeQueue until it can be put into a flight.
    void addPlane(long currentTime, std::shared_ptr<Plane> aPlane);
    
    // Add a plane to the charger. Should only be called if there are available chargers
    // Captures the time the plane has already started waited in the queue (if any).
    void addCharger(long currentTime, long startedWaiting, std::shared_ptr<Plane> aPlane);

    // Indicate that we are testing and want information in cout about ongoing actions
    void setVerboseTesting(bool newValue);
    
    // For testing: describe detailed current state of the queues.
    // This is more detailed than the override of the generic describe() method.
    // While describe() gives counts of planes, this lists them.
    void describeQueues(long currentTime);

    // Get status information in a vector so automated testing can check things
    std::vector<ChargerQueueStatusItem> getQueueStatus();
};

// These are here to allow the test menus access to call them.

// This does a longer test of the ChargerQueue class outputing verbose information
// as it "simulates" being part of a simulation.
bool testChargerQueueLong();

// This does a shorter test of the ChargerQueue class. It collects informataion about the status and actions of the object into an vector
// as it "simulates" being part of a simulation as it "simulates" being part of a simulation.
// For now this then outputs to cout a summary of the results, but later we could do more automated
// testing using expected values of the result vector for known inputs.
bool testChargerQueueShort();

#endif /* ChargerQueue_hpp */
