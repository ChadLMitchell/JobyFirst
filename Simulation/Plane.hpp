//
//  Plane.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/18/25.
//

#ifndef Plane_hpp
#define Plane_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>

// enumeration of the possible company/kinds of planes
enum Company {
    Alpha = 0,
    Bravo,
    Charlie,
    Delta,
    Echo
};
// Allow iterating through the Company enumeration
const Company allCompany[] = {Alpha, Bravo, Charlie, Delta, Echo};
// Minimum and maximum elements of the Company enumeration
const Company minCompany{Alpha};
const Company maxCompany{Echo};

// Reference to an array of company names
extern const char *companyNames[];
// Global function that takes a company enumeration and returns a company name
const char *companyName(Company c);

// Specifications provided by the assigned task
/*
 *******************************************************************************************
 * Struct PlaneSpecification
 * This structure contains the specifications of a plane type provided for this exercise.
 * Plane.cpp includes an array of all of the provided specifications. One row from that
 * is copied into a plane when it is created guiding its behavior.
 *******************************************************************************************
 */
struct PlaneSpecification {
    Company theCompany;
    double cruise_speed__mph;
    double battery_capacity__kWh;
    double time_to_charge__hours;
    double energy_use__kWh_per_mile;
    long passenger_count;
    double probability_fault__per_hour;
};

/*
 *******************************************************************************************
 * class Plane
 * This represents a single plane. Ever plane is always in exactly one of three places:
 *     In thePlaneQueue waiting for passengers, ready to join a flight or being grounded.
 *     In theChargerQueue on a charger or waiting for a charger.
 *     In a Flight.
 * A plane may move between the three, but should only be in one place at a time.
 * The number of planes in the overall system should never decrease.
 *
 * Plane.hpp and Plane.cpp are also currently the repository of plane related information
 * data and structures including the planes' specifications and some data that helps us
 * work with that. Those may eventually be moved elsewhere, but work OK here for now.
 *******************************************************************************************
 */
class Plane {
    PlaneSpecification mySpecs;
    
    long nextFaultInterval; // Hoe much flight until the next fault?
                            // We assume fault timing happens during flight only
                            // If we complete a flight without triggering the current
                            // fault we decrement this inteval by the duration of the
                            // flight passed without a fault.
    int planeNumber; // Each plane is assigned a plane number (mostly for testing)
    static int NextPlaneNumber; // Keep track of next number to assign
public:
    Plane(PlaneSpecification &spec);
    ~Plane();

    // For testing: get the Company enum assigned to this plane
   Company getCompany();

    // For testing: get the name of the company assigned
   const char *getCompanyName();

    // For testing: get the plane number
    int getPlaneNumber();

    // For testing: provide a description of this object
    const std::string describe();

    // Get from the specification
    long getMilesPerHour();

    // Calculate from the specification
    long calcTimeToCharge__seconds();

    // Calculate from the specification
    double calcDistanceFullCharge__miles();

    // Calculate from the specification
    long calcTimeOnFullCharge__seconds();

    // Calculate from the specification
    double calcMeanTimeBetweenFaults();

    // Get from the specification
    long getMaxPassengerCount();
    
    // Get the current interval of flight time to the next fault
    long getNextFaultInterval();

    // Reduce the fault interval remaining after a successful flight
    // The amount we we are asked to decrement represents the time
    // in the current flight applied to this fault Interval. Since
    // we may have had a previous fault during the flight, do not
    // decrement the full flight time, only the time since this
    // fault interval was set up. The Flight remembers that.
    long decrementNextFaultInterval(long seconds);

    // Use the MTBF to generate a random next fault interval
    long createFaultInterval();

    // Validate the plane specifications are (somewhat) valid
    static bool validateSpecs(PlaneSpecification &spec);

    // For testing: get a random plane for testing
    // Note that thePlaneQueue directly creates the Plane
    // Objects in a simualtion when it starts.
    static std::shared_ptr<Plane> getRandomPlane();
};

// Function to test some functionality of the Plane class
bool testPlaneClassObjects(bool verbose = true);

#endif /* Plane_hpp */
