# Matrix Multiplication
## Introduction
This program multiplies two randomly generated 2D matrices.

## Running the code
Use this command:
```cmd
mpiexec -n <number_of_processes> <executable_name> matrix_m
```

Example:
```cmd
mpiexec -n 12 ParallelComputing.exe matrix_m
```

## Results
This table shows the results of the program running on my machine, multiplying two 1000×1000 matrices each time.

| Number of Processes | Total Time Taken (s) |
|---------------------|----------------------|
| 1                   | 61.6495              |
| 6                   | 12.4177              |
| 12 (default)        | 9.70210              |
| 24                  | 9.60900              |
| 36                  | 10.0642              |
| 48                  | 10.6329              |
