/**
 * MA-Memphis
 * @file BrLite.cpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Light BrNoC
 */

#include <BrLite.hpp>

BrLite::BrLite(sc_module_name _name, uint8_t x_size, uint8_t y_size) :
	sc_module(_name)
{
	static std::vector<BrLiteRouter> 			routers;
	static std::vector<sc_signal<bool> > 		bool_gnd;
	static std::vector<sc_signal<uint32_t> > 	word_gnd;

	for(int y = 0; y < y_size; y++)
		for(int x = 0; x < x_size; x++)
			routers.push_back(BrLiteRouter("router", x << 4 | y));

	for(int y = 0; y < y_size; y++){
		for(int x = 0; x < x_size; x++){
			routers[y*x_size + x].clock(clock);
			routers[y*x_size + x].reset(reset);

			if(y != y_size - 1){
				routers[y*x_size + x].req_in[NORTH](routers[y*x_size + x + x_size].req_out[SOUTH]);
				routers[y*x_size + x].ack_in[NORTH](routers[y*x_size + x + x_size].ack_out[SOUTH]);
				routers[y*x_size + x].data_in[NORTH](routers[y*x_size + x + x_size].data_out[SOUTH]);
				routers[y*x_size + x].header_in[NORTH](routers[y*x_size + x + x_size].header_out[SOUTH]);
			} else {
				/* Create signal and ground it */
				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].req_in[NORTH](bool_gnd.back());
				bool_gnd.back() = 0;

				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].ack_in[NORTH](bool_gnd.back());
				bool_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].data_in[NORTH](word_gnd.back());
				word_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].header_in[NORTH](word_gnd.back());
				word_gnd.back() = 0;
			}

			if(y != 0){
				routers[y*x_size + x].req_in[SOUTH](routers[y*x_size + x - x_size].req_out[NORTH]);
				routers[y*x_size + x].ack_in[SOUTH](routers[y*x_size + x - x_size].ack_out[NORTH]);
				routers[y*x_size + x].data_in[SOUTH](routers[y*x_size + x - x_size].data_out[NORTH]);
				routers[y*x_size + x].header_in[SOUTH](routers[y*x_size + x - x_size].header_out[NORTH]);
			} else {
				/* Create signal and ground it */
				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].req_in[SOUTH](bool_gnd.back());
				bool_gnd.back() = 0;

				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].ack_in[SOUTH](bool_gnd.back());
				bool_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].data_in[SOUTH](word_gnd.back());
				word_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].header_in[SOUTH](word_gnd.back());
				word_gnd.back() = 0;
			}

			if(x != x_size - 1){
				routers[y*x_size + x].req_in[EAST](routers[y*x_size + x + 1].req_out[WEST]);
				routers[y*x_size + x].ack_in[EAST](routers[y*x_size + x + 1].ack_out[WEST]);
				routers[y*x_size + x].data_in[EAST](routers[y*x_size + x + 1].data_out[WEST]);
				routers[y*x_size + x].header_in[EAST](routers[y*x_size + x + 1].header_out[WEST]);
			} else {
				/* Create signal and ground it */
				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].req_in[EAST](bool_gnd.back());
				bool_gnd.back() = 0;

				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].ack_in[EAST](bool_gnd.back());
				bool_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].data_in[EAST](word_gnd.back());
				word_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].header_in[EAST](word_gnd.back());
				word_gnd.back() = 0;
			}

			if(x != 0){
				routers[y*x_size + x].req_in[WEST](routers[y*x_size + x - 1].req_out[EAST]);
				routers[y*x_size + x].ack_in[WEST](routers[y*x_size + x - 1].ack_out[EAST]);
				routers[y*x_size + x].data_in[WEST](routers[y*x_size + x - 1].data_out[EAST]);
				routers[y*x_size + x].header_in[WEST](routers[y*x_size + x - 1].header_out[EAST]);
			} else {
				/* Create signal and ground it */
				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].req_in[WEST](bool_gnd.back());
				bool_gnd.back() = 0;

				bool_gnd.push_back(sc_signal<bool>());
				routers[y*x_size + x].ack_in[WEST](bool_gnd.back());
				bool_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].data_in[WEST](word_gnd.back());
				word_gnd.back() = 0;

				word_gnd.push_back(sc_signal<uint32_t>());
				routers[y*x_size + x].header_in[WEST](word_gnd.back());
				word_gnd.back() = 0;
			}

			req_out.push_back(sc_out<bool>());
			routers[y*x_size + x].req_out[LOCAL](req_out.back());
			ack_out.push_back(sc_out<bool>());
			routers[y*x_size + x].ack_out[LOCAL](ack_out.back());
			data_out.push_back(sc_out<uint32_t>());
			routers[y*x_size + x].data_out[LOCAL](data_out.back());
			header_out.push_back(sc_out<uint32_t>());
			routers[y*x_size + x].header_out[LOCAL](header_out.back());

			req_in.push_back(sc_in<bool>());
			routers[y*x_size + x].req_in[LOCAL](req_in.back());
			ack_in.push_back(sc_in<bool>());
			routers[y*x_size + x].ack_in[LOCAL](ack_in.back());
			data_in.push_back(sc_in<uint32_t>());
			routers[y*x_size + x].data_in[LOCAL](data_in.back());
			header_in.push_back(sc_in<uint32_t>());
			routers[y*x_size + x].header_in[LOCAL](header_in.back());
		}
	}
}
