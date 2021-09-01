/**
 * MA-Memphis
 * @file BrLite.hpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Light BrNoC -- Router interconnection
 */

#pragma once

#include <vector>
#include <systemc.h>

#include "BrLiteRouter.hpp"

SC_MODULE(BrLite){
public:
	sc_in<bool>	clock;
	sc_in<bool>	reset;

	std::vector<sc_in<bool>> ack_in;
	std::vector<sc_in<bool>> req_in;
	std::vector<sc_in<uint32_t>> data_in;
	std::vector<sc_in<uint32_t>> header_in;

	std::vector<sc_out<bool>> ack_out;
	std::vector<sc_out<bool>> req_out;
	std::vector<sc_out<uint32_t>> data_out;
	std::vector<sc_out<uint32_t>> header_out;

	std::vector<sc_out<bool>> local_busy;

	SC_HAS_PROCESS(BrLite);
	BrLite(sc_module_name _name, uint8_t _x_size, uint8_t _y_size);

private:
	std::vector<BrLiteRouter*> routers;

	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> data_in_sig;
	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> header_in_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	req_in_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	ack_out_sig;

	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> data_out_sig;
	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> header_out_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	req_out_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	ack_in_sig;

	uint8_t x_size;
	uint8_t y_size;

	void interconnection();
};
