//
//  TestsMenu.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/23/25.
//

#include "TestsMenu.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "MenuGroupWithAllOption.hpp"
#include "Plane.hpp"
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
#include "PlaneQueue.hpp"
#include "SimSettings.hpp"
using namespace std;

using TestFuncPtr = bool (*)(int selector);

bool testPlaneClass(int selector) {
    if(testPlaneClassObjects(true)) {
        cout << "Test of Plane Class passed" << endl;
    } else {
        cout << "Test of Plane Class failed" << endl;
    }
    return false;
}
bool shortTestSimClockClass(int selector) {
    if(testSimClock(shortTestClockSeconds)) {
        cout << "Test of SimClock Class passed" << endl;
    } else {
        cout << "Test of SimClock Class failed" << endl;
    }
    return false;
}
bool longTestSimClockClass(int selector) {
    if(testSimClock(defaultTestClockSeconds)) {
        cout << "Test of SimClock Class passed" << endl;
    } else {
        cout << "Test of SimClock Class failed" << endl;
    }
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
bool shortTestPlaneQueue(int selector) {
    testPlaneQueueShort();
    return false;
}
bool longTestPlaneQueue(int selector) {
    testPlaneQueueLong();
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
    longTestSimClockClass, // test 3
    shortTestChargerQueue, // test 4
    longTestPlaneQueue,  // test 5
    shortTestPlaneQueue, // test 6
    longTestChargerQueue  // test 7
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
    MenuItem('3', string{"Long Test Sim Clock"}, &runTest, 3),
    MenuItem('4', string{"Short Test ChargerQueue"}, &runTest, 4),
    MenuItem('5', string{"Long Test ChargerQueue"}, &runTest, 5),
    MenuItem('6', string{"Short Test PlaneQueue"}, &runTest, 6),
    MenuItem('7', string{"Long Test PlaneQueue"}, &runTest, 7),
    MenuItem('A', string{"Run All Tests"}, &runAllTests, 0),
    MenuItem('M', string{"Return to Main Menu"}, &doMainMenu, 0)
};
MenuGroupWithAllOption testMenu = MenuGroupWithAllOption(testMenus);

bool runTests(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Tests");
    testMenu.runMenu();
    return false;
}
