//
//  MenuGroupWithAllOption.cpp
//  Demo CPP Project
//
//  Created by Chad Mitchell on 1/16/25.
//

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "CmdLineMenus.hpp"
#include "MenuGroupWithAllOption.hpp"

MenuGroupWithAllOption::MenuGroupWithAllOption(std::vector<MenuItem> &someMenus): MenuGroup(someMenus)
{
}
MenuGroupWithAllOption::~MenuGroupWithAllOption() {
}
bool MenuGroupWithAllOption::runSpecial() {
    for (auto item : theMenus) {
        int selector = item.getSelector();
        if(selector != 0) {
            if (item.runFunction(*this)) {
                return true; // return to the higher level menu or exit the menu system
            }
        }
    }
    return false;
}
