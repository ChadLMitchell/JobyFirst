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
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"
#include "Simulation.hpp"
#include "SimSettings.hpp"
using namespace std;

using TestFuncPtr = bool (*)(int selector);

bool testPlaneClass(int selector) {
    if(testPlaneClassObjects(true)) {
        cout << "Test of Plane Class passed" << endl;
    } else {
        cout << "Test of Plane Class failed" << endl;
    }
    std::cout << std::endl;
    return false;
}
bool shortTestSimClockClass(int selector) {
    if(testSimClock(false)) {
        cout << "Short Test of SimClock Class passed" << endl;
    } else {
        cout << "Short Test of SimClock Class failed" << endl;
    }
    std::cout << std::endl;
    return false;
}
bool longTestSimClockClass(int selector) {
    if(testSimClock(true)) {
        cout << "Short Test of SimClock Class passed" << endl;
    } else {
        cout << "Short Test of SimClock Class failed" << endl;
    }
    std::cout << std::endl;
    return false;
}
bool shortTestChargerQueue(int selector) {
    testChargerQueueShort();
    return false;
}
bool longTestChargerQueue(int selector) {
    testChargerQueueLong();
    return false;
}
bool testPlaneQueueWaits(int selector) {
    testPlaneQueueWaits();
    return false;
}
bool testPlaneQueueMinimumPerKind(int selector) {
    testPlaneQueueMinimumPerKind();
    return false;
}

bool test(int selector) {
    string message = "===> ran test " + to_string(selector);
    debugMessage(message);
    return false;
}

vector<TestFuncPtr> tests {
    testPlaneClass, // test 1
    shortTestSimClockClass, // test 2
    shortTestChargerQueue, // test 3
    longTestChargerQueue,  // test 4
    testPlaneQueueWaits,  // test 5
    testPlaneQueueMinimumPerKind, // test 6
    longTestSimClockClass // test 7
};

bool runTest(int selector, MenuGroup &thisMenuGroup) {
    if(selector < 1 || selector > tests.size()) {
        cout << "Test selector " << to_string(selector) << "out of range" << endl;
        return false;
    } else {
        return tests[selector - 1](selector);
    }
}
bool runAllTests(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run All Above Tests");
    thisMenuGroup.runSpecial();
    return false;
}
bool doMainMenu(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> chose return to main menu\n");
    return true;
}

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

bool runTests(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Tests");
    testMenu.runMenu();
    return false;
}
