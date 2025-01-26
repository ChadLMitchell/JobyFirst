//
//  MenuGroupWithAllOption.cpp
//  Joby First Project
//
//  Created by Chad Mitchell on 1/16/25.
//

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "CmdLineMenus.hpp"
#include "MenuGroupWithAllOption.hpp"

/*
 *******************************************************************************************
 * Class MenuGroupWithAllOption
 * This child of MenuGroup overrides runSpecial to implement an "All" menu which will.
 * run all of the menus items in sequence top to bottom up to the point where one has a
 * selectorValue == 0.
 * This is used, for example, to define a "Run All Tests" option in a Test menu.
 *******************************************************************************************
 */
MenuGroupWithAllOption::MenuGroupWithAllOption(std::vector<MenuItem> &someMenus): MenuGroup(someMenus)
{
}
MenuGroupWithAllOption::~MenuGroupWithAllOption() {
}

// This will run all of the menu items in order until one has a selectorValue == 0;
bool MenuGroupWithAllOption::runSpecial() {
    for (auto item : theMenus) {
        int selector = item.getSelectorValue();
        if(selector == 0) {
            return false; // the first selector with 0 selector ends running all the menu items
        } else if (item.runFunction(*this)) {
            // If the function for this menu item requests that we terminate the menu by returning
            // true then we do it immediately and do not continue through the menu items
            return true; // return to the higher level menu or exit the menu system
        }
    }
    return false;
}
