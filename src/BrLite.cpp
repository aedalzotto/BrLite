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
	sc_module(_name),
	ack_in(x_size*y_size),
	req_in(x_size*y_size),
	data_in(x_size*y_size),
	header_in(x_size*y_size),
	ack_out(x_size*y_size),
	req_out(x_size*y_size),
	data_out(x_size*y_size),
	header_out(x_size*y_size),
	local_busy(x_size*y_size),
	bool_gnd((x_size*2 + y_size*2)*2),
	word_gnd((x_size*2 + y_size*2)*2)
{
	routers.reserve(x_size*y_size);

	for(int y = 0; y < y_size; y++)
		for(int x = 0; x < x_size; x++)
			routers.push_back(new BrLiteRouter("router", x << 4 | y));

	static uint16_t bool_idx = 0;
	static uint16_t word_idx = 0;
	for(int y = 0; y < y_size; y++){
		for(int x = 0; x < x_size; x++){
			routers[y*x_size + x]->clock(clock);
			routers[y*x_size + x]->reset(reset);

			if(y != y_size - 1){
				routers[y*x_size + x]->req_in[NORTH](routers[y*x_size + x + x_size]->req_out[SOUTH]);
				routers[y*x_size + x]->ack_in[NORTH](routers[y*x_size + x + x_size]->ack_out[SOUTH]);
				routers[y*x_size + x]->data_in[NORTH](routers[y*x_size + x + x_size]->data_out[SOUTH]);
				routers[y*x_size + x]->header_in[NORTH](routers[y*x_size + x + x_size]->header_out[SOUTH]);
			} else {
				/* Ground signals */
				routers[y*x_size + x]->req_in[NORTH](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->ack_in[NORTH](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->data_in[NORTH](word_gnd[word_idx++]);
				routers[y*x_size + x]->header_in[NORTH](word_gnd[word_idx++]);
			}

			if(y != 0){
				routers[y*x_size + x]->req_in[SOUTH](routers[y*x_size + x - x_size]->req_out[NORTH]);
				routers[y*x_size + x]->ack_in[SOUTH](routers[y*x_size + x - x_size]->ack_out[NORTH]);
				routers[y*x_size + x]->data_in[SOUTH](routers[y*x_size + x - x_size]->data_out[NORTH]);
				routers[y*x_size + x]->header_in[SOUTH](routers[y*x_size + x - x_size]->header_out[NORTH]);
			} else {
				/* Ground signals */
				routers[y*x_size + x]->req_in[SOUTH](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->ack_in[SOUTH](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->data_in[SOUTH](word_gnd[word_idx++]);
				routers[y*x_size + x]->header_in[SOUTH](word_gnd[word_idx++]);
			}

			if(x != x_size - 1){
				routers[y*x_size + x]->req_in[EAST](routers[y*x_size + x + 1]->req_out[WEST]);
				routers[y*x_size + x]->ack_in[EAST](routers[y*x_size + x + 1]->ack_out[WEST]);
				routers[y*x_size + x]->data_in[EAST](routers[y*x_size + x + 1]->data_out[WEST]);
				routers[y*x_size + x]->header_in[EAST](routers[y*x_size + x + 1]->header_out[WEST]);
			} else {
				/* Ground signals */
				routers[y*x_size + x]->req_in[EAST](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->ack_in[EAST](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->data_in[EAST](word_gnd[word_idx++]);
				routers[y*x_size + x]->header_in[EAST](word_gnd[word_idx++]);
			}

			if(x != 0){
				routers[y*x_size + x]->req_in[WEST](routers[y*x_size + x - 1]->req_out[EAST]);
				routers[y*x_size + x]->ack_in[WEST](routers[y*x_size + x - 1]->ack_out[EAST]);
				routers[y*x_size + x]->data_in[WEST](routers[y*x_size + x - 1]->data_out[EAST]);
				routers[y*x_size + x]->header_in[WEST](routers[y*x_size + x - 1]->header_out[EAST]);
			} else {
				/* Ground signals */
				routers[y*x_size + x]->req_in[WEST](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->ack_in[WEST](bool_gnd[bool_idx++]);
				routers[y*x_size + x]->data_in[WEST](word_gnd[word_idx++]);
				routers[y*x_size + x]->header_in[WEST](word_gnd[word_idx++]);
			}

			routers[y*x_size + x]->req_out[LOCAL](req_out[y*x_size + x]);
			routers[y*x_size + x]->ack_out[LOCAL](ack_out[y*x_size + x]);
			routers[y*x_size + x]->data_out[LOCAL](data_out[y*x_size + x]);
			routers[y*x_size + x]->header_out[LOCAL](header_out[y*x_size + x]);

			routers[y*x_size + x]->req_in[LOCAL](req_in[y*x_size + x]);
			routers[y*x_size + x]->ack_in[LOCAL](ack_in[y*x_size + x]);
			routers[y*x_size + x]->data_in[LOCAL](data_in[y*x_size + x]);
			routers[y*x_size + x]->header_in[LOCAL](header_in[y*x_size + x]);

			routers[y*x_size + x]->local_busy(local_busy[y*x_size + x]);
		}
	}

	for(int i = 0; i < (x_size*2 + y_size*2)*2; i++){
		bool_gnd[i] = 0;
		word_gnd[i] = 0;
	}
}
