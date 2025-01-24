//
//  PlaneQueue.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#ifndef PlaneQueue_hpp
#define PlaneQueue_hpp

#include <stdio.h>
#include "SimClock.hpp"
#include "Plane.hpp"

/*
 *******************************************************************************************
 * Struct PlaneQueueItem
 * This is a helper struct. The plane queue contains a vector of planes, but we also need
 * to know when they should be assigned to a flight.
 *******************************************************************************************
 */
struct PlaneQueueItem {
    std::shared_ptr<Plane> thePlane;
    long nextFlightTime;
};
/*
 *******************************************************************************************
 * Struct PlaneQueueStatusItem
 * This is a helper struct for PlaneQueue. We use this for testing to return a vector of
 * the times and Plane numbers currently in this object.
 *******************************************************************************************
 */
struct PlaneQueueStatusItem {
    int planeNumber;
    long nextFlightTime;
};
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
class PlaneQueue: public EventHandler {
    Simulation *theSimulation; // Simulation object containing current simulation or nullptr
    bool verboseTesting; // Option for testing to have the class output action descriptions
    // The following is a vector, not a queue because it is kept soonest at tne end and
    // we can add a Plane to it that has a our of order passengerDelay
    std::vector<PlaneQueueItem> planesWaiting;
public:
    PlaneQueue(Simulation *theSimulation);
    virtual ~PlaneQueue()override;

    // As a child of EventHandler, PlaneQueue has a nextEventTime. Each time the SimClock
    // reaches the nextEventTime, it will call handleEvent(). If handleEvent returns false,
    // the SimClock will remove this object from the simulation, but there should always be
    // one PlaneQueue object in the SimClock. If there are no planes
    // in the queue we will set nextEventTime to LONG_MAX so we are never asked to do an
    // event (until something changes).
    virtual bool handleEvent(long currentTime, bool closeOut) override;

    // For testing: count how may planes are in this "queue"
    virtual long countPlanes() override;

    // For testing: describe this object in the clock handler queue
    virtual const std::string describe() override;

    // Is the vector of Plane objects empty?
    bool isEmpty();

    // Add a plane to the vector, sorting it according to the delayUntil time.
    // When the SimClock currentTime >= delayUntil the next call to handleEvent will put
    // the plane into a filght and put that into the SimClock.
    void addPlane(long delayUntil, std::shared_ptr<Plane> aPlane);

    // Fill the vector with planes with possible delays according to the settings.
    // In a simulation, these will then be put into flights as their time arrives,
    // which will be immediately if the option is no passenger delays.
    // If count >= minOfEachCompany and count >= "the number of plane Companys" there will
    // at least be minOfEachCompany planes of each Company.
    void generatePlanes(long currentTime, long count, long minOfEachCompany);
 
    // For testing: remove the next Plane from the vector independent of timing
    // and return it to the caller.
    std::shared_ptr<Plane> removeNextPlane();

    // For testing: ask some actions to be sent to cout
    void setVerboseTesting(bool newValue);

    // For testing: verbose description of the current state of this object
    void describeQueue(long currentTime);

    // For testing: provide a list of the planes in the queue and thier timing.
    std::vector<PlaneQueueStatusItem> getQueueStatus();
};

// Test the class with verbose output that can be manually reviewed
bool testPlaneQueueLong();

// Test the class with minimal output, eventully TO DO would do automated test evaluation
bool testPlaneQueueShort();

#endif /* PlaneQueue_hpp */
