#include "../programs.h"

#include <mpi.h>
#include <string>
#include <random>
#include <cmath>
#include <numbers>
#include <vector>

constexpr int MATRIX_ROWS = 1000;
constexpr int MATRIX_COLUMNS = 1000;

int matrix_multiplication(Arguments args) {
	using std::vector;

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	std::random_device rd;
	std::mt19937 rng(rd() ^ rank * RANDOM_SEED_MULTIPLIER);
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	vector<double> matrix_a;
	vector<double> matrix_b;

	matrix_a.resize(MATRIX_ROWS * MATRIX_COLUMNS);
	matrix_b.resize(MATRIX_ROWS * MATRIX_COLUMNS);

	if (rank == 0) {
		for (int i = 0; i < MATRIX_ROWS; i++) {
			for (int j = 0; j < MATRIX_COLUMNS; j++) {
				matrix_a[i * MATRIX_COLUMNS + j] = dist(rng);
				matrix_b[i * MATRIX_COLUMNS + j] = dist(rng);
			}
		}
	}

	if (rank == 0) {
		std::cout << "Matrix A:" << std::endl;
		MatrixTools::peek_at_matrix_vec_flattened<double, MATRIX_ROWS, MATRIX_COLUMNS>(matrix_a, 5);
		std::cout << "Matrix B:" << std::endl;
		MatrixTools::peek_at_matrix_vec_flattened<double, MATRIX_ROWS, MATRIX_COLUMNS>(matrix_b, 5);
		std::cout << std::endl;
	}

	MPI_Bcast(matrix_b.data(), MATRIX_ROWS * MATRIX_COLUMNS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	vector<int> sendcounts;
	vector<int> displs;

	int displ = 0;
	int extra_rows = MATRIX_ROWS % world_size;
	for (int i = 0; i < world_size; i++) {
		int rows = MATRIX_ROWS / world_size;
		if (i < extra_rows) {
			rows++;
		}

		sendcounts.push_back(rows * MATRIX_COLUMNS);
		displs.push_back(displ);
		displ += rows * MATRIX_COLUMNS;
	}

	vector<double> local_matrix_a(sendcounts[rank]);
	MPI_Scatterv(matrix_a.data(), sendcounts.data(), displs.data(), MPI_DOUBLE, local_matrix_a.data(), sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	vector<double> local_output(sendcounts[rank], 0);

	//std::cout << "Rank " << rank << " has " << sendcounts[rank] / MATRIX_COLUMNS << " rows." << std::endl;

	for (int i = 0; i < sendcounts[rank] / MATRIX_COLUMNS; i++) {
		for (int j = 0; j < MATRIX_COLUMNS; j++) {
			for (int k = 0; k < MATRIX_COLUMNS; k++) {
				if (i * MATRIX_COLUMNS + j >= sendcounts[rank]) {
					std::cout << 1 << std::endl;
				}
				if (i * MATRIX_COLUMNS + k >= sendcounts[rank]) {
					std::cout << 2 << std::endl;
				}
				if (i * MATRIX_COLUMNS + j >= MATRIX_ROWS * MATRIX_COLUMNS) {
					std::cout << 3 << std::endl;
				}
				local_output[i * MATRIX_COLUMNS + j] += local_matrix_a[i * MATRIX_COLUMNS + k] * matrix_b[k * MATRIX_COLUMNS + j];
			}
		}
	}
	vector<double> output(MATRIX_ROWS * MATRIX_COLUMNS);
	MPI_Gatherv(local_output.data(), sendcounts[rank], MPI_DOUBLE, output.data(), sendcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		std::cout << "Output:" << std::endl;
		MatrixTools::peek_at_matrix_vec_flattened<double, MATRIX_ROWS, MATRIX_COLUMNS>(output, 5);
	}

	return 0;
}