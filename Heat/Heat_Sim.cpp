#include "../programs.h"

#include <mpi.h>
#include <vector>

using std::vector;

char symbolise(int heat, int target);
void pretty_print_grid(vector<vector<int>>& grid, int target);

int heat_sim(Arguments args) {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	vector<vector<int>> grid;

	if (rank == 0) {
		for (int i = 0; i < 10; i++) {
			vector<int> row;
			for (int j = 0; j < 10; j++) {
				row.push_back(0);
			}
			grid.push_back(row);
		}

		grid[1][1] = 1000;

		pretty_print_grid(grid, 1000);
	}

	return 0;
}

char symbolise(int heat, int target) {
	char output = '.';
	if ((double)heat / target > 0.75) {
		output = '#';
	}
	else if ((double)heat / target > 0.5) {
		output = '+';
	}
	else if ((double)heat / target > 0.25) {
		output = '-';
	}

	return output;
}

void pretty_print_grid(vector<vector<int>>& grid, int target) {
	for (vector<int>& row : grid) {
		for (int& cell : row) {
			std::cout << symbolise(cell, target);
		}
		std::cout << std::endl;
	}
}