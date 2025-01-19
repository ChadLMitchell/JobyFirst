#  Ideas for Joby First Project

This is to capture observations and ideas as they occur.

Jan 17
Received the assignment this evening, delayed due to a mail error from Joby HR.
Asked confirmation of the C++ version currently used there.
Also asked for clarification about the term "faults" to see if there is a standard meaning.
    Some options:
    Faults could be minor and just be tracked.
    Faults could mean that plane no longer operates for the duration of the test.
    Faults could cause shorter-term interruptions in service.

Other initial thoughts:
    Allow user interface to specify duration of simulation, passenger plan, distirbution of aircraft, number of chargers
    Passengers could be always full or have a distribution across capacity
    All planes could start at the same time or some could start later.
        Assume all planes start at the same time for this exercise.
    Do we provide a singe call user interface to perform a simulation or an object created and then set up?
        Possibly provide both user interfaces
        Do the object oriented version first. Then add a simpler user interface on top of it as an option.
    When we create an opject for a plane, we probably want to pre-calculate some things to speed up the simulation
    We could do the simulation with multi-tasking, but that would add a lot of work and debugging
        Instead, set up a time-slice architecture where a simulation dispatcher has actions listed with times and does them as they happen
    When a plane is created, probably set up in advance a random vector of faults that will happen over the duration of the test
    Assume each plane fully recharges before flying again.
            We could simulate allowing planes to do partial recharges,
            but we would need to have a relationship between % charge time and % flight time

Next steps:
    Define a general simulation architecture
    Need Specifications, planes, overall time-slice driver, passengers, flight? (plane+time+passengers), faults
    Calculate how long each plane type can go on a full charge
    
Assumptions
    Since flying is much more stress on the plane than idling or charging, assume faults only happen during a flight.
        So the interval to the next fault only includes time in flight.
    All planes start in flights with no delay for starting some planes.
        All planes start in a queue of planes to fly next so they could have a delay, but for now assume the delay is 0.
    Seconds is fine enough ressolution for this simulation so the clock can do seconds allowing mostly integer arithmetic.
    
