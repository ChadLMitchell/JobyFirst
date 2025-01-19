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

Plane::Plane(Specification &spec): mySpecs(spec) {
    createFaultInterval();
    faultCount = 0;
}
Plane::~Plane() {
}
Company Plane::getCompany(){
    return mySpecs.theCompany;
}
double Plane::distanceFullCharge__miles() {
    return mySpecs.battery_capacity__kWh / mySpecs.energy_use__kWh_per_mile;
}
long Plane::timeFullCharge__seconds() {
    return lround(distanceFullCharge__miles()*360 / mySpecs.cruise_speed__mph);
}
double Plane::meanTimeBetweenFaults() {
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
    if (random0to1 == 0) { random0to1 = 1; };
    nextFaultInterval = -std::log(random0to1) * 360 / mySpecs.probability_fault__per_hour;
    return nextFaultInterval;
}
void Plane::recordFault() {
    faultCount++;
}
long Plane::getFaultCount() {
    return faultCount;
}

bool testPlane(bool verbose) {
    const double tries = 10000;
    double accumFaultCount = 0;
    Plane testPlane {Plane(specifications[0])};
    for (auto tryCount = 0; tryCount < tries; tryCount++) {
        accumFaultCount += testPlane.createFaultInterval();
    }
    double averageFI = accumFaultCount/tries;
    if(verbose) {
        std::cout << "AverageFaultInterval (seconds): " << averageFI << " MTBF: " << testPlane.meanTimeBetweenFaults() << std::endl;
    }
    return true; // FIX THIS, also loop through all plane types
}
