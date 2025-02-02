# Monte Carlo Simulation for Pi
## Introduction
This program runs a Monte Carlo simulation for estimating the value of π.

## Arguments
This program takes a single integer argument, the number of iterations to run the simulation.

## Running the code
Use this command:
```cmd
mpiexec -n <number_of_processes> <executable_name> monte_carlo <program_arguments>
```

Example:
```cmd
mpiexec -n 12 ParallelComputing.exe monte_carlo 200000000
```

## Results
This table shows the results of the program running on my machine, running the default number of iterations each time.

| Number of Processes | Slowest Thread Time (s) | Total Time Taken (s) |
|---------------------|------------------------|----------------------|
| 12 (default)       | 4.17990                 | 4.18163             |
| 48                 | 3.48702                 | 3.84104             |
| 60                 | 3.39870                 | 3.68278             |
| 120                | 2.26636                 | 3.75843             |
