# Joby eVTOL Simulation Project

## Overview

This project implements a simulation of electric vertical takeoff and landing (eVTOL) aircraft operations, designed for Joby's job interview process. The simulation runs at a one-second granularity but uses a quantum simulation technique to only advance the simulation clock during meaningful events, significantly improving performance.

## Project Structure

### Directories

- **Simulation**: Core simulation implementation code
- **Interface**: Header files required to run the simulation
- **JobyFirstProject**: Command-line menu solution for testing and configuration
- **Support Files**: Additional project documentation and resources

## Compilation Requirements

- Compatible with C++11 or later
- Tested on MacOS and Windows
- Some compilers may require moving files from Simulation, Interface, and JobyFirstProject into the same directory

Note: The files are in separate directories to later facilitate the Simulation and Interface directories being used to create a library or module.

## Simulation Settings

### Configurable Parameters

| Setting | Default | Description |
|---------|---------|-------------|
| **Simulation Duration** | 3 hours | Maximum duration (seconds) of simulated events |
| **Charger Count** | 3 | Simultaneous charging stations available |
| **Plane Count** | 20 | Number of planes in simulation |
| **Minimum Planes Per Kind** | 1 | Guaranteed minimum planes per each company type |
| **Passenger Count** | Fill maximum | Plane occupancy strategy |
| **Maximum Passenger Delay** | 0 | Waiting time (seconds) for passenger readiness |
| **Fault Handling** | Log only | Plane operation response to faults |

### Passenger Count Options
- **Option 0**: Maximum passenger capacity
- **Option 1 or**: Random 1-to-max passenger count

### Passenger Delay Explaination
- **If == 0**: Planes fly as soon as ready
- **If > 0**: Ready planes wait random interval up to this long for passengers


### Fault Handling Options
- **Option 0**: Log faults only, no affect on plane operations
- **Option 1**: Immediate grounding of plane when fault detected
- **Option 2**: Complete current flight, then ground plane

## Performance

- Typical 3-hour simulation: 300-800 microseconds
- Stress test up to 35,040 hours (4 years): 1-2 seconds
- Tested up to 300,000 hours with potential integer limitation considerations

## Running the Project

1. Compile the project
2. Execute the program
3. Navigate through command-line menus
4. Select simulation options:
   - Sub-menu to adjust settings
   - Single execution
   - Multiple execution averaging
   - Verbose debugging mode
   - Sub-menu for some unit tests
   - Several levels of stress tests
