//
//  MenuGroupWithAllOption.hpp
//  Joby First Project
//
//  Created by Chad Mitchell on 1/16/25.
//

#ifndef MenuGroupWithAllOption_hpp
#define MenuGroupWithAllOption_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
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
class MenuGroupWithAllOption: public MenuGroup {
public:
    MenuGroupWithAllOption(std::vector<MenuItem> &someMenus);
    virtual ~MenuGroupWithAllOption() override;

    // This will run all of the menu items in order until one has a selectorValue == 0;
    virtual bool runSpecial() override; // run all functions with non-zero selectors
};

#endif /* MenuGroupWithAllOption_hpp */
