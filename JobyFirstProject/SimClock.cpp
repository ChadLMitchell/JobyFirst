//
//  SimClock.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//
#include <random>
#include "SimClock.hpp"

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
bool EventHandler::handleEvent(long currentTime) {
    return false;
}

SimClock::SimClock(long endTime): endTime{endTime}, currentTime{0}, needSort{false} {
    eventHandlers = std::vector<std::shared_ptr<EventHandler>>();
}
SimClock::~SimClock() {
}
long SimClock::getTime() {
    return currentTime;
}
bool SimClock::addHandler(std::shared_ptr<EventHandler> aHandler) {
    // Keep eventHandlers sorted with soonest time at the end.
    // Insert the new handler into the first location to keep it sorted.
    auto handlerPtr = begin(eventHandlers);
    while(handlerPtr != end(eventHandlers) &&  (*handlerPtr)->getNextEventTime() > aHandler->getNextEventTime()) {
        handlerPtr++;
    }
    eventHandlers.insert(handlerPtr, aHandler);
    return true;
}
void SimClock::markNeedSort()
{
    needSort = true;
}
void SimClock::sortHandlers() {
    sort(begin(eventHandlers), end (eventHandlers), [](auto lhs, auto rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
}
bool SimClock::run() {
    while(!eventHandlers.empty()) {
        if(needSort) {
            sortHandlers();
            needSort = false;
        }
        std::shared_ptr<EventHandler> nextEventHandler = eventHandlers.back();
        eventHandlers.pop_back();
        long nextTime = nextEventHandler->getNextEventTime();
        if(currentTime> nextTime) {
            std::cout << "Error in SimClock::run(): Out of order nextEventTime" << std::endl;
        }
        if(nextTime > endTime) {
            currentTime = endTime;
            break;
        }
        currentTime = nextTime;
        if(nextEventHandler->handleEvent(currentTime)) {
            if(currentTime >= nextEventHandler->getNextEventTime()) {
                // avoid infinite time loops by requiring reinserted eventHandlers to be in the future
                // if an eventHandler wants to do something else now, do it in the handler
                std::cout << "Error in SimClock::run(): Attempt to reinsert eventHandler not in future time" << std::endl;
                std::cout << "   Handler Time: " << nextEventHandler->getNextEventTime() << std::endl;
                std::cout << "   Current time: " << currentTime << std::endl;
            } else {
                addHandler(nextEventHandler);
            }
        }
        
    }
    for(std::shared_ptr<EventHandler> remainingEventHandler: eventHandlers) {
        remainingEventHandler->handleEvent(currentTime);
    }
    return true;
}

const int maxRangeDelay{20};
const int maxTestHandlerRepeat{20};
const int testHandlerCount{100};

std::random_device rdTestHandler;
std::mt19937 genTestHandler(rdTestHandler());
std::uniform_int_distribution<> distribTestHandlerDelay(1, maxRangeDelay);
std::uniform_int_distribution<> distribTestHandlerRepeat(1, maxTestHandlerRepeat);

class TestHandler: public EventHandler {
    static long errorCount;
    long repeats; // how many times does this run?
    long handlerID;
public:
    TestHandler(): EventHandler()  {
        repeats = 1;
        handlerID = 0;
    };
    TestHandler(long nextEventTime, int repeatCount, long handlerID): EventHandler(nextEventTime),repeats{repeatCount},handlerID{handlerID}  {
    };
    virtual ~TestHandler() override {
    };
    virtual bool handleEvent(long currentTime) override {
        if(currentTime < nextEventTime) {
            std::cout << "Test Handler #" << handlerID << " ended early at " << currentTime << " wanted " << nextEventTime << std::endl;
        } else {
            std::cout << "Test Event for Handler #" << handlerID << " at " << currentTime << std::endl;
        }
        if(--repeats <= 0) return false;
        nextEventTime = currentTime + distribTestHandlerDelay(genTestHandler);
        return true;
    }
    long getErrorCount() {
        return errorCount;
    }
};
long TestHandler::errorCount = 0;

bool testSimClock(long howLongSeconds) {
    std::cout << "Test Sim Clock" << std::endl;
    bool returnValue = true;
    SimClock aClock(howLongSeconds);
 
    for(int i=0; i<testHandlerCount; i++) {
        returnValue = aClock.addHandler(std::make_shared<TestHandler>(distribTestHandlerDelay(genTestHandler), distribTestHandlerRepeat(genTestHandler),i + 1)) && returnValue;
    }
    if(returnValue) {
        returnValue = aClock.run();
    }
    
    return returnValue;
}

std::shared_ptr<SimClock> theSimClock{};
