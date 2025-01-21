//
//  Simulation.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#ifndef Simulation_hpp
#define Simulation_hpp

#include <stdio.h>
#include <iostream>

class SimClock;
class ChargerQueue;

class Simulation {
   
public:
    std::shared_ptr<SimClock> theSimClock;
    std::shared_ptr<ChargerQueue> theChargerQueue;

    Simulation();
    ~Simulation();
    

    
    
};

#endif /* Simulation_hpp */
