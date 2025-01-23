//
//  Simulation.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/21/25.
//

#include "Simulation.hpp"
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"
#include "SimSettings.hpp"

Simulation::Simulation(SimSettings someSettings):
 theSimClock{}, theChargerQueue{} {
     theSettings = std::make_shared<SimSettings>(someSettings);
}
Simulation::~Simulation() {
    
}
bool Simulation::run()
{
    theSimClock = std::make_shared<SimClock>(this, theSettings->simulationDuration);
    theChargerQueue = std::make_shared<ChargerQueue>(this, theSettings->chargerCount);
    thePlaneQueue = std::make_shared<PlaneQueue>(this);
    return true;
}
