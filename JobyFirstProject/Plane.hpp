//
//  Plane.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/18/25.
//

#ifndef Plane_hpp
#define Plane_hpp

#include <stdio.h>
#include <iostream>

enum class Company {
    Alpha = 0,
    Bravo,
    Charlie,
    Delta,
    Echo
};
extern const char *companyNames[];

// Specifications provided by the task
struct Specification {
    Company theCompany;
    double cruise_speed__mph;
    double battery_capacity__kWh;
    double time_to_charge__hours;
    double energy_use__kWh_per_mile;
    long passenger_count;
    double probability_fault__per_hour;
};

class Plane {
    Specification mySpecs;

    long nextFaultInterval; // seconds
    long faultCount;
    
public:
    Plane(Specification &spec);
    ~Plane();
    Company getCompany();
    double distanceFullCharge__miles(); // calculated value
    long timeFullCharge__seconds(); // calculated value
    double meanTimeBetweenFaults(); // calculated value
    long getNextFaultInterval();
    long decrementNextFaultLinterval(long seconds);
    long createFaultInterval();
    void recordFault();
    long getFaultCount();
};

bool testPlane(bool verbose = true);

#endif /* Plane_hpp */
