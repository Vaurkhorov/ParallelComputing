#include "../programs.h"

#include <mpi.h>
#include <vector>
#include <fstream>

using std::vector;

// These are defaults, overridden if arguments are provided.
constexpr int ROWS = 50;
constexpr int COLUMNS = 50;

constexpr int STEP_INTERVAL = 5; // ms
constexpr int NUM_STEPS = 1000; // total time = STEPS_INTERVAL * NUM_STEPS milliseconds

constexpr double MAX_TEMP = 1000.0;

constexpr double grid_distance = 0.001; // 0.001 meters = 1 mm
constexpr double thermal_diffusivity = 23e-6; // alpha for iron is 2.3 × 10^(-5) m^2/s

constexpr int OPENGL_PREVIEW_FRAME_DELAY = 5; // minimum milliseconds between frames

enum class BoundaryCondition {
	// this was an interesting rabbit hole.
	Neumann,	// no heat loss to boundary
	Dirichlet,	// boundary is absolute zero❅
	//Convective,	// boundary is air~~ 
	// (not implemented because i've had enough of thermodynamics for one day)
	// (actually, enough for a lifetime)
};

double boundary_adjusted_temp(double self_temp, BoundaryCondition condition = BoundaryCondition::Neumann);
void preview_in_txt(const std::vector<std::vector<uint8_t>>& results, int columns);

// THIS IS DISABLED IF USE_OPENGL_FOR_PREVIEW IS NOT DEFINED!!!!!!!!!!!!!!!!
// If you want OpenGL based output, define USE_OPENGL_FOR_PREVIEW in the project settings.
// Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions -> Add USE_OPENGL_FOR_PREVIEW to the end of the list.
void preview_in_gl(const std::vector<std::vector<uint8_t>>& results, int rows, int columns);

void record_interval(vector<double>& grid, vector<vector<uint8_t>>& results, int rows, int columns) {
	results.push_back(vector<uint8_t>(rows * columns));

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			results.back()[i * columns + j] = grid[i * columns + j] / MAX_TEMP * 255;
		}
	}
}

int heat_sim(Arguments args) {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

#ifdef USE_OPENGL_FOR_PREVIEW
	if (rank == 0) {
		std::cout << "OpenGL is ENABLED. The timer may NOT be accurate, and may not work at all." << std::endl;
		std::cout << "Check the README for this program to check how to disable OpenGL." << std::endl;
	}
#endif // USE_OPENGL_FOR_PREVIEW

	int no_of_rows = ROWS;
	int no_of_columns = COLUMNS;
	int step_interval = STEP_INTERVAL;
	int num_steps = NUM_STEPS;

	if (args.len() >= 2) {
		no_of_rows = std::stoi(args.get(0));
		no_of_columns = std::stoi(args.get(1));
	}

	if (args.len() >= 3) {
		step_interval = std::stoi(args.get(2));
	}

	if (args.len() >= 4) {
		num_steps = std::stoi(args.get(3));
	}

	vector<double> grid;

	if (rank == 0) {
		grid.resize(no_of_rows * no_of_columns);	
	}

	vector<int> rows_per_process(world_size);
	vector<int> sendcounts(world_size);
	vector<int> displs(world_size);

	int displ = 0;
	for (int i = 0; i < world_size; i++) {
		rows_per_process[i] = no_of_rows / world_size;
		rows_per_process[i] += (i < no_of_rows % world_size) ? 1 : 0;
		sendcounts[i] = (rows_per_process[i]) * no_of_columns;

		displs[i] = displ;
		displ += sendcounts[i];
	}

	int local_rows = rows_per_process[rank];
	vector<double> local_grid(rows_per_process[rank] * no_of_columns);
	vector<double> local_output;
	vector<double> upper_neighbor(no_of_columns);
	vector<double> lower_neighbor(no_of_columns);

	vector<vector<uint8_t>> results;
	if (rank == 0) record_interval(grid, results, no_of_rows, no_of_columns);

	for (int iteration = 0; iteration < num_steps; iteration++) {
		if (iteration % (num_steps / 10) == 0) {
			if (rank == 0) std::cout << "Approximately " << (double)iteration / num_steps * 100 << "% done." << std::endl;
		}
		if (rank == 0) grid[10 * no_of_columns + 10] = 1000;
		if (rank == 0) grid[40 * no_of_columns + 25] = 1000;

		MPI_Scatterv(grid.data(), sendcounts.data(), displs.data(), MPI_DOUBLE, local_grid.data(), sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

		MPI_Request requests[4];

		if (rank > 0) {
			MPI_Isend(local_grid.data(), no_of_columns, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[0]);
			MPI_Irecv(upper_neighbor.data(), no_of_columns, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[1]);
		}

		if (rank < world_size - 1) {
			MPI_Isend(local_grid.data() + (local_rows - 1) * no_of_columns, no_of_columns, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &requests[2]);
			MPI_Irecv(lower_neighbor.data(), no_of_columns, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &requests[3]);
		}

		int request_start = (rank > 0) ? 2 : 0;
		int request_end = (rank < world_size - 1) ? 2 : 4;

		MPI_Waitall(request_end - request_start, requests + request_start, MPI_STATUSES_IGNORE);

		//local_grid.insert(local_grid.begin(), upper_neighbor.begin(), upper_neighbor.end());
		//local_grid.insert(local_grid.end(), lower_neighbor.begin(), lower_neighbor.end());

		local_output.resize(local_grid.size());

		// by this point, each thread should have its local grid with ghost rows

		bool ghost_row_up = rank > 0;
		bool ghost_row_down = rank < world_size - 1;

		for (int i = 0; i < local_rows; i++) {
			for (int j = 0; j < no_of_columns; j++) {
				double current_temp = local_grid[i * no_of_columns + j];
				double adjusted = boundary_adjusted_temp(current_temp);
				// for now, i'm not handling different boundary conditions for different sides

				double left_temp = (j > 0) ? local_grid[i * no_of_columns + j - 1] : adjusted;
				double right_temp = (j < no_of_columns - 1) ? local_grid[i * no_of_columns + j + 1] : adjusted;

				double up_temp = adjusted;
				if (i > 0) {
					up_temp = local_grid[(i - 1) * no_of_columns + j];
				}
				else if (ghost_row_up) {
					up_temp = upper_neighbor[j];
				}

				double down_temp = adjusted;
				if (i < local_rows - 1) {
					down_temp = local_grid[(i + 1) * no_of_columns + j];
				}
				else if (ghost_row_down) {
					down_temp = lower_neighbor[j];
				}

				bool flag = current_temp == 100;

				double new_temp =
					current_temp +
					(
						(thermal_diffusivity * (step_interval / 1000.0) / (grid_distance * grid_distance))
						*
						(left_temp + right_temp + up_temp + down_temp - 4 * current_temp)
					);

				if (flag) std::cout << new_temp << "=" << thermal_diffusivity * step_interval / (grid_distance * grid_distance) << '*' << (left_temp + right_temp + up_temp + down_temp - 4 * current_temp) << std::endl;

				if (new_temp > MAX_TEMP) {
					new_temp = MAX_TEMP;
				}
				else if (new_temp < 0) {
					new_temp = 0;
				}

				//if (iteration == 0) {
				//	if (new_temp != 0 && new_temp > 500) {
				//		std::cout << "Iteration " << iteration << ", " << i << ", " << j << ": " << new_temp << std::endl;
				//	}
				//}

				local_output[i * no_of_columns + j] = new_temp;
			}
		}

		MPI_Gatherv(local_output.data() + no_of_columns * (int)ghost_row_up, local_rows * no_of_columns, MPI_DOUBLE, grid.data(), sendcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

		if (rank == 0) grid[10 * no_of_columns + 10] = 1000;
		if (rank == 0) grid[40 * no_of_columns + 25] = 1000;

		if (rank == 0) record_interval(grid, results, no_of_rows, no_of_columns);
	}

#ifdef USE_OPENGL_FOR_PREVIEW
	if (rank == 0) preview_in_gl(results, no_of_rows, no_of_columns);
#else
	if (rank == 0) preview_in_txt(results, no_of_columns);
#endif

	return 0;
}

double boundary_adjusted_temp(double self_temp, BoundaryCondition condition) {
	switch (condition) {
	case BoundaryCondition::Neumann:
		return self_temp;
	case BoundaryCondition::Dirichlet:
		return 0;
	/*case BoundaryCondition::Convective:
		return '☺'*/
	default:
		return self_temp;
	}
}

void preview_in_txt(const std::vector<std::vector<uint8_t>>& results, int columns) {
	std::string filename = "heat_sim_results.txt";
	std::ofstream file(filename);
	if (!file) {
		std::cerr << "Error opening file for writing.\n";
		return;
	}

	for (size_t i = 0; i < results.size(); i++) {
		file << "Vector " << i << ":\n";
		for (size_t j = 0; j < results[i].size(); j++) {
			if (results[i][j]) file << std::setw(4) << static_cast<int>(results[i][j]);
			else file << "....";
			//file << "\t";

			if ((j + 1) % columns == 0) file << "\n";
		}
		file << "\n"; 
	}

	file.close();
}

#ifdef USE_OPENGL_FOR_PREVIEW
#include <GL/glut.h>
#include <GL/GL.h>

#include <chrono>
#include <thread>

using namespace std;

void init();
void plot(const vector<vector<uint8_t>>& results, int rows, int columns);

class Renderer {
// because 'raw' global variables are bad or something?
public:
	static void set(const vector<vector<uint8_t>>& results, int rows, int columns) {
		Renderer::results = results;
		Renderer::rows = rows;
		Renderer::columns = columns;
	};

	static void call() {
		plot(results, rows, columns);
	};

	static vector<vector<uint8_t>> results;
	static int rows;
	static int columns;
};

vector<vector<uint8_t>> Renderer::results;
int Renderer::rows = 0;
int Renderer::columns = 0;

int result_size = 0;
int current_result = 0;

void preview_in_gl(const vector<vector<uint8_t>>& results, int rows, int columns) {
	int mode = GLUT_DOUBLE | GLUT_RGB;

	int fake_argc = 1;
	char* fake_argv[] = { (char*)"ParallelProcessing.exe", nullptr };
	glutInit(&fake_argc, fake_argv);

	Renderer::set(results, rows, columns);

	result_size = results.size();
	current_result = 0;

	glutInitDisplayMode(mode);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("heiß oder kalt?");
	init();
	glutDisplayFunc(Renderer::call);
	glutIdleFunc(Renderer::call);
	glutMainLoop();
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glPointSize(20);
	glLineWidth(1.0);
	glViewport(0, 0, Renderer::columns, Renderer::rows);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, Renderer::columns, 0, Renderer::rows);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
}

void plot(const vector<vector<uint8_t>>& results, int rows, int columns) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			glColor3ub(results[current_result][i * columns + j], 0, 0);
			glVertex2i(j, i);
		}
	}

	glEnd();
	current_result = (current_result + 1) % result_size;

	// this is some double buffering magic. all i know is that it makes everything work.
	glutSwapBuffers();
	glutPostRedisplay();
	std::this_thread::sleep_for(std::chrono::milliseconds(OPENGL_PREVIEW_FRAME_DELAY));
}

#endif // USE_OPENGL_FOR_PREVIEW