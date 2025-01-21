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
public:
    Flight(long startTime, long passengerCount, std::shared_ptr<Plane> aPlane);
    virtual ~Flight();
    virtual bool handleEvent(long currentTime);
    void recordFlight();
};

struct FlightStats {
    Company theCompany;
    long duration;
    long passengerCount;
    long faultCount;
};
extern std::vector<FlightStats> completedFlights;

#endif /* Flight_hpp */
