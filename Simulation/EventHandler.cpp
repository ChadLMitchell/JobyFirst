//
//  EventHandler.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/22/25.
//

#include "EventHandler.hpp"

EventHandler::EventHandler():  nextEventTime{} {
}
EventHandler::EventHandler(long nextEventTime):  nextEventTime{nextEventTime} {
}

EventHandler::~EventHandler() {
}
long EventHandler::getNextEventTime() {
    return nextEventTime;
}
void EventHandler::setNextEventTime(long aTime) {
    nextEventTime = aTime;
}
bool EventHandler::handleEvent(long currentTime, bool closeOut) {
    return false;
}
long EventHandler::countPlanes() {
    return 0;
}
const std::string EventHandler::describe() {
    std::string description = "Generic EventHandler";
    return description;
}
