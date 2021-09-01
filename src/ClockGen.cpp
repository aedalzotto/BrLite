/**
 * MA-Memphis
 * @file ClockGen.cpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Clock and reset generator
 */

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
