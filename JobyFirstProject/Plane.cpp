//
//  Plane.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/18/25.
//

#include <random>
#include "Plane.hpp"

const char *companyNames[]{
    "Alpha Company",
    "Bravo Company",
    "Charlie Company",
    "Delta Company",
    "Echo Company"
};
// Show company name with error checking
const char *companyName(Company c) {
    if(c < minCompany || c > maxCompany) {
        return "Invalid Company Name";
    }
    return companyNames[c];
}

Specification specifications[]{
//    Company theCompany;
//    double cruise_speed__mph;
//    double battery_capacity__kWh;
//    double time_to_charge__hours;
//    double energy_use__kWh_per_mile;
//    long passenger_count;
//    double probability_fault__per_hour;
    Specification(Company::Alpha, 120, 320, 0.6, 1.6, 4, 0.25),
    Specification(Company::Bravo, 100, 100, 0.2, 1.5, 5, 0.1),
    Specification(Company::Charlie, 160, 220, 0.8, 2.2, 3, 0.05),
    Specification(Company::Delta, 90, 120, 0.62, 0.8, 2, 0.22),
    Specification(Company::Echo, 30, 150, 0.3, 5.8, 2, 0.61)
};

bool validateSpecs(Specification &spec) {
    bool returnValue{true};
    if(spec.cruise_speed__mph <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.battery_capacity__kWh <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.time_to_charge__hours <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.energy_use__kWh_per_mile <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.passenger_count <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.energy_use__kWh_per_mile <= 0) {
        returnValue = false;
        // log error
    }
    return returnValue;
}

int Plane::NextPlaneNumber = 1;
Plane::Plane(Specification &spec): mySpecs(spec) {
    // Avoid divide by 0 and other silly errors
    // We should validate specs before creating Plane, this is extra checking
    if(!validateSpecs(mySpecs)) {
        throw std::runtime_error("Attempt to create plane with invalid specifications");
    }
    planeNumber = NextPlaneNumber++;
    createFaultInterval();
}
Plane::~Plane() {
}
Company Plane::getCompany(){
    return mySpecs.theCompany;
}
const char *Plane::getCompanyName(){
    return companyName(mySpecs.theCompany);
}
int Plane::getPlaneNumber() {
    return planeNumber;
}
const std::string Plane::describe(){
    std::string description = "plane #" + std::to_string(planeNumber) + " from " + getCompanyName();
    return description;
}
long Plane::calcTimeToCharge__seconds() {
    return mySpecs.time_to_charge__hours * 60 * 60;
}
double Plane::calcDistanceFullCharge__miles() {
    return mySpecs.battery_capacity__kWh / mySpecs.energy_use__kWh_per_mile;
}
long Plane::calcTimeOnFullCharge__seconds() {
    return lround(calcDistanceFullCharge__miles()*360 / mySpecs.cruise_speed__mph);
}
double Plane::calcMeanTimeBetweenFaults() {
    return 360/mySpecs.probability_fault__per_hour;
}
long Plane::getNextFaultInterval() {
    return nextFaultInterval;
}
long Plane::decrementNextFaultLinterval(long seconds) {
    nextFaultInterval -= seconds;
    return seconds;
}
long Plane::createFaultInterval() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0, 1);
    double random0to1 = distrib(gen);
    if (random0to1 == 0) { random0to1 = 0.1; }; // avoid log 0
    if (random0to1 == 1) { random0to1 = 0.9; }; // avoid log 1
    nextFaultInterval = -std::log(random0to1) * 360 / mySpecs.probability_fault__per_hour;
    return nextFaultInterval;
}
long Plane::calcPassengerCount() {
    return mySpecs.passenger_count;
    // NEED TO RESPOND TO SETTINGS TO DO *******************************************
}
std::shared_ptr<Plane> Plane::getRandomPlane() {
    return std::make_shared<Plane>(specifications[0]);
}

const double allowedPercentDiffFromMTBF{3.0};

bool testPlane(bool verbose) {
    bool returnValue = true;
    const double tries = 10000;
    if(verbose) {
        std::cout << "Testing Plane Class" << std::endl;
        std::cout << "Validating specificastions and simulation of time to next fault" << std::endl;
    }
    for(Specification aSpec: specifications) {
        if(!validateSpecs(aSpec)) {
            if(verbose) {
                std::cout << companyName(aSpec.theCompany) << " has invalid specification" << std::endl;
            }
            returnValue = false;
        } else {
            double accumFaultCount = 0;
            Plane testPlane {Plane(aSpec)};
            for (auto tryCount = 0; tryCount < tries; tryCount++) {
                accumFaultCount += testPlane.createFaultInterval();
            }
            double averageFaultInterval = accumFaultCount/tries;
            if(verbose) {
                std::cout  << companyName(aSpec.theCompany) << " MTBF from Spec: " << testPlane.calcMeanTimeBetweenFaults() << " Average Simulated Fault Interval (seconds): " << averageFaultInterval << std::endl;
            }
            double percentDiff = abs(testPlane.calcMeanTimeBetweenFaults() - averageFaultInterval)/testPlane.calcMeanTimeBetweenFaults() * 100;
            if(percentDiff > allowedPercentDiffFromMTBF) {
                if(verbose) {
                    std::cout << "Error: " << companyName(aSpec.theCompany) << " Difference between Simulated Average Interval and MTBF: " << percentDiff << "% > " << allowedPercentDiffFromMTBF << "% (allowed difference)" << std::endl;
                }
                returnValue = false;
            }
        }
    }
    return returnValue;
}
