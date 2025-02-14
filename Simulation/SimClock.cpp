//
//  SimClock.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/20/25.
//
#include <list>
#include "SimClock.hpp"
#include "Simulation.hpp"
#include "RandomGenerators.hpp"

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
theSimulation{theSimulation}, endTime{endTime}, currentTime{0}, needSort{false}, eventHandlers{} {
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

# if SORTED_EVENT_QUEUE_TYPE == 2 // multiset always already sorted
    eventHandlers.insert(EventHandlerSharedPtr(aHandler));
#else
    auto handlerPtr = begin(eventHandlers);
    while(handlerPtr != end(eventHandlers) &&  (*handlerPtr)->getNextEventTime() > aHandler->getNextEventTime()) {
        handlerPtr++;
    }
    eventHandlers.insert(handlerPtr, aHandler);
#endif
}

// One of our handlers thinks that it may be in the wrong order in the vector due to internal changes.
// If we can find the charger in the queue, we remove it and insert it back in, sorting it in the proper
// location. But if the handler is not in the queue than nothing happens. For example, if a handler
// asks us to do this while responding to a handleEvent() call, we already removed it from the vector
// before calling handleEvent() and will add it back when that handleEvent() returns so no need to do anyting now.
void SimClock::reSortHandler(std::shared_ptr<EventHandler> aHandler) {
# if SORTED_EVENT_QUEUE_TYPE == 2 // multiset always already sorted
    // But if the value of handler's nextEventTime changed, it needs to be resorted
    auto handlerPtr = begin(eventHandlers);
    while(handlerPtr != end(eventHandlers)) {
        if(*handlerPtr == aHandler) {
            eventHandlers.erase(handlerPtr);
            addHandler(aHandler);
            return;
        }
        handlerPtr++;
    }
    std::cout << "***error: tried to resort aHandler, but not found in eventHandler list" << std::endl;
    std::cout << "aHandler:" << aHandler->describe() << std::endl;
#else
#if 1 // use find instead of manual search
    auto handlerPtr = find_if(begin(eventHandlers),end(eventHandlers),
                              [aHandler](std::shared_ptr<EventHandler> aHandlerPtr)
                              {return aHandlerPtr == aHandler;});
    if(*handlerPtr == aHandler) {
        eventHandlers.erase(handlerPtr);
        addHandler(aHandler);
    } else {
        std::cout << "***error: tried to resort aHandler, but not found in eventHandler list" << std::endl;
        std::cout << "aHandler:" << aHandler->describe() << std::endl;
    }
#else
    auto handlerPtr = begin(eventHandlers);
    while(handlerPtr != end(eventHandlers)) {
        if(*handlerPtr == aHandler) {
            eventHandlers.erase(handlerPtr);
            addHandler(aHandler);
            return;
        }
        handlerPtr++;
    }
    std::cout << "***error: tried to resort aHandler, but not found in eventHandler list" << std::endl;
    std::cout << "aHandler:" << aHandler->describe() << std::endl;
#endif
#endif
}

// Set internal variable indicating that we need to resort the vector at the start of the next iteration of the run loop
// So far this has not been needed.
void SimClock::markNeedSort()
{
    needSort = true;
}


// For testing: check if the vector is properly sorted
bool SimClock::checkSort() {
# if SORTED_EVENT_QUEUE_TYPE == 2 // multiset always already sorted
    return true;
#else
    return is_sorted(begin(eventHandlers), end (eventHandlers), [](std::shared_ptr<EventHandler> lhs, std::shared_ptr<EventHandler> rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
#endif
}

// Sort the vector of handlers
// This function is private so only this object can call it at times that are safe
void SimClock::sortHandlers() {
#if SORTED_EVENT_QUEUE_TYPE == 0
    sort(begin(eventHandlers), end (eventHandlers), [](std::shared_ptr<EventHandler> lhs, std::shared_ptr<EventHandler> rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
#else // SORTED_EVENT_QUEUE_TYPE == 1 so Use a sort that works for lists and vactors

#if SORTED_EVENT_QUEUE_TYPE == 1
    eventHandlers.sort([](std::shared_ptr<EventHandler> lhs, std::shared_ptr<EventHandler> rhs) {
        return lhs->getNextEventTime() > rhs->getNextEventTime();
    });
#else // SORTED_EVENT_QUEUE_TYPE == 2     // Already always sort
#endif

#endif
}

// Run the actual simulation
bool SimClock::run(bool verbose) {

#if 0 // Use this to force verbose for special testing situations
    verbose = true
#endif
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
            if(verbose) {
                std::cout << std::endl;
                std::cout << "'needSort == true' in simCLock event loop so listing pre-sort status first." << std::endl;
                describeQueue(currentTime);
            }
            sortHandlers();
            needSort = false;
        }
        if(verbose) {
            std::cout << std::endl;
            describeQueue(currentTime);
        }
        // Pull the handler with the lowest next time off the end of the list (kept sorted that way)
#if SORTED_EVENT_QUEUE_TYPE == 2
        auto endPtr = eventHandlers.end();
        endPtr--;
        std::shared_ptr<EventHandler> nextEventHandler = endPtr->theHandler;
#else
        std::shared_ptr<EventHandler> nextEventHandler = eventHandlers.back();
#endif
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
#if SORTED_EVENT_QUEUE_TYPE == 2
        // We already got the pointer to the last item (the one we just processed)earlier
        eventHandlers.erase(endPtr);
#else
        eventHandlers.pop_back();
#endif
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
#if SORTED_EVENT_QUEUE_TYPE == 2
    for(EventHandlerSharedPtr remainingEventHandler: eventHandlers) {
        if(verbose) {
            std::cout << "Close out handleEvent() for " << remainingEventHandler.theHandler->describe() << std::endl;
        }
        // Notify them that this is the final close-out in case they need to do something different
        remainingEventHandler.theHandler->handleEvent(currentTime, true);
    }
#else
    for(std::shared_ptr<EventHandler> remainingEventHandler: eventHandlers) {
        if(verbose) {
            std::cout << "Close out handleEvent() for " << remainingEventHandler->describe() << std::endl;
        }
        // Notify them that this is the final close-out in case they need to do something different
        remainingEventHandler->handleEvent(currentTime, true);
    }
#endif
    return true;
}

// For testing: sum all the planes in all of the handlers
// TO-DO: use this in some testing to ensure that all planes exist througohut the simulation and that a plane is
// never in more than one place at the same time.
long SimClock::countPlanes() {
    long returnValue = 0;
#if SORTED_EVENT_QUEUE_TYPE == 2
    for(auto aHandlerPtr: eventHandlers) {
        auto aHandler = aHandlerPtr.theHandler;
        returnValue += aHandler->countPlanes();
    }
#else
    for(auto aHandler: eventHandlers) {
        returnValue += aHandler->countPlanes();
    }
#endif
    return returnValue;
}

// For testing: describe detailed current state of the vector.
// This is more detailed than the override of the generic describe method
// While describe() gives counts of planes, this lists the handlers.
void SimClock::describeQueue(long currentTime) {
    std::cout << "*** SimClock eventHandler list at " << currentTime << std::endl;
    // List the planes waitint
    if(eventHandlers.size() == 0) {
        std::cout << "No eventHandlers in SimClock" << std::endl;
   } else {
#if SORTED_EVENT_QUEUE_TYPE == 2
    for(auto aHandlerPtr: eventHandlers) {
        auto aHandler = aHandlerPtr.theHandler;
#else
        for(auto aHandler: eventHandlers) {
#endif
           std::cout << "    " << aHandler->describe() << " having next event time " << aHandler->getNextEventTime() << std::endl;
        }
    }
    std::cout << std::endl;
}


/*
 *******************************************************************************************
 * Class TestHandler
 * This is a child of EventHandler. It is a simple event handler for testing.
 *******************************************************************************************
 */
class TestHandler: public EventHandler {
    long repeats; // How many events does this process?
    std::mt19937 *theGenerator;
    std::uniform_int_distribution<> *theDistribution;
public:
    TestHandler(long nextEventTime, int repeatCount, std::mt19937 *aGenerator, std::uniform_int_distribution<> *aDistribution): EventHandler(nextEventTime),repeats{repeatCount}, theGenerator{aGenerator},theDistribution{aDistribution}   {
    };
    virtual ~TestHandler() override {
    };
 
    // Handle a simulated event
    virtual bool handleEvent(long currentTime, bool closeOut) override {
        // Inform the user that this handler is processing an event.
        if(currentTime < nextEventTime) {
            std::cout << "Wanted " << nextEventTime << " but ended early at " << currentTime << " for " << describe() << std::endl;
        } else {
            std::cout << "Handle event at " << currentTime << " for " << describe() << std::endl;
        }
        // If we are done, ask that we be removed from the list of active event handlers
        if(--repeats <= 0) { return false; }
        // Get a delay for the next event we want to handle
        // Set a next Event time
        nextEventTime = currentTime + (*theDistribution)(*theGenerator);
        return true;
    }
    
    // Describe this handler, required of all children of EventHandler
    virtual const std::string describe() override {
        std::string description = "Test EventHandler #" + std::to_string(getHandlerNumber()) +" with " +
        std::to_string(repeats) + " repeats remaining and next event at " + std::to_string(nextEventTime);
        return description;
    }
};

// Test the way SimClock processes events
bool testSimClock(bool longTest) {
    // These are constants to use in the test event handlers
    const int maxRangeDelay{20}; // Test event handlers will delay in a range [2 - maxRangeDelay] seconds after each event
    const int maxTestHandlerRepeat{20}; // Ech test event handler will be asked to process [1 - maxTestHandlerRepeat] events
    const int longTestHandlerCount{25}; // How many test handlers are created for a long test?
    const int shortTestHandlerCount{5}; // How many test handlers are created for a short test?

    std::cout << "***** Starting Test of SimClock Class *****" << std::endl;
    
    // Set up a random number generator and some distributions for the SimClock tests
    RandomGenerators theRandomGenerators(nullptr);
    std::mt19937 *theGenerator = theRandomGenerators.getGenerator(); // cache this since we use it repeatedly
    std::uniform_int_distribution<> distribTestHandlerDelay(2, maxRangeDelay);
    std::uniform_int_distribution<> distribTestHandlerRepeat(1, maxTestHandlerRepeat);

    // Create a SimClock object
    // For a long test run the clock for the default duration (3 hours) which produces a lot of output
    // For a short test run the clock for one minute of simulated time
    SimClock aClock(nullptr, longTest ? defaultTestClockSeconds : secondsPerMinute);
    // Select how many test handlers to use
    int testHandlerCount = longTest ? longTestHandlerCount : shortTestHandlerCount;
    // Construct and add the test event handlers
    for(int i=0; i<testHandlerCount; i++) {
        aClock.addHandler(std::make_shared<TestHandler>(distribTestHandlerDelay(*theGenerator),
                                                        distribTestHandlerRepeat(*theGenerator), theGenerator, &distribTestHandlerDelay));
    }
    // List the contents of the SimClock
    aClock.describeQueue(0);
    // Run the test
    bool returnValue = aClock.run(true);
    std::cout << std::endl;
    return returnValue;
}
