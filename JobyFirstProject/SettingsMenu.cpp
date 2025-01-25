//
//  SettingsMenu.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/23/25.
//

#include "SettingsMenu.hpp"

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include "DebugHelp.hpp"
#include "CmdLineMenus.hpp"
#include "SimSettings.hpp"

using namespace std;

// Refer to the current settings to be used in creating simulations
extern SimSettings currentSettings;

// Simple number input. TO DO add range checking, etc.
long getNumberFromUser(string prompt) {
    cout << prompt;
    long result = 1;
    bool success = false;
    while(!success) {
        cin >> result;
        if(cin.fail()) {
            cin.clear();
            cin.ignore();
            cout << "Please enter a number: ";
        } else {
            success = true;
        }
    }
    return result;
}

// Get input from the user for the value for currentSettings.chargerCount
bool setChargerCount(int selector, MenuGroup &thisMenuGroup) {
    currentSettings.chargerCount = getNumberFromUser("Input number of chargers to use: ");
    return false;
}

// Get input from the user for the value for currentSettings.planeCount
bool setPlaneCount(int selector, MenuGroup &thisMenuGroup) {
    currentSettings.planeCount = getNumberFromUser("Input number of planes to use: ");
    return false;
}

// Get input from the user for the value for currentSettings.minPlanePerKind
bool setMinimumPlaneOption(int selector, MenuGroup &thisMenuGroup) {
    // With companyCount kinds, we need minPlanePerKind * companyCount to be <= planeCount
    // This is a deliberate use of integer math to round down to the highest integer that works
    long maxMinPlanePerKind = currentSettings.planeCount / companyCount;
    string message = "Input minimum number of planes per kind [0 - " + to_string(maxMinPlanePerKind) + "]: ";
    while(true) {
        long tempMinPlanePerKind = getNumberFromUser(message);
        if(tempMinPlanePerKind >= 0 && tempMinPlanePerKind <= maxMinPlanePerKind) {
            currentSettings.minPlanePerKind = tempMinPlanePerKind;
            return false;
        }
        cout << "With " << currentSettings.planeCount << " total planes and "
        << companyCount << " different kinds, the max for this setting is "
        << maxMinPlanePerKind << endl;
    }
    return false;
}

// Get input from the user for the value for currentSettings.simulationDuration
bool setDuration(int selector, MenuGroup &thisMenuGroup) {
    if(selector == 4) {
        currentSettings.simulationDuration = getNumberFromUser("Input Simulation duration (in seconds): ");
    } else {
        currentSettings.simulationDuration = getNumberFromUser("Input Simulation duration (in hours): ") * secondsPerHour;
    }
    return false;
}

// Implement a menu that selects the value for currentSettings.maxPassengerDelay
bool selectPassengerCountOption(int selector, MenuGroup &thisMenuGroup) {
    currentSettings.passengerCountOption = selector;
    return true;
}
vector<MenuItem> passengerCountOptionMenus {
    MenuItem('1', string{"Planes Always Fly Full"}, &selectPassengerCountOption, 0),
    MenuItem('2', string{"Randomly Distribute Pasengers Count on [1..Max]"}, &selectPassengerCountOption, 1),
};
MenuGroup passengerCountOptionMenu = MenuGroup(passengerCountOptionMenus);
bool setPassengerCountOption(int selector, MenuGroup &thisMenuGroup) {
    passengerCountOptionMenu.runMenu();
    return false;
}

// Get input from the user for the value for currentSettings.maxPassengerDelay
bool setPassengerDelayOption(int selector, MenuGroup &thisMenuGroup) {
    currentSettings.maxPassengerDelay = getNumberFromUser("Input maximum wait for new passengers: ");
    return false;
}

// Implement a menu that selects the value for currentSettings.faultOption
bool selectFaultOption(int selector, MenuGroup &thisMenuGroup) {
    currentSettings.faultOption = selector;
    return true;
}
vector<MenuItem> faultOptionMenus {
    MenuItem('1', string{"Just Count Faults"}, &selectFaultOption, 0),
    MenuItem('2', string{"On Fault, Ground Plane Immediately"}, &selectFaultOption, 1),
    MenuItem('3', string{"On Fault, Ground Plane at End of Flight"}, &selectFaultOption, 2),
};
MenuGroup faultOptionMenu = MenuGroup(faultOptionMenus);
bool setFaultOption(int selector, MenuGroup &thisMenuGroup) {
    faultOptionMenu.runMenu();
    return false;
}

// Implement settings menu
bool returnToMainMenu(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Chose return to main menu\n");
    return true;
}
vector<MenuItem> settingsMenus {
    MenuItem('1', string{"Set Charger Count"}, &setChargerCount, 1),
    MenuItem('2', string{"Set Plane Count"}, &setPlaneCount, 2),
    MenuItem('3', string{"Set Minimum Planes Per Kind Option"}, &setMinimumPlaneOption, 3),
    MenuItem('4', string{"Change Simulation Duration (in Seconds)"}, &setDuration, 4),
    MenuItem('5', string{"Change Simulation Duration (in Hours)"}, &setDuration, 5),
    MenuItem('6', string{"Set Passenger Count Option"}, &setPassengerCountOption, 6),
    MenuItem('7', string{"Set Passenger Delay Option"}, &setPassengerDelayOption, 7),
    MenuItem('8', string{"Set Fault Option"}, &setFaultOption, 8),
    MenuItem('M', string{"Return to Main Menu"}, &returnToMainMenu, 0)
};
MenuGroup settingsMenu = MenuGroup(settingsMenus);
bool editSettings(int selector, MenuGroup &thisMenuGroup) {
    debugMessage("===> Selected Edit Settings");
    settingsMenu.runMenu();
    return false;
}
