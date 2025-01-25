//
//  CmdLineMenus.cpp
//  Demo CPP Project
//
//  Created by Chad Mitchell on 1/14/25.
//

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "CmdLineMenus.hpp"
using namespace std;

const char* menuPrompt {"Please select a menu item"};
const char* unrecognizedSelection {"Unrecognized menu selection: "};

MenuItem::MenuItem(char menuSelect, string menuText, MenuFuncPtr funcPtr, int selector):
       menuSelect(menuSelect), menuText(menuText), funcPtr(funcPtr), selector(selector)
    // The funcPtr function performs the actions of the menu item.
    // It returns returns true if we should return to the higher level menu.
{
        
}
MenuItem::~MenuItem() {
}
bool MenuItem::isSelected(char aChar) { // does the character match the selector for this item?
    return tolower(aChar)==tolower(menuSelect);
}
bool MenuItem::runFunction(MenuGroup &thisMenuGroup)  {
    // run the function stored in the menu item
    return funcPtr(selector, thisMenuGroup);
}
int MenuItem::getSelector() { // does the character match the selector for this item?
    return selector;
}
void MenuItem::printItem() { // print the menu item
    cout << "  " << menuSelect << ": " << menuText << endl;
}

MenuGroup::MenuGroup(vector<MenuItem> &someMenus): theMenus(someMenus) {
}
void MenuGroup::runMenu() {
    while (true) {
        cout << endl << menuPrompt << endl;
        for (auto item : theMenus) {
            item.printItem();
        }
        cout << "> " << flush; // output a prompt
        char inChar;
        cin >> inChar;
        cin.ignore();
        auto chosenMenu = find_if(begin(theMenus), end(theMenus), [inChar](MenuItem item){return item.isSelected(inChar);});
        if(chosenMenu != end(theMenus)) {
            if (chosenMenu->runFunction(*this)) {
                return; // return to the higher level menu or exit the menu system
            }
        } else {
            cout << unrecognizedSelection << "'" << inChar << "'" << endl;
        }
/*
 for (auto item : this->theMenus) {
            if(item.isSelected(inChar)) {
                if (item.runFunction(*this)) {
                    return; // return to the higher level menu or exit the menu system
                }
                break; // redisplay the current menu
            }
        }
 */
    }
}
MenuGroup::~MenuGroup() {
}
bool MenuGroup::runSpecial() {
    cout << "===> ran special placeholder in menuGroup\n";
    return false;
}


