#include "programs.h"

#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	Arguments args(argc, argv);

	int rc = 0;

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	bool is_main_thread = rank == 0;

	Program program = Program::None;

	if (is_main_thread) {
		if (args.has_program_name()) {
			program = PROGRAM_NAMES.at(args.program());
		}
		else {
			std::cout << "No arguments provided. Choose one:" << std::endl;

			for (auto kv : PROGRAM_NAMES) {
				std::cout << kv.first << std::endl;
			}

			std::string program_name;
			std::cin >> program_name;

			program = PROGRAM_NAMES.at(program_name);
		}
	}

	MPI_Bcast(&program, 1, MPI_INT, 0, MPI_COMM_WORLD);

	Timer overall_time;

	switch (program) {
	case Program::Monte_Carlo:
		if (is_main_thread) std::cout << "Running Monte Carlo:" << std::endl;
		rc = monte_carlo(args);
		break;
	case Program::Matrix_Multiplication:
		if (is_main_thread) std::cout << "Running Matrix Multiplication:" << std::endl;
		rc = matrix_multiplication(args);
		break;
	default:
		if (is_main_thread) std::cout << "Invalid program selected." << std::endl;
		break;
	}

	if (is_main_thread) std::cout << "Total time taken: " << overall_time.stop() << std::endl;
	MPI_Finalize();
	
	return rc;
}