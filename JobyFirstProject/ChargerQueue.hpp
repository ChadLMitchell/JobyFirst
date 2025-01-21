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

class ChargerQueue: public EventHandler {
    Simulation *theSimulation;
    long chargerCount;
    std::vector<Charger> chargers;
    std::queue<std::shared_ptr<Plane>> planesWaiting;
        // TO DO: consider using a list for performance later
public:
    ChargerQueue(Simulation *theSimulation, long chargerCount);
    virtual ~ChargerQueue();
    virtual bool handleEvent(long currentTime);
    void addPlane(long currentTime, std::shared_ptr<Plane> aPlane);
};

#endif /* ChargerQueue_hpp */
