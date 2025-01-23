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
#include "SimSettings.hpp"

class SimClock;
class ChargerQueue;
class PlaneQueue;

class Simulation {
   
public:
    std::shared_ptr<SimSettings> theSettings;
    std::shared_ptr<SimClock> theSimClock;
    std::shared_ptr<ChargerQueue> theChargerQueue;
    std::shared_ptr<PlaneQueue> thePlaneQueue;

 //   Simulation(std::shared_ptr<SimSettings> theSettings);
    Simulation(SimSettings someSettings);
    ~Simulation();
    bool run();

    
    
};

#endif /* Simulation_hpp */
