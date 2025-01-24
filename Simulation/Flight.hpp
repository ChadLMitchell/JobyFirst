//
//  Flight.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//

#ifndef Flight_hpp
#define Flight_hpp

#include <stdio.h>
#include "SimClock.hpp"
#include "Plane.hpp"

class Flight: public EventHandler {
    long startTime;
    long endTime;
    long nextFaultTime;
    long passengerCount;
    long faultCount;
    std::shared_ptr<Plane> thePlane;
    Simulation *theSimulation;
public:
    Flight(Simulation *theSimulation, long startTime, long passengerCount, std::shared_ptr<Plane> aPlane);
    virtual ~Flight() override;
    virtual bool handleEvent(long currentTime, bool closeOut) override;
    virtual long countPlanes() override;
    virtual const std::string describe() override;
    void recordFlight();
};

#endif /* Flight_hpp */
