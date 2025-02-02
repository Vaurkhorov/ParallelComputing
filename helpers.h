﻿#pragma once

#include <chrono>

// 3878 is my r0ll numb3r. 3 at the end of it makes it prime. ¯\_(ツ)_/¯
const int RANDOM_SEED_MULTIPLIER = 3878'3;

/**
* Wrapper class for command line arguments.
*/
class Arguments {
public:
	const int EXECUTABLE_NAME = 0;
	const int PROGRAM_NAME = 1;

	Arguments(int argc, char* argv[]){
		m_argv = argv;
		if (argc <= 1) {
			m_argc = 1;
			m_argument_index = 1;
			m_no_of_arguments = 0;
		}
		else {
			m_argc = argc;
			m_argument_index = 2;
			m_no_of_arguments = argc - 2;
		}
	}

	bool has_program_name() {
		return m_argc > 1;
	}

	char* program() {
		if (has_program_name()) {
			return m_argv[PROGRAM_NAME];
		}
		return m_argv[EXECUTABLE_NAME];
	}

	/**
	* @return The number of arguments passed to the program.
	*/
	int len() {
		return m_no_of_arguments;
	}

	/**
	* @param index The index of the argument.
	* @return Whether there is an argument at the given index.
	*/
	bool has(int index) {
		return index < m_no_of_arguments;
	}

	/**
	* @param index The index of the argument.
	* @return The argument at the given index.
	*/
	char* get(int index) {
		return m_argv[index + m_argument_index];
	}
private:
	int m_argc;
	char** m_argv;
	int m_argument_index;
	int m_no_of_arguments;
};


/**
* Timer class for benchmarking.
*/
class Timer {
public:
	/**
	* Starts the timer.
	*/
	Timer() {
		m_start_time = std::chrono::high_resolution_clock::now();
		m_duration = std::chrono::duration<double>(0);
	}

	/**
	* Stops the timer.
	*/
	std::chrono::duration<double> stop() {
		m_duration = std::chrono::high_resolution_clock::now() - m_start_time;
		m_completed = true;

		return m_duration;
	}

	/**
	* @return The duration of the timer.
	*/
	std::chrono::duration<double> duration() {
		if (completed()) return m_duration;

		return std::chrono::high_resolution_clock::now() - m_start_time;
	}

	/**
	* @return Whether the timer has completed.
	*/
	bool completed() {
		return m_completed;
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
	bool m_completed = false;
	std::chrono::duration<double> m_duration;
};