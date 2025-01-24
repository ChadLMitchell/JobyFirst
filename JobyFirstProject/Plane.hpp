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

enum Company {
    Alpha = 0,
    Bravo,
    Charlie,
    Delta,
    Echo
};
// extern const Company allCompany[];
const Company allCompany[] = {Alpha, Bravo, Charlie, Delta, Echo};

const Company minCompany{Alpha};
const Company maxCompany{Echo};
extern const char *companyNames[];
const char *companyName(Company c);

// Specifications provided by the assigned task
struct PlaneSpecification {
    Company theCompany;
    double cruise_speed__mph;
    double battery_capacity__kWh;
    double time_to_charge__hours;
    double energy_use__kWh_per_mile;
    long passenger_count;
    double probability_fault__per_hour;
};

class Plane {
    PlaneSpecification mySpecs;
    
    long nextFaultInterval; // seconds
    static int NextPlaneNumber;
    int planeNumber;
public:
    Plane(PlaneSpecification &spec);
    ~Plane();
    Company getCompany();
    const char *getCompanyName();
    int getPlaneNumber();
    const std::string describe();
    long getMilesPerHour();
    long calcTimeToCharge__seconds();
    double calcDistanceFullCharge__miles(); // calculated value
    long calcTimeOnFullCharge__seconds(); // calculated value
    double calcMeanTimeBetweenFaults(); // calculated value
    long getMaxPassengerCount();
    
    long getNextFaultInterval();
    long decrementNextFaultInterval(long seconds);
    long createFaultInterval();

    static bool validateSpecs(PlaneSpecification &spec);

    static std::shared_ptr<Plane> getRandomPlane();
};

bool testPlaneClassObjects(bool verbose = true);

#endif /* Plane_hpp */
