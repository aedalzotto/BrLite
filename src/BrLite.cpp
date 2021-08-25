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
	data_in_sig(x_size*y_size),
	header_in_sig(x_size*y_size),
	req_in_sig(x_size*y_size),
	ack_out_sig(x_size*y_size),
	data_out_sig(x_size*y_size),
	header_out_sig(x_size*y_size),
	req_out_sig(x_size*y_size),
	ack_in_sig(x_size*y_size)
{
	routers.reserve(x_size*y_size);

	for(int y = 0; y < y_size; y++){
		for(int x = 0; x < x_size; x++){
			char name[255];
			snprintf(name, 255, "router_%x", x << 4 | y);
			routers.push_back(new BrLiteRouter(name, x << 4 | y));
		}
	}

	for(int y = 0; y < y_size; y++){
		for(int x = 0; x < x_size; x++){
			for(int i = 0; i < NPORT - 1; i++){
				routers[y*x_size + x]->data_out[i](data_out_sig[y*x_size+x][i]);
				routers[y*x_size + x]->header_out[i](header_out_sig[y*x_size+x][i]);
				routers[y*x_size + x]->req_out[i](req_out_sig[y*x_size+x][i]);
				routers[y*x_size + x]->ack_in[i](ack_in_sig[y*x_size+x][i]);

				routers[y*x_size + x]->data_in[i](data_in_sig[y*x_size+x][i]);
				routers[y*x_size + x]->header_in[i](header_in_sig[y*x_size+x][i]);
				routers[y*x_size + x]->req_in[i](req_in_sig[y*x_size+x][i]);
				routers[y*x_size + x]->ack_out[i](ack_out_sig[y*x_size+x][i]);

			}
			routers[y*x_size + x]->clock(clock);
			routers[y*x_size + x]->reset(reset);

			// if(y != y_size - 1){
			// 	routers[y*x_size + x]->req_in[NORTH](routers[y*x_size + x + x_size]->req_out[SOUTH]);
			// 	routers[y*x_size + x]->ack_in[NORTH](routers[y*x_size + x + x_size]->ack_out[SOUTH]);
			// 	routers[y*x_size + x]->data_in[NORTH](routers[y*x_size + x + x_size]->data_out[SOUTH]);
			// 	routers[y*x_size + x]->header_in[NORTH](routers[y*x_size + x + x_size]->header_out[SOUTH]);
			// } else {
			// 	/* Ground signals */
			// 	routers[y*x_size + x]->req_in[NORTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_in[NORTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_in[NORTH](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_in[NORTH](word_gnd[word_idx++]);

			// 	routers[y*x_size + x]->req_out[NORTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_out[NORTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_out[NORTH](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_out[NORTH](word_gnd[word_idx++]);
			// }

			// if(y != 0){
			// 	routers[y*x_size + x]->req_in[SOUTH](routers[y*x_size + x - x_size]->req_out[NORTH]);
			// 	routers[y*x_size + x]->ack_in[SOUTH](routers[y*x_size + x - x_size]->ack_out[NORTH]);
			// 	routers[y*x_size + x]->data_in[SOUTH](routers[y*x_size + x - x_size]->data_out[NORTH]);
			// 	routers[y*x_size + x]->header_in[SOUTH](routers[y*x_size + x - x_size]->header_out[NORTH]);
			// } else {
			// 	/* Ground signals */
			// 	routers[y*x_size + x]->req_in[SOUTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_in[SOUTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_in[SOUTH](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_in[SOUTH](word_gnd[word_idx++]);

			// 	routers[y*x_size + x]->req_out[SOUTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_out[SOUTH](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_out[SOUTH](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_out[SOUTH](word_gnd[word_idx++]);
			// }

			// if(x != x_size - 1){
			// 	routers[y*x_size + x]->req_in[EAST](routers[y*x_size + x + 1]->req_out[WEST]);
			// 	routers[y*x_size + x]->ack_in[EAST](routers[y*x_size + x + 1]->ack_out[WEST]);
			// 	routers[y*x_size + x]->data_in[EAST](routers[y*x_size + x + 1]->data_out[WEST]);
			// 	routers[y*x_size + x]->header_in[EAST](routers[y*x_size + x + 1]->header_out[WEST]);
			// } else {
			// 	/* Ground signals */
			// 	routers[y*x_size + x]->req_in[EAST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_in[EAST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_in[EAST](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_in[EAST](word_gnd[word_idx++]);

			// 	routers[y*x_size + x]->req_out[EAST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_out[EAST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_out[EAST](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_out[EAST](word_gnd[word_idx++]);
			// }

			// if(x != 0){
			// 	routers[y*x_size + x]->req_in[WEST](routers[y*x_size + x - 1]->req_out[EAST]);
			// 	routers[y*x_size + x]->ack_in[WEST](routers[y*x_size + x - 1]->ack_out[EAST]);
			// 	routers[y*x_size + x]->data_in[WEST](routers[y*x_size + x - 1]->data_out[EAST]);
			// 	routers[y*x_size + x]->header_in[WEST](routers[y*x_size + x - 1]->header_out[EAST]);
			// } else {
			// 	/* Ground signals */
			// 	routers[y*x_size + x]->req_in[WEST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_in[WEST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_in[WEST](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_in[WEST](word_gnd[word_idx++]);

			// 	routers[y*x_size + x]->req_out[WEST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->ack_out[WEST](bool_gnd[bool_idx++]);
			// 	routers[y*x_size + x]->data_out[WEST](word_gnd[word_idx++]);
			// 	routers[y*x_size + x]->header_out[WEST](word_gnd[word_idx++]);
			// }

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
}
