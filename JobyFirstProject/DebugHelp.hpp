//
//  DebugHelp.hpp
//  Joby First Project
//
//  Created by Chad Mitchell on 1/15/25.
//

#ifndef DebugHelp_hpp
#define DebugHelp_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <iostream>

#define DODEBUG 1

// This is used to indicate some situations where in production we may not want to produce input,
// but for now we do.
// TO DO: There are probably some cases where we output to cout directly when we should use this function.
inline void debugMessage(const std::string message) {
#if DODEBUG
    std::cout << message << std::endl;
#endif
}

#endif /* DebugHelp_hpp */
