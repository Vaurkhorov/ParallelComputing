#pragma once
#include "helpers.h"
#include <map>
#include <string>

enum class Program {
	None = 0,

	Monte_Carlo,
	Matrix_Multiplication,
	Heat_Sim
};

const std::map<std::string, Program> PROGRAM_NAMES = {
	std::make_pair("none", Program::None),
	std::make_pair("monte_carlo", Program::Monte_Carlo),
	std::make_pair("matrix_m", Program::Matrix_Multiplication),
	std::make_pair("heat_sim", Program::Heat_Sim),
};

// programs
int monte_carlo(Arguments args);
int matrix_multiplication(Arguments args);
int heat_sim(Arguments args);