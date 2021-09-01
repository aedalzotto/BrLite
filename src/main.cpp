/**
 * MA-Memphis
 * @file main.cpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Main SystemC simulation file for Light BrNoC
 */

#include <systemc.h>

#include "ClockGen.hpp"
#include "Testbench.hpp"

#include "../scenario.h"

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> clock;
	sc_signal<bool> reset;

	ClockGen clock_gen("clock_gen");
	clock_gen.clock(clock);
	clock_gen.reset(reset);

	Testbench testbench("testbench", X_SIZE, Y_SIZE);

	testbench.clock(clock);
	testbench.reset(reset);

	sc_start(200, SC_MS);
  	return 0;
}
