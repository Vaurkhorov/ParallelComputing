# Parallel Computing
## Introduction
This repository contains a collection of parallel computing algorithms implemented in C++ using MPI.

The implementations have been tested on a Windows 11 Machine, compiled using MSVC running the C++20 standard and MSMPI v10.0.

## Preview
![Heat Simulation](Heat/Heat_Sim.gif)
*Simulation of diffusion of heat in iron.*

## Programs
#### [Monte Carlo Simulation for Pi](Monte_Carlo/README.md)
#### [Matrix Multiplication](Matrix_Multiplication/README.md)
#### [Heat Simulation](Heat/README.md) | Read the doc before trying to run this one, or you might be a little disappointed.

## Running the code
To run the code, you need to have MS-MPI installed on your machine.

After compiling the code, navigate to the output directory and run the following command:
```cmd
mpiexec [-n <number_of_processes>] <executable_name> <program_name> [<program_arguments>]
```

For a quick interactive preview without arguments, you can run:
```cmd
mpiexec [-n <number_of_processes>] <executable_name>
```

Or if you don't want to run it parallelly, just:
```cmd
<executable_name>
```
Though I'm not sure why you'd want to do that. 

You can set the number of processes to 1 if you want to compare performance based on number of threads.

To run, say, Monte Carlo Pi estimation with 6 processes and 2e8 iterations:
```cmd
mpiexec -n 6 ParallelComputing.exe monte_carlo 200000000
```