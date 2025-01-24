//
//  EventHandler.hpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#ifndef EventHandler_hpp
#define EventHandler_hpp

#include <stdio.h>
#include <iostream>

class EventHandler {
protected:
    long nextEventTime;
public:
    EventHandler();
    EventHandler(long nextEventTime);
    virtual ~EventHandler();
    long getNextEventTime();
    void setNextEventTime(long aTime);
    virtual bool handleEvent(long currentTime, bool closeOut);
    virtual long countPlanes();
    virtual const std::string describe();
};

#endif /* EventHandler_hpp */
