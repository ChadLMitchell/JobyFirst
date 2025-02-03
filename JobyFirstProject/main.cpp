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
#include "SimSettings.hpp"

// Define default settings
// These can be edited using the settings menu
// Some simulations will override some settings (at least the duration)
SimSettings currentSettings;

void setRandomSeed() {
    // if the randomSeed has a value, then use it to provide consistent regression testing
    // if it is 0, use a random seed
    if(useRandomSeed) {
        srand(useRandomSeed & INT_MAX);
    } else {
        // get a seed from the system clock
        srand(time(0) & INT_MAX);
    }
}

int main(int argc, const char * argv[]) {
    debugMessage("Start program!");
    // seed the default random number generator
    setRandomSeed();
    // run the main menu
    mainMenu.runMenu();

    return 0;
}
