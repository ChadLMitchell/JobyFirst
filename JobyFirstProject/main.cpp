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
using namespace std;

using TestFuncPtr = bool (*)(int selector);

bool testPlaneClass(int selector) {
    return testPlane(true);
}
bool test(int selector) {
    string message = "===> ran test " + to_string(selector);
    debugMessage(message);
    return false;
}

vector<TestFuncPtr> tests {
    testPlaneClass, // test 1
    test, // test 2
    test, // test 3
    test, // test 4
    test  // test 5
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
    MenuItem('2', string{"Run Test 2"}, &runTest, 2),
    MenuItem('3', string{"Run Test 3"}, &runTest, 3),
    MenuItem('4', string{"Run Test 4"}, &runTest, 4),
    MenuItem('5', string{"Run Test 5"}, &runTest, 5),
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

    mainMenu.runMenu();
    
    return 0;
}
