//
//  main.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/17/25.
//

#include <iostream>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "MenuGroupWithAllOption.hpp"
#include "Plane.hpp"
#include "SimClock.hpp"
#include "ChargerQueue.hpp"
using namespace std;

using TestFuncPtr = bool (*)(int selector);

bool testPlaneClass(int selector) {
    if(testPlane(true)) {
        cout << "Test of Plane Class passed" << endl;
    } else {
        cout << "Test of Plane Class failed" << endl;
    }
    return false;
}
const long longTestClockSeconds{60*60*3}; //3 hours
const long shortTestClockSeconds{60}; //1 minute
bool shortTestSimClockClass(int selector) {
    if(testSimClock(shortTestClockSeconds)) {
        cout << "Test of SimClock Class passed" << endl;
    } else {
        cout << "Test of SimClock Class failed" << endl;
    }
    return false;
}
bool longTestSimClockClass(int selector) {
    if(testSimClock(longTestClockSeconds)) {
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
    longTestChargerQueue  // test 5
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
    MenuItem('A', string{"Run All Tests"}, &runAllTests, 0),
    MenuItem('M', string{"Return to Main Menu"}, &doMainMenu, 0)
};
MenuGroupWithAllOption testMenu = MenuGroupWithAllOption(testMenus);

bool runTests(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Run Tests");
    testMenu.runMenu();
    return false;
}
bool doQuit(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Quit");
    return true;
}

vector<MenuItem> mainMenus {
    MenuItem('T', string{"Run Tests"}, &runTests, 0),
    MenuItem('Q', string{"Quit"}, &doQuit, 0)
};
MenuGroup mainMenu = MenuGroup(mainMenus);

int main(int argc, const char * argv[]) {
    debugMessage("Start program!");

 //   shared_ptr<Simulation> aSimulation = make_shared<Simulation>();
    mainMenu.runMenu();

    return 0;
}
