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
