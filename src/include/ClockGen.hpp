/**
 * MA-Memphis
 * @file ClockGen.hpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Clock and reset generator
 */

#pragma once

#include <systemc.h>

SC_MODULE(ClockGen){
public:
	sc_out<bool> clock;
	sc_out<bool> reset;

	SC_HAS_PROCESS(ClockGen);
	ClockGen(sc_module_name _name);

private:
	void clock_gen();
	void reset_gen();
};
