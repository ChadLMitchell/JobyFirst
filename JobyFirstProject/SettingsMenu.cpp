//
//  SettingsMenu.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/23/25.
//

#include "SettingsMenu.hpp"

#include <iostream>
#include <iomanip>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "SimSettings.hpp"
using namespace std;

bool setChargerCount(int selector, MenuGroup &thisMenuGroup) {
    cout << "Chose setChargerCount (to be implemented later)" << endl;
    return false;
}
bool setPlaneCount(int selector, MenuGroup &thisMenuGroup) {
    cout << "Chose setPlaneCount (to be implemented later)" << endl;
    return false;
}
bool setRandomOption(int selector, MenuGroup &thisMenuGroup) {
    cout << "Chose setRandomOption (to be implemented later)" << endl;
    return false;
}
bool setDuration(int selector, MenuGroup &thisMenuGroup) {
    cout << "Chose setDuration (to be implemented later)" << endl;
    return false;
}
bool setPassengerOption(int selector, MenuGroup &thisMenuGroup) {
    cout << "Chose setPassengerOption (to be implemented later)" << endl;
    return false;
}
bool setFaultOption(int selector, MenuGroup &thisMenuGroup) {
    cout << "Chose setFaultOption (to be implemented later)" << endl;
    return false;
}
bool returnToMainMenu(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Chose return to main menu\n");
    return true;
}

vector<MenuItem> settingsMenus {
    MenuItem('1', string{"Set Charger Count"}, &setChargerCount, 1),
    MenuItem('2', string{"Set Plane Count"}, &setPlaneCount, 2),
    MenuItem('3', string{"Set Random Plane Option"}, &setRandomOption, 3),
    MenuItem('4', string{"Change Simulation Duration"}, &setDuration, 4),
    MenuItem('5', string{"Set Passenger Count Option"}, &setPassengerOption, 5),
    MenuItem('6', string{"Set Fault Option"}, &setFaultOption, 6),
    MenuItem('M', string{"Return to Main Menu"}, &returnToMainMenu, 0)
};
MenuGroup settingsMenu = MenuGroup(settingsMenus);

bool editSettings(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Edit Settings");
    settingsMenu.runMenu();
    return false;
}
