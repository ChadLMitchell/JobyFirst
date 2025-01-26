//
//  TestsMenu.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/23/25.
//

#include "TestsMenu.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "MenuGroupWithAllOption.hpp"
#include "Simulation.hpp"
#include "SimSettings.hpp"

// This is the one file in the group including main() and menu handling that includes headers
// from the simulation other than Simulation.hpp and SimSettings.hpp. It uses the following
// three includes to call the test functions for these classes.
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"

using namespace std;

// Run a test of the Plane class
bool testPlaneClass(int selector) {
    if(testPlaneClassObjects(true)) {
        cout << "Test of Plane Class passed" << endl;
    } else {
        cout << "Test of Plane Class failed" << endl;
    }
    std::cout << std::endl;
    return false;
}
// Run a shorter test of the Clock class
bool shortTestSimClockClass(int selector) {
    if(testSimClock(false)) {
        cout << "Short Test of SimClock Class passed" << endl;
    } else {
        cout << "Short Test of SimClock Class failed" << endl;
    }
    std::cout << std::endl;
    return false;
}
// Run a longer test of the Clock class
bool longTestSimClockClass(int selector) {
    if(testSimClock(true)) {
        cout << "Short Test of SimClock Class passed" << endl;
    } else {
        cout << "Short Test of SimClock Class failed" << endl;
    }
    std::cout << std::endl;
    return false;
}
// Run a shorter test of the Charger class
bool shortTestChargerQueue(int selector) {
    testChargerQueueShort();
    return false;
}
// Run a longer test of the Charger class
bool longTestChargerQueue(int selector) {
    testChargerQueueLong();
    return false;
}
// Test the way the PlaneQueue processes waits for passengers
// That is only relevant if the options include such waits, but
// we run the test independent of the current settings
bool testPlaneQueueWaits(int selector) {
    testPlaneQueueForWaits();
    return false;
}
// Test that the PlaneQueue class generates the right distirbution of planes
// given different constraints on the minimum number of planes needed from each company
bool testPlaneQueueMinimumPerKind(int selector) {
    testPlaneQueueForMinimumPerKind();
    return false;
}

// This menu handles the test functions differently. All the menu items refer to the function
// runTest which uses the selector to look up the test in this list.
using TestFuncPtr = bool (*)(int selector);
vector<TestFuncPtr> tests {
    testPlaneClass, // test 1
    shortTestSimClockClass, // test 2
    shortTestChargerQueue, // test 3
    longTestChargerQueue,  // test 4
    testPlaneQueueWaits,  // test 5
    testPlaneQueueMinimumPerKind, // test 6
    longTestSimClockClass // test 7
};

// Check that the selector is in range, then use it to choose the function to run
bool runTest(int selector, MenuGroup &thisMenuGroup) {
    if(selector < 1 || selector > tests.size()) {
        cout << "Test selector " << to_string(selector) << "out of range" << endl;
        return false;
    } else {
        return tests[selector - 1](selector);
    }
}

// The test menu is of MenuGroupWithAllOption, a child of MenuGroup
// that implements the runSpecial function to run all of the menus
// up to the first menu with a selectorValue of 0. We do it this
// way honoring object oriented approaches so that the menuGroup
// can be called on to loop through the menu items in the group
// which would not be visible to an individual menu item.
bool runAllTests(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected 'Run All Above Tests'");
    thisMenuGroup.runSpecial();
    return false;
}

// Respond to a selection to return to the main menu
bool doMainMenu(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> chose return to main menu\n");
    return true;
}

// The menu items for this menu. Note that all but the last call runTest().
vector<MenuItem> testMenus {
    MenuItem('1', string{"Test Plane Class"}, &runTest, 1),
    MenuItem('2', string{"Short Test Sim Clock"}, &runTest, 2),
    MenuItem('3', string{"Short Test ChargerQueue"}, &runTest, 3),
    MenuItem('4', string{"Long Test ChargerQueue"}, &runTest, 4),
    MenuItem('5', string{"Test PlaneQueue: Waits for Passengers"}, &runTest, 5),
    MenuItem('6', string{"Test PlaneQueue: Minimum per Kind"}, &runTest, 6),
    MenuItem('-', string{""}, nullptr, 0),
    MenuItem('A', string{"Run All Above Tests"}, &runAllTests, 0),
    MenuItem('L', string{"Long Test Sim Clock"}, &runTest, 7),
    MenuItem('M', string{"Return to Main Menu"}, &doMainMenu, 0)
};
MenuGroupWithAllOption testMenu = MenuGroupWithAllOption(testMenus);

// This is the only external entry point to this file that is called when we want to display
// and use the manage the tests menu.
bool runTests(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Tests");
    testMenu.runMenu();
    return false;
}
