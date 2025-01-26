//
//  CmdLineMenus.cpp
//  Joby First Project
//
//  Created by Chad Mitchell on 1/14/25.
//

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "CmdLineMenus.hpp"
using namespace std;

// TO DO Move all text constants (char and string) to a separate file that could then be loaded
// from a file or resources and also to allow localization.
const char* menuPrompt {"Please select a menu item"}; // Initial menu propt
const char* unrecognizedSelection {"Unrecognized menu selection: "}; // Indicate inncorrect input
const char* menuSeparatorString {"---------------------------------"}; // separator between sections of a menu

/*
 *******************************************************************************************
 * Class MenuItem
 * This defines one item or row in a menu.
 * It incluedes a selector for the user to select the menu, some menu text, a pointer to
 * a function to run when the menu is selected and a selectorValue to pass to that function.
 * Unless it has a special value for the selector, each menu item will display with
 * a space, the selector, a space and then the text. When the user inputs the matching
 * selector the function is called passing the selectorValue.
 *******************************************************************************************
 */
MenuItem::MenuItem(char menuSelect, string menuText, MenuFuncPtr funcPtr, int selectorValue):
       menuSelect(menuSelect), menuText(menuText), funcPtr(funcPtr), selectorValue(selectorValue)
{
}
MenuItem::~MenuItem() {
}

// does the character match the selector for this item?
bool MenuItem::isSelected(char aChar) {
    // Special values for menuSelect are never selected
    if(menuSelect == menuSeparator || menuSelect == menuIgnore) { return false; }
    // return the comparison (ignoring case)
    return tolower(aChar)==tolower(menuSelect);
}

// run the function stored in the menu item
bool MenuItem::runFunction(MenuGroup &thisMenuGroup)  {
    if(funcPtr) {
        return funcPtr(selectorValue, thisMenuGroup);
    }
    return true;
}

// does the character match the selector for this item?
int MenuItem::getSelectorValue() {
    return selectorValue;
}

// Display the menu item to cout
void MenuItem::printItem() {
    if(menuSelect == menuSeparator) {
        // This is a separator so ouput the separateor text (such as "-------------------")
        cout << menuSeparatorString << endl;
    } else if(menuSelect == menuIgnore) {
        // This is an ignored menu item so do not print the selector
        cout  << menuText << endl;
    } else {
        // Display the selector and the menu text
        cout << "  " << menuSelect << ": " << menuText << endl;
    }
}

/*
 *******************************************************************************************
 * Class MenuGroup
 * This class along with MenuItems implement a simple command line menu manager.
 * To use them, create one or more MenuGroups passing a vector of MenuItem objects.
 * You can then call run() on a MenuGroup and it will display and process that menu.
 * run() only returns when the user requests an option to go to a higher level menu
 * or quit if they are already in the highest level menu.
 *******************************************************************************************
 */
MenuGroup::MenuGroup(vector<MenuItem> &someMenus): theMenus(someMenus) {
}
MenuGroup::~MenuGroup() {
}

// Run the menu
void MenuGroup::runMenu() {
    // Loop until the user chooses to close this menu
    while (true) {
        // Output the prompt and the menu items
        cout << endl << menuPrompt << endl;
        for (auto item : theMenus) {
            item.printItem();
        }
        cout << "> " << flush; // output a prompt
        // Get a character from the user
        char inChar;
        cin >> inChar;
        cin.ignore();
        // find it in the menuItem vector
        auto chosenMenu = find_if(begin(theMenus), end(theMenus), [inChar](MenuItem item){return item.isSelected(inChar);});
        // If it is not a special selector then ask the menuItem to run the function
        if(inChar != menuSeparator && inChar != menuIgnore && chosenMenu != end(theMenus)) {
            if (chosenMenu->runFunction(*this)) {
                return; // if the function returns true, return to the higher level menu or exit the menu system
            }
        } else {
            cout << unrecognizedSelection << "'" << inChar << "'" << endl;
        }
    }
}

// placeholder for children to add functionaltiy
bool MenuGroup::runSpecial() {
    cout << "===> ran special placeholder in menuGroup\n";
    return false;
}


