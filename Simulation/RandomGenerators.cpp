//
//  RandomGenerators.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 2/13/25.
//
#include "RandomGenerators.hpp"
#include "SimSettings.hpp"
#include "PlaneQueue.hpp"

extern PlaneSpecification planeSpecifications[];

RandomGenerators::RandomGenerators(Simulation *theSimulation): theSimulation{theSimulation} {
    aGenerator = std::mt19937(useRandomSeed ? useRandomSeed : aRandomDevice());
    distribution0to1 = std::uniform_real_distribution<>(0,1);
    companyDistribution = std::uniform_int_distribution<>(0, companyCount - 1);
    delayDistribution = std::uniform_int_distribution<>(0, static_cast<int>((theSimulation && theSimulation->theSettings) ? theSimulation->theSettings->maxPassengerDelay : 0));
    for(int c=0; c<companyCount; c++) {
        passengerDistribution[c] = std::uniform_int_distribution<>(1, (int)planeSpecifications[c].passenger_count);
    }
}
RandomGenerators::~RandomGenerators() {}

std::mt19937 *RandomGenerators::getGenerator(){
    return &aGenerator;
}

double RandomGenerators::getRandomRange0to1() {
    return distribution0to1(aGenerator);
}
long RandomGenerators::getRandomPassengerDelay() {
    return delayDistribution(aGenerator);
}
Company RandomGenerators::getRandomCompany() {
    return allCompany[companyDistribution(aGenerator)];
}

long RandomGenerators::getRandomPassengerCount(Company aCompany) {
    if(aCompany < 0 || aCompany >= companyCount)
        return 1;
    return passengerDistribution[aCompany](aGenerator);
}
