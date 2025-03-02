# Parallel Computing
## Introduction
This repository contains a collection of parallel computing algorithms implemented in C++ using MPI.

The implementations have been tested on a Windows 11 Machine, compiled using MSVC running the C++20 standard and MSMPI v10.0.

## Algorithms
### [Monte Carlo Simulation for Pi](Monte_Carlo/README.md)
### [Matrix Multiplication](Matrix_Multiplication/README.md)

## Running the code
To run the code, you need to have MS-MPI installed on your machine.

After compiling the code, navigate to the output directory and run the following command:
```cmd
mpiexec -n <number_of_processes> <executable_name> <program_name> <program_arguments>
```

For example, to run Monte Carlo Pi estimation with 6 processes and 2e8 iterations:
```cmd
mpiexec -n 6 ParallelComputing.exe monte_carlo 200000000
```