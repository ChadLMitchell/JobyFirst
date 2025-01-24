//
//  CmdLineMenus.h
//  Demo CPP Project
//
//  Created by Chad Mitchell on 1/14/25.
//

#ifndef CmdLineMenus_hpp
#define CmdLineMenus_hpp

#include <iostream>
#include <vector>
#include <string>

class MenuGroup;

using MenuFuncPtr = bool (*)(int selector, MenuGroup &thisMenuGroup);

class MenuItem {
    char menuSelect; // The character used to select the menu item
    std::string menuText; // The text of the menu item
    // The following performs the actions of the menu item.
    // It returns returns true if we should return to the higher level menu.
    MenuFuncPtr funcPtr;
    int selector; // An optional selector int passed to the function
public:
    MenuItem(char menuSelect, std::string menuText, MenuFuncPtr funcPtr, int selector);
    ~MenuItem();
    bool isSelected(char aChar);
    bool runFunction(MenuGroup &thisMenuGroup);
    int getSelector();
    void printItem();
};

class MenuGroup {
protected:
    std::vector<MenuItem> theMenus;
public:
    MenuGroup(std::vector<MenuItem> &someMenus);
    virtual ~MenuGroup();
    void runMenu();
    virtual bool runSpecial(); // placeholder for children
};

#endif /* CmdLineMenus_hpp */
