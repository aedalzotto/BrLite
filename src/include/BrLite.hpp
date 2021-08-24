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
 * @brief Light BrNoC
 */

#include <systemc.h>

#include <BrLiteRouter.hpp>

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
	BrLite(sc_module_name _name, uint8_t x_size, uint8_t y_size);

private:
	std::vector<BrLiteRouter*> routers;

	std::vector<sc_signal<bool>> 		bool_gnd;
	std::vector<sc_signal<uint32_t>>	word_gnd;
};
