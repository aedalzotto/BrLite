#include "ClockGen.hpp"

ClockGen::ClockGen(sc_module_name _name) :
	sc_module(_name)
{
	SC_THREAD(clock_gen);
	SC_THREAD(reset_gen);
};

void ClockGen::clock_gen()
{
	while(true){
		clock = 0;
		wait(5, SC_NS);
		clock = 1;
		wait(5, SC_NS);
	}
}

void ClockGen::reset_gen()
{
	reset = 1;
	wait(100, SC_NS);
	reset = 0;
}
