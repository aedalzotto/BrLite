#include <systemc.h>

#include "ClockGen.hpp"
#include "Testbench.hpp"

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> clock;
	sc_signal<bool> reset;

	ClockGen clock_gen("clock_gen");
	clock_gen.clock(clock);
	clock_gen.reset(reset);

	Testbench testbench("testbench", 8, 8);

	testbench.clock(clock);
	testbench.reset(reset);

	sc_start(2000, SC_NS);
  	return 0;
}
