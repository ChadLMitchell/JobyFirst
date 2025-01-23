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
public:
    ChargerQueue(Simulation *theSimulation, long chargerCount);
    virtual ~ChargerQueue() override;
    virtual bool handleEvent(long currentTime) override;
    virtual long countPlanes() override;
    bool isEmpty();
    void addPlane(long currentTime, std::shared_ptr<Plane> aPlane);
    void setVerboseTesting(bool newValue);
    void describeQueues(long currentTime);
    std::vector<ChargerQueueStatusItem> getQueueStatus();
};

bool testChargerQueueLong();
bool testChargerQueueShort();

#endif /* ChargerQueue_hpp */
