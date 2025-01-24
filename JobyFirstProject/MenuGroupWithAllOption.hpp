//
//  MenuGroupWithAllOption.hpp
//  Demo CPP Project
//
//  Created by Chad Mitchell on 1/16/25.
//

#ifndef MenuGroupWithAllOption_hpp
#define MenuGroupWithAllOption_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "MenuGroupWithAllOption.hpp"

class MenuGroupWithAllOption: public MenuGroup {
public:
    MenuGroupWithAllOption(std::vector<MenuItem> &someMenus);
    virtual ~MenuGroupWithAllOption() override;
    virtual bool runSpecial() override; // run all functions with non-zero selectors
};

#endif /* MenuGroupWithAllOption_hpp */
