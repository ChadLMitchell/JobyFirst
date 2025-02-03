//
//  SimClock.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//
#include <random>
#include "SimClock.hpp"
#include "Simulation.hpp"

/*
 *******************************************************************************************
 * Class SimClock
 * This manages a vector of EventHandlers. The vector is kept sorted so that the handler
 * with the earliest time is at the end ready to be popped off.
 *
 * This uses a "quantum" clock meaning that the time jumps from one meaningful time to the
 * next without passing through the times in between. It requires all EventHandlers to be
 * able to pre-calculate when the next event they will care about will happen so the
 * SimClock can skip directly to the next time that matters.
 *
 * All handlers will be descendants of EventHandler.
 *
 * Once everything is set up, the simulation proceeds by calling the run() in this object.
 * That function will loop until time runs out for the simulation or until there are no
 * more EventHandlers listed.
 *******************************************************************************************
 */
SimClock::SimClock(Simulation *theSimulation, long endTime):
        theSimulation{theSimulation}, endTime{endTime}, currentTime{0}, needSort{false} {
    // initialize our vector of event handlers
    eventHandlers = std::vector<std::shared_ptr<EventHandler>>();
}
SimClock::~SimClock() {
}

// Get the private variable for the current time
long SimClock::getTime() {
    return currentTime;
}

// Insert a handler into the correct location to keep the vector sorted with the handler with the soonest next event time
// at the end of the vector
void SimClock::addHandler(std::shared_ptr<EventHandler> aHandler) {
    // Keep eventHandlers sorted with soonest time at the end.
    // Insert the new handler into the first location to keep it sorted.
    auto handlerPtr = begin(eventHandlers);
    while(handlerPtr != end(eventHandlers) &&  (*handlerPtr)->getNextEventTime() > aHandler->getNextEventTime()) {
        handlerPtr++;
    }
    eventHandlers.insert(handlerPtr, aHandler);
}

// One of our handlers thinks that it may be in the wrong order in the vector due to internal changes.
// If we can find the charger in the queue, we remove it and insert it back in, sorting it in the proper
// location. But if the handler is not in the queue than nothing happens. For example, if a handler
// asks us to do this while responding to a handleEvent() call, we already removed it from the vector
// before calling handleEvent() and will add it back when that handleEvent() returns so no need to do anyting now.
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

// Set internal variable indicating that we need to resort the vector at the start of the next iteration of the run loop
// So far this has not been needed.
void SimClock::markNeedSort()
{
    needSort = true;
}


// For testing: check if the vector is properly sorted
bool SimClock::checkSort() {
    return is_sorted(begin(eventHandlers), end (eventHandlers), [](std::shared_ptr<EventHandler> lhs, std::shared_ptr<EventHandler> rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
}

// Sort the vector of handlers
// This function is private so only this object can call it at times that are safe
void SimClock::sortHandlers() {
    sort(begin(eventHandlers), end (eventHandlers), [](std::shared_ptr<EventHandler> lhs, std::shared_ptr<EventHandler> rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
}

// Run the actual simulation
bool SimClock::run(bool verbose) {

    // Decide if we need to share progress status
    long nextProgressUpdate = LONG_MAX; // default is never do a progress update
    long progressInterval = 0; // How often do we show progress (<= 0 means do not show it)
    // Don't bother with progress indicator if we are being verbose anyway
    if(!verbose && theSimulation) {
        // Ask the simulation to indicate if we should show the progress indicator
        progressInterval = theSimulation->getProgressInterval();
        if(progressInterval > 0)
            nextProgressUpdate = progressInterval;
    }
    
    // process events while there are any in our list
    while(!eventHandlers.empty()) {
        // If some other object has indicate that we may need to sort, this is a safe time to do it
        if(needSort) {
            sortHandlers();
            needSort = false;
        }
        // Pull the handler with the lowest next time off the end of the list (kept sorted that way)
        std::shared_ptr<EventHandler> nextEventHandler = eventHandlers.back();
        long nextTime = nextEventHandler->getNextEventTime();
        if(currentTime> nextTime) {
            // This should never happen so indiate there was a problem we need to fix
            std::cout << "Error in SimClock::run(): Out of order nextEventTime" << std::endl;
            std::cout << "currentTime: " << currentTime << std::endl;
            std::cout << "nextTime: " << nextTime << " for" << nextEventHandler-> describe() << std::endl;
        }
 
        // If it is time for a progress update, do it.
        // This is set up as a simple comparison so there is no overhead except when we actually update the indicator
        if(nextTime >= nextProgressUpdate) {
            long timeToDisplay = nextTime;
            if(timeToDisplay > endTime) {
                timeToDisplay = endTime;
            }
            // It is time for a progress update
            std::cout << "\r" << std::flush;
            std::cout << "Simulation Clock Time = " << timeToDisplay/secondsPerHour
            << " of " << endTime/secondsPerHour << " hours" << std::flush;
            nextProgressUpdate += progressInterval;
        }

        if(nextTime >= endTime) {
            // This means that we have reached the end of the simulation
            // The current eventHandler wants us to move to or beyond the end of the simulation
            
            if(verbose) {
                std::cout << std::endl;
                std::cout << "Closing out clock at time " << endTime << " with " << eventHandlers.size() << " eventHandlers" << std::endl;
            }
            currentTime = endTime;
            break;
        }
        // Advance the current time
        eventHandlers.pop_back();
        if(nextTime > currentTime && verbose) {
            std::cout << std::endl;
            std::cout << "Advancing time to " << nextTime << std::endl;
        }
        currentTime = nextTime;
        if(verbose) {
            std::cout << "Call handleEvent() for " << nextEventHandler->describe() << std::endl;
        }
        // Have the current eventHandler process an event
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
                // if the eventHandler returned true, then it wants to stay in the list of handlers,
                // but we already removed it so put it back in the list.
                if(verbose) {
                    std::cout << "Adding handler for " << nextEventHandler->describe() << " back to SimClock queue" << std::endl;
                }
                addHandler(nextEventHandler);
            }
        } else {
            // If the eventHandler returned false, it does not want to stay in the list of handlers,
            // but we already removed it so nothing to do here
            if(verbose) {
                std::cout << "Removing event handler from SimClock queue" << std::endl;
            }

        }
    }
    if(nextProgressUpdate < LONG_MAX) {
        // if we did progress reports, close out the line that we kept reusing
        std::cout << std::endl;
    }
    // Close out remaining handlers
    for(std::shared_ptr<EventHandler> remainingEventHandler: eventHandlers) {
        if(verbose) {
            std::cout << "Close out handleEvent() for " << remainingEventHandler->describe() << std::endl;
        }
        // Notify them that this is the final close-out in case they need to do something different
        remainingEventHandler->handleEvent(currentTime, true);
    }
    return true;
}

// For testing: sum all the planes in all of the handlers
// TO-DO: use this in some testing to ensure that all planes exist througohut the simulation and that a plane is
// never in more than one place at the same time.
long SimClock::countPlanes() {
    long returnValue = 0;
    for(auto aHandler: eventHandlers) {
        returnValue += aHandler->countPlanes();
    }
    return returnValue;
}


// These are constants to use in the test event handlers
const int maxRangeDelay{20}; // Test event handlers will delay in a range [2 - maxRangeDelay] seconds after each event
const int maxTestHandlerRepeat{20}; // Ech test event handler will be asked to process [1 - maxTestHandlerRepeat] events
const int longTestHandlerCount{25}; // How many test handlers are created for a long test?
const int shortTestHandlerCount{5}; // How many test handlers are created for a short test?

// Set up a random number generator and some distributions for the SimClock tests
std::random_device rdTestHandler;
std::mt19937 genTestHandler(useRandomSeed ? useRandomSeed : rdTestHandler());
std::uniform_int_distribution<> distribTestHandlerDelay(2, maxRangeDelay);
std::uniform_int_distribution<> distribTestHandlerRepeat(1, maxTestHandlerRepeat);

/*
 *******************************************************************************************
 * Class TestHandler
 * This is a child of EventHandler. It is a simple event handler for testing.
 *******************************************************************************************
 */
class TestHandler: public EventHandler {
    long repeats; // How many events does this process?
    long handlerID; // A unique identification for output
public:
    TestHandler(): EventHandler(LONG_MAX), repeats{1}, handlerID{0}   {
    };
    TestHandler(long nextEventTime, int repeatCount, long handlerID): EventHandler(nextEventTime),repeats{repeatCount},handlerID{handlerID}  {
    };
    virtual ~TestHandler() override {
    };
 
    // Handle a simulated event
    virtual bool handleEvent(long currentTime, bool closeOut) override {
        // Inform the user that this handler is processing an event.
        if(currentTime < nextEventTime) {
            std::cout << "Test Handler #" << handlerID << " ended early at " << currentTime << " wanted " << nextEventTime << std::endl;
        } else {
            std::cout << "Test Event for Handler #" << handlerID << " at " << currentTime << std::endl;
        }
        // If we are done, ask that we be removed from the list of active event handlers
        if(--repeats <= 0) { return false; }
        // Get a delay for the next event we want to handle
        nextEventTime = currentTime + distribTestHandlerDelay(genTestHandler);
        return true;
    }
    
    // Descrobe this handler, required of all children of EventHandler
    virtual const std::string describe() override {
        std::string description = "Test EventHandler";
        return description;
    }
};

// Test the way SimClock processes events
bool testSimClock(bool longTest) {
    std::cout << "***** Starting Test of SimClock Class *****" << std::endl;
    
    // Create a SimClock object
    // For a long test run the clock for the default duration (3 hours) which produces a lot of output
    // For a short test run the clock for one minute of simulated time
    SimClock aClock(nullptr, longTest ? defaultTestClockSeconds : secondsPerMinute);
    // Select how many test handlers to use
    int testHandlerCount = longTest ? longTestHandlerCount : shortTestHandlerCount;
    // Construct and add the test event handlers
    for(int i=0; i<testHandlerCount; i++) {
        aClock.addHandler(std::make_shared<TestHandler>(distribTestHandlerDelay(genTestHandler),
                                                        distribTestHandlerRepeat(genTestHandler),i + 1));
    }
    // Run the test
    bool returnValue = aClock.run(true);
    std::cout << std::endl;
    return returnValue;
}
