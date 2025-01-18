//
//  DebugHelp.hpp
//  Demo CPP Project
//
//  Created by Chad Mitchell on 1/15/25.
//

#ifndef DebugHelp_hpp
#define DebugHelp_hpp

#include <stdio.h>
#include <iostream>

#define DODEBUG 1

inline void debugMessage(const std::string message) {
#if DODEBUG
    std::cout << message << std::endl;
#endif
}

#endif /* DebugHelp_hpp */
