//
//  Plane.cpp
//  JobyFirstProject
//
//  Created by Chad Mitchell on 1/18/25.
//

#include <random>
#include "Plane.hpp"
#include "Passenger.hpp"

// We would eventually put the plane specifications into their own object to manage them
// for now, they are here. Later, in their own object, we could add the abilty to read them from a file.
// For now, if any of this changes, make sure you change the constants at the top of SimSettings that
// need to match them. And if we read them from a file some of the constants would become variables.

// For output purposes, here are the "names" of the comapnies
const char *companyNames[]{
    "Alpha Company",
    "Bravo Company",
    "Charlie Company",
    "Delta Company",
    "Echo Company"
};

// A global utility function to show the name of a company from a Company enum value
// Show company name with error checking
const char *companyName(Company c) {
    if(c < minCompany || c > maxCompany) {
        return "Invalid Company Name";
    }
    return companyNames[c];
}

// Here are the specifications we were given for this project.
PlaneSpecification planeSpecifications[]{
//    Company theCompany;
//    double cruise_speed__mph;
//    double battery_capacity__kWh;
//    double time_to_charge__hours;
//    double energy_use__kWh_per_mile;
//    long passenger_count;
//    double probability_fault__per_hour;
    PlaneSpecification{Company::Alpha, 120, 320, 0.6, 1.6, 4, 0.25},
    PlaneSpecification{Company::Bravo, 100, 100, 0.2, 1.5, 5, 0.1},
    PlaneSpecification{Company::Charlie, 160, 220, 0.8, 2.2, 3, 0.05},
    PlaneSpecification{Company::Delta, 90, 120, 0.62, 0.8, 2, 0.22},
    PlaneSpecification{Company::Echo, 30, 150, 0.3, 5.8, 2, 0.61}
};

// Each plane is assigned a plane number (mostly for testing)
int Plane::NextPlaneNumber = 1; // We use this static member to keep track of next number to assign
Plane::Plane(PlaneSpecification &spec): mySpecs(spec) {
    // To aoid divide by 0 and other silly errors
    // we should validate specs before creating Plane, this is extra checking
    if(!validateSpecs(mySpecs)) {
        throw std::runtime_error("Attempt to create plane with invalid PlaneSpecifications");
    }
    planeNumber = NextPlaneNumber++;
    // Use the failure rate to randomly select when this plane will next see a fault
    // Every Plane object knows when it will next see a fault.
    createFaultInterval();
}
Plane::~Plane() {
}

// For testing: get the Company enum assigned to this plane
Company Plane::getCompany(){
    return mySpecs.theCompany;
}

// For testing: get the name of the company assigned
const char *Plane::getCompanyName(){
    return companyName(mySpecs.theCompany);
}

// For testing: get the plane number
int Plane::getPlaneNumber() {
    return planeNumber;
}

// For testing: provide a description of this plane
const std::string Plane::describe(){
    std::string description = "plane #" + std::to_string(planeNumber) + " from " + getCompanyName();
    return description;
}

// Get from the specification
double Plane::getMilesPerHour() {
    return mySpecs.cruise_speed__mph;
}

// Some of the calculations below could be pre-calculated for better effeciency,
// but a good compiler will pre-calculate them anyway so it may not make much
// difference.

// Calculate from the specifications
long Plane::calcTimeToCharge__seconds() {
    // Change it from hours to seconds
    return lround(mySpecs.time_to_charge__hours * secondsPerHourD);
}

// Calculate from the specifications
double Plane::calcDistanceFullCharge__miles() {
    // Distance on a full charge is the battery capacity in kWh divided by
    // the energy used per mile in kWh.
    return mySpecs.battery_capacity__kWh / mySpecs.energy_use__kWh_per_mile;
}

// Calculate from the specifications
long Plane::calcTimeOnFullCharge__seconds() {
    // Time on a full charge is the distance on a full charge divided by the
    // speed. But since the speed is mph we need to covert hours to seconds also.
    return lround(calcDistanceFullCharge__miles()* secondsPerHourD / mySpecs.cruise_speed__mph);
}

// Calculate from the specifications
double Plane::calcMeanTimeBetweenFaults() {
    // MTBF is the invese of the probabilty of fault. ALso convert hours to seconds.
    return secondsPerHourD/mySpecs.probability_fault__per_hour;
}

// Get from the specification
long Plane::getMaxPassengerCount() {
    return mySpecs.passenger_count;
}

// Get the current interval of flight time to the next fault
long Plane::getNextFaultInterval() {
    return nextFaultInterval;
}

// After completing a flight, we reduce the interval to the next fault
// by the time of the interval used up during the flight
long Plane::decrementNextFaultInterval(long seconds) {
    nextFaultInterval -= seconds;
    return seconds;
}

// Across a population we can create the effect of a fault per time rate by having each object
// randomly assigned an interval to its next fault in a way that matches the distribution.
// This function does that calcuation. Knwoing in advance the next fault interval for a plane
// allow our quantum clock to work because we do not need to keep chaecking if each plane has
// a fault this second. Instead, we just queue up future faults and process them when they come.
// It works because the fault per hour probabiilty is still honored across the population as long
// as every plane is assigned the an intervaly according to the right random distribution.
long Plane::createFaultInterval() {
    std::random_device rd;
    std::mt19937 gen(rd());
    // First generate a random real number between 0 and 1
    std::uniform_real_distribution<> distrib(0, 1);
    double random0to1 = distrib(gen);
    // We then take the ln (natural logarithm) of that number and divide it by the fault rate
    // But ln(0) is infinity so we avoid the occasional very small number
    if (random0to1 < 0.001) { random0to1 = 0.001; };
    // Now we do our calcuation. The math is with real numbers, but then we need an integral number of seconds
    nextFaultInterval = lround(-std::log(random0to1)* secondsPerHourD / mySpecs.probability_fault__per_hour);
    // Truncating to integer value may result in 0 if interval is very small, so avoid instant (0) intervals.
    // Instead make sure the result is at least 1.
    if(nextFaultInterval <= 0) {
        nextFaultInterval = 1;
    }
    return nextFaultInterval;
}

// This is to validate that the specifications are reasonable
// Later, particularly if we load them from a file, we need to
// do more rigorous validation.
bool Plane::validateSpecs(PlaneSpecification &spec) {
    bool returnValue{true};
    if(spec.cruise_speed__mph <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.battery_capacity__kWh <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.time_to_charge__hours <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.energy_use__kWh_per_mile <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.passenger_count <= 0) {
        returnValue = false;
        // log error
    }
    if(spec.energy_use__kWh_per_mile <= 0) {
        returnValue = false;
        // log error
    }
    return returnValue;
}

// Generate a plane randomly from the kids allowed. This is only used for testing
// and does not take into account the minPlanePerKind setting
std::shared_ptr<Plane> Plane::getRandomPlane() {
    Company randCompany = allCompany[rand() % (maxCompany + 1)];
    return std::make_shared<Plane>(planeSpecifications[randCompany]);
}

// This tests some functionality of the Plane class. The main thing is to validate the approach to fault distribution produces valid results.
// For each company specifications and using those, this repeatedly calculates a random fault interval using createFaultInterval().
// It then takes an average of all of those fault intervals. That should match the fault intervale we are asked to use.
// This repors an error if there is a significant (more than allowedPercentDiffFromMTBF perentage difference).
bool testPlaneClassObjects(bool verbose) {
    const double allowedPercentDiffFromMTBF{3.0}; // How much percentage can our calculated average be from the specifiction
    const double tries = 10000; // How many fault intervals do we calculate for each company

    bool returnValue = true;
    if(verbose) {
        std::cout << " ***** Starting Test of Plane Class  *****" << std::endl;
        std::cout << "Validating specificastions and simulation of time to next fault" << std::endl;
    }
    for(PlaneSpecification aSpec: planeSpecifications) {
        if(!Plane::validateSpecs(aSpec)) {
            if(verbose) {
                std::cout << companyName(aSpec.theCompany) << " has invalid specification" << std::endl;
            }
            returnValue = false;
        } else {
            double accumFaultCount = 0; // this will accumulate a total of all fault intervals generated
            Plane testPlane {Plane(aSpec)}; // Get a plane
            for (auto tryCount = 0; tryCount < tries; tryCount++) {
                accumFaultCount += testPlane.createFaultInterval(); // Recreate its fault interval "tries" times
            }
            double averageFaultInterval = accumFaultCount/tries; // Get the average fault interval
            if(verbose) { // Display the spec and the average of all the intervals
                std::cout  << companyName(aSpec.theCompany) << " MTBF from Spec: " << testPlane.calcMeanTimeBetweenFaults() << " Average Simulated Fault Interval (seconds): " << averageFaultInterval << std::endl;
            }
            // Get the percentage difference between the calculated average MTBF and the target in the specifications
            double percentDiff = abs(testPlane.calcMeanTimeBetweenFaults() - averageFaultInterval)/testPlane.calcMeanTimeBetweenFaults() * 100;
            if(percentDiff > allowedPercentDiffFromMTBF) {
                // If there is a difference, report an error
                if(verbose) {
                    std::cout << "Error: " << companyName(aSpec.theCompany) << " Difference between Simulated Average Interval and MTBF: " << percentDiff << "% > " << allowedPercentDiffFromMTBF << "% (allowed difference)" << std::endl;
                }
                returnValue = false;
            }
        }
    }
    return returnValue;
}
