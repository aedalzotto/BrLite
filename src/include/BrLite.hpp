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
	std::vector<sc_in<uint32_t>> payload_in;
	std::vector<sc_in<uint32_t>> address_in;
	std::vector<sc_in<uint8_t>> id_svc_in;

	std::vector<sc_out<bool>> ack_out;
	std::vector<sc_out<bool>> req_out;
	std::vector<sc_out<uint32_t>> payload_out;
	std::vector<sc_out<uint32_t>> address_out;
	std::vector<sc_out<uint8_t>> id_svc_out;

	std::vector<sc_out<bool>> local_busy;

	SC_HAS_PROCESS(BrLite);
	BrLite(sc_module_name _name, uint8_t _x_size, uint8_t _y_size);

private:
	std::vector<BrLiteRouter*> routers;

	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> payload_in_sig;
	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> address_in_sig;
	std::vector<std::array<sc_signal<uint8_t>, NPORT - 1>> id_svc_in_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	req_in_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	ack_out_sig;

	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> payload_out_sig;
	std::vector<std::array<sc_signal<uint32_t>, NPORT - 1>> address_out_sig;
	std::vector<std::array<sc_signal<uint8_t>, NPORT - 1>> id_svc_out_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	req_out_sig;
	std::vector<std::array<sc_signal<bool>, NPORT - 1>> 	ack_in_sig;

	uint8_t x_size;
	uint8_t y_size;

	void interconnection();
};
