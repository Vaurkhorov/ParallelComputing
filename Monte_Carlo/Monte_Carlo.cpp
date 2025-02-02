#include "../programs.h"

#include <mpi.h>
#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <numbers>

int monte_carlo(Arguments args) {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int total_no_of_iterations = (args.has(0)) ? atoi(args.get(0)) : (int)1e8;
	int iterations_per_thread = total_no_of_iterations / world_size;

	if (rank == world_size - 1) {
		iterations_per_thread += total_no_of_iterations % world_size;
	}

	std::random_device rd;
	std::mt19937 rng(rd() ^ rank * RANDOM_SEED_MULTIPLIER);
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	double inside_circle = 0;
	double factor = 4 / (double)total_no_of_iterations;

	Timer timer;
	while (iterations_per_thread--) {
		double x = dist(rng);
		double y = dist(rng);
		double distance = x * x + y * y;
		if (distance <= 1) {
			inside_circle++;
		}
	}
	timer.stop();

	double total_inside_circle;
	MPI_Reduce(&inside_circle, &total_inside_circle, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	double local_time_taken = timer.duration().count();
	double max_time_taken;
	MPI_Reduce(&local_time_taken, &max_time_taken, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		double pi = total_inside_circle * factor;
		std::cout << "PI: " << pi << std::endl;
		std::cout << "Accuracy: " << 100 - (std::abs(std::numbers::pi - pi) / std::numbers::pi) * 100 << "%" << std::endl;
		std::cout << "Slowest thread took " << max_time_taken << " seconds to compute." << std::endl;
	}

	return 0;
}