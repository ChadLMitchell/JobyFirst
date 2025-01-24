//
//  main.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/17/25.
//

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "TestsMenu.hpp"
#include "MainMenu.hpp"
#include "MenuGroupWithAllOption.hpp"
#include "Plane.hpp"
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"
#include "SimSettings.hpp"

SimSettings currentSettings;

int main(int argc, const char * argv[]) {
    debugMessage("Start program!");
    // seed the default random number generatpr
    srand(time(0) & INT_MAX);

    mainMenu.runMenu();

    return 0;
}
