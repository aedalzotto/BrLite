#include <systemc.h>

#include "ClockGen.hpp"
#include "Testbench.hpp"

int sc_main(int argc, char *argv[])
{
	ClockGen clock_gen("clock_gen");
	Testbench testbench("testbench", 8, 8);

	testbench.clock(clock_gen.clock);
	testbench.reset(clock_gen.reset);

	sc_start(2000, SC_NS);
  	return 0;
}
