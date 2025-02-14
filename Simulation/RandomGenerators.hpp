//
//  RandomGenerators.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 2/13/25.
//

#ifndef RandomGenerators_hpp
#define RandomGenerators_hpp

#include <stdio.h>
#include <random>
#include "Simulation.hpp"

class RandomGenerators {
    Simulation *theSimulation;
    std::random_device aRandomDevice;
    std::mt19937 aGenerator;
    std::uniform_real_distribution<> distribution0to1;
    std::uniform_int_distribution<> delayDistribution;
    std::uniform_int_distribution<> companyDistribution;

    std::array<std::uniform_int_distribution<>, companyCount> passengerDistribution;

public:
    RandomGenerators(Simulation *theSimulation);
    ~RandomGenerators();
    std::mt19937 *getGenerator();
    double getRandomRange0to1();
    long getRandomPassengerDelay();
    long getRandomPassengerCount(Company aCompany);
    Company getRandomCompany();
};


#endif /* RandomGenerators_hpp */
