# Heat Simulation
## Introduction
This program runs a 2D heat simulation. It *can* visualise the results using OpenGL.

## OpenGL
By default, OpenGL is not enabled. The output is written to a text file 'heat_sim_results.txt' in the program directory. To enable OpenGL, you'll have to add `USE_OPENGL_FOR_PREVIEW` to the preprocessor definitions.

If you're using Visual Studio, this can be done by going to project properties -> C/C++ -> Preprocessor -> Preprocessor Definitions.

You'll also have to add glut to path. I was using [freeglut](https://freeglut.sourceforge.net) during testing.

## Result
![Simulation Preview](Heat_Sim.gif)
*Normally results go at the end, but can you blame me for wanting to show this off?*

## Principle
Equation used to incrementally calculate temperature:

![Heat Equation](Heat_Equation.svg)

The Courant-Friedrichs-Lewy condition and von Neumann stability analysis yields the conditions necessary for a stable simulation.
Luckily for me, then, the example used on wikipedia for the von Neumann condition used exactly the scenario I'm using.
> [https://en.wikipedia.org/wiki/Von_Neumann_stability_analysis#Illustration_of_the_method](https://en.wikipedia.org/wiki/Von_Neumann_stability_analysis#Illustration_of_the_method) 
> 
> Page accessed on 3 March 2025.
> 
> *I may or may not have just yanked the equations from there. I don't like thermodynamics too much. Or differental equations. Or thinking too hard.*

## Arguments
This program accepts command line arguments:
1. number of rows
2. number of columns
3. duration of intervals (in ms) between increments
4. number of iterations to simulate
> Currently, all arguments are positional and optional.
> 
> If you want to specify the number of rows, you'll also have to specify the number of columns. If only the number of rows is provided, it will be ignored.

## Running the code
Use this command:
```cmd
mpiexec [-n <number_of_processes>] <executable_name> heat_sim [<program_arguments>]
```

Example:
```cmd
mpiexec -n 12 ParallelComputing.exe heat_sim 102 20 3 878
```