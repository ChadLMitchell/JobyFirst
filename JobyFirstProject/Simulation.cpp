//
//  Simulation.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#include "Simulation.hpp"
#include "SimClock.hpp"
#include "SimSettings.hpp"

Simulation::Simulation() {
    theSimClock = std::make_shared<SimClock>(this, simulationDuration);
}
Simulation::~Simulation() {
    
}
