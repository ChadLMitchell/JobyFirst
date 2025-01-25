//
//  SimClock.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//
#include <random>
#include "SimClock.hpp"
#include "Simulation.hpp"

SimClock::SimClock(Simulation *theSimulation, long endTime):
        theSimulation{theSimulation}, endTime{endTime}, currentTime{0}, needSort{false} {
    eventHandlers = std::vector<std::shared_ptr<EventHandler>>();
}
SimClock::~SimClock() {
}
long SimClock::getTime() {
    return currentTime;
}
void SimClock::addHandler(std::shared_ptr<EventHandler> aHandler) {
    // Keep eventHandlers sorted with soonest time at the end.
    // Insert the new handler into the first location to keep it sorted.
    auto handlerPtr = begin(eventHandlers);
    while(handlerPtr != end(eventHandlers) &&  (*handlerPtr)->getNextEventTime() > aHandler->getNextEventTime()) {
        handlerPtr++;
    }
    eventHandlers.insert(handlerPtr, aHandler);
}
void SimClock::reSortHandler(std::shared_ptr<EventHandler> aHandler) {
    auto handlerPtr = begin(eventHandlers);
    while(handlerPtr != end(eventHandlers)) {
        if(*handlerPtr == aHandler) {
            eventHandlers.erase(handlerPtr);
            addHandler(aHandler);
            return;
        }
        handlerPtr++;
    }
}
void SimClock::markNeedSort()
{
    needSort = true;
}
bool SimClock::checkSort() {
    return is_sorted(begin(eventHandlers), end (eventHandlers), [](std::shared_ptr<EventHandler> lhs, std::shared_ptr<EventHandler> rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
}
void SimClock::sortHandlers() {
    sort(begin(eventHandlers), end (eventHandlers), [](std::shared_ptr<EventHandler> lhs, std::shared_ptr<EventHandler> rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
}
bool SimClock::run(bool verbose) {
    // process events while there are any
    while(!eventHandlers.empty()) {
        if(needSort) {
            sortHandlers();
            needSort = false;
        }
        std::shared_ptr<EventHandler> nextEventHandler = eventHandlers.back();
        long nextTime = nextEventHandler->getNextEventTime();
        if(currentTime> nextTime) {
            std::cout << "Error in SimClock::run(): Out of order nextEventTime" << std::endl;
            std::cout << "currentTime: " << currentTime << std::endl;
            std::cout << "nextTime: " << nextTime << " for" << nextEventHandler-> describe() << std::endl;
        }
        if(nextTime >= endTime) {
            
            if(verbose) {
                std::cout << "Closing out clock at time " << endTime << " with " << eventHandlers.size() << " eventHandlers" << std::endl;
            }
            currentTime = endTime;
            break;
        }
        // Process this eventHandler
        eventHandlers.pop_back();
        if(nextTime > currentTime && verbose) {
            std::cout << "Advancing time to " << nextTime << std::endl;
        }
        currentTime = nextTime;
        if(verbose) {
            std::cout << "Call handleEvent() for " << nextEventHandler->describe() << std::endl;
        }
        if(nextEventHandler->handleEvent(currentTime, false)) {
            if(verbose) {
                std::cout << "Keeping event handler in SimClock queue" << std::endl;
            }
            if(currentTime >= nextEventHandler->getNextEventTime()) {
                // avoid infinite time loops by requiring reinserted eventHandlers to be in the future
                // if an eventHandler wants to do something else now, do it in the handler
                std::cout << "Error in SimClock::run(): Attempt to reinsert eventHandler not in future time" << std::endl;
                std::cout << "   Handler Time: " << nextEventHandler->getNextEventTime() << std::endl;
                std::cout << "   Current time: " << currentTime << std::endl;
            } else {
                if(verbose) {
                    std::cout << "Adding handler for " << nextEventHandler->describe() << " back to SimClock queue" << std::endl;
                }
                addHandler(nextEventHandler);
            }
        } else {
            if(verbose) {
                std::cout << "Removing event handler from SimClock queue" << std::endl;
            }

        }
    }
    // Close out remaining handlers
    for(std::shared_ptr<EventHandler> remainingEventHandler: eventHandlers) {
        if(verbose) {
            std::cout << "Close out handleEvent() for " << remainingEventHandler->describe() << std::endl;
        }
        remainingEventHandler->handleEvent(currentTime, true);
    }
    return true;
}
long SimClock::countPlanes() {
    long returnValue = 0;
    for(auto aHandler: eventHandlers) {
        returnValue += aHandler->countPlanes();
    }
    return returnValue;
}


const int maxRangeDelay{20};
const int maxTestHandlerRepeat{20};
const int testHandlerCount{100};

std::random_device rdTestHandler;
std::mt19937 genTestHandler(rdTestHandler());
std::uniform_int_distribution<> distribTestHandlerDelay(1, maxRangeDelay);
std::uniform_int_distribution<> distribTestHandlerRepeat(1, maxTestHandlerRepeat);

class TestHandler: public EventHandler {
    long repeats; // how many times does this run?
    long handlerID;
public:
    TestHandler(): EventHandler(LONG_MAX)  {
        repeats = 1;
        handlerID = 0;
    };
    TestHandler(long nextEventTime, int repeatCount, long handlerID): EventHandler(nextEventTime),repeats{repeatCount},handlerID{handlerID}  {
    };
    virtual ~TestHandler() override {
    };
    virtual bool handleEvent(long currentTime, bool closeOut) override {
        if(currentTime < nextEventTime) {
            std::cout << "Test Handler #" << handlerID << " ended early at " << currentTime << " wanted " << nextEventTime << std::endl;
        } else {
            std::cout << "Test Event for Handler #" << handlerID << " at " << currentTime << std::endl;
        }
        if(--repeats <= 0) return false;
        nextEventTime = currentTime + distribTestHandlerDelay(genTestHandler);
        return true;
    }
    virtual const std::string describe() override {
        std::string description = "Test EventHandler";
        return description;
    }
};

bool testSimClock(long howLongSeconds) {
    std::cout << "Test Sim Clock" << std::endl;
    SimClock aClock(nullptr, howLongSeconds);
    for(int i=0; i<testHandlerCount; i++) {
        aClock.addHandler(std::make_shared<TestHandler>(distribTestHandlerDelay(genTestHandler), distribTestHandlerRepeat(genTestHandler),i + 1));
    }
    
    return aClock.run(true);
}
