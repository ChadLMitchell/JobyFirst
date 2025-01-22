//
//  ChargerQueue.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef ChargerQueue_hpp
#define ChargerQueue_hpp

#include <stdio.h>
#include "SimClock.hpp"
#include "Plane.hpp"

struct Charger {
    long timeDone;
    std::shared_ptr<Plane> thePlane;
};

struct ChargerQueueStatusItem { // for testing
    bool onCharger;
    int planeNum;
};
class ChargerQueue: public EventHandler {
    Simulation *theSimulation;
    long chargerCount;
    bool verboseTesting;
    std::vector<Charger> chargers;
    std::queue<std::shared_ptr<Plane>> planesWaiting;
        // TO DO: consider using a list for performance later
public:
    ChargerQueue(Simulation *theSimulation, long chargerCount);
    virtual ~ChargerQueue();
    virtual bool handleEvent(long currentTime);
    void addPlane(long currentTime, std::shared_ptr<Plane> aPlane);
    void describeQueues(long currentTime);
    bool isEmpty();
    std::vector<ChargerQueueStatusItem> getQueueStatus();
};

bool testChargerQueueLong();
bool testChargerQueueShort();

#endif /* ChargerQueue_hpp */
