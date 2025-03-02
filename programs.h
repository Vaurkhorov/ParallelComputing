#pragma once
#include "helpers.h"
#include <map>
#include <string>

enum class Program {
	None = 0,

	Monte_Carlo,
	Matrix_Multiplication,
};

const std::map<std::string, Program> PROGRAM_NAMES = {
	std::make_pair("none", Program::None),
	std::make_pair("monte_carlo", Program::Monte_Carlo),
	std::make_pair("matrix_m", Program::Matrix_Multiplication),
};

// programs
int monte_carlo(Arguments args);
int matrix_multiplication(Arguments args);
