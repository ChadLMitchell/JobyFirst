//
//  CmdLineMenus.h
//  Joby First Project
//
//  Created by Chad Mitchell on 1/14/25.
//


#ifndef CmdLineMenus_hpp
#define CmdLineMenus_hpp

#include <iostream>
#include <vector>
#include <queue>
#include <string>

class MenuItem; // Forward reference since they reference each other

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
class MenuGroup {
protected:
    std::vector<MenuItem> theMenus;
public:
    MenuGroup(std::vector<MenuItem> &someMenus);
    virtual ~MenuGroup();
    
    // Display the menu and handle user resposes until they requiest a higher level menu.
    void runMenu();
    
    // Allows extensions
    virtual bool runSpecial(); // placeholder for children to add functionaltiy
    
    // Get simple number input. TO-DO add range checking, etc.
    long getNumberFromUser(std::string prompt);
};

// Each MenuItem includes a pointer to the function that it will call when selected
// If the value is a nullptr then the menu will not do anything when selected
using MenuFuncPtr = bool (*)(int selector, MenuGroup &thisMenuGroup);

// Each MenuItem includes a selector character that the user inputs to select that menu
// These are two special values for the selector for display-only rows in a menu
const char menuSeparator{'-'}; // Displays a row of --------------------------------
const char menuIgnore{' '}; // Menu cannot be selected and does not display selector

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
class MenuItem {
    char menuSelect; // The character used to select the menu item
    std::string menuText; // The text of the menu item

    MenuFuncPtr funcPtr;     // This function performs the actions of the menu item.
                // It returns returns true if we should return to the higher level menu.
                // It cam be nullPtr and will be ignored in that case

    int selectorValue; // An optional selector int passed to the function
                // This allows a single function to implelent multiple menu items
public:
    MenuItem(char menuSelect, std::string menuText, MenuFuncPtr funcPtr, int selectorValue);
    ~MenuItem();

    // returns true if aChar matches menuSelect
    bool isSelected(char aChar);
    
    // Runs the function in funcPtr (if not a nullptr), passing to it the selectorValue.
    // Returns the return value of the function. If true, that tells the menuGroup run()
    // function to stop running the menu group returning to a higher levle menu or quitting.
    // The menuGroup passes itself to the function allowing the function to call back to
    // member function of the menuGroup (for example, runSpecial()).
    bool runFunction(MenuGroup &thisMenuGroup);
    
    // Access the value of the selector for this MenuItem
    int getSelectorValue();

    // Write the menu to cout
    void printItem();
};


#endif /* CmdLineMenus_hpp */
