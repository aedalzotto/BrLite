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

BrLite::BrLite(sc_module_name _name, uint8_t _x_size, uint8_t _y_size) :
	sc_module(_name),
	ack_in(_x_size*_y_size),
	req_in(_x_size*_y_size),
	data_in(_x_size*_y_size),
	header_in(_x_size*_y_size),
	ack_out(_x_size*_y_size),
	req_out(_x_size*_y_size),
	data_out(_x_size*_y_size),
	header_out(_x_size*_y_size),
	local_busy(_x_size*_y_size),
	data_in_sig(_x_size*_y_size),
	header_in_sig(_x_size*_y_size),
	req_in_sig(_x_size*_y_size),
	ack_out_sig(_x_size*_y_size),
	data_out_sig(_x_size*_y_size),
	header_out_sig(_x_size*_y_size),
	req_out_sig(_x_size*_y_size),
	ack_in_sig(_x_size*_y_size),
	x_size(_x_size),
	y_size(_y_size)
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

	SC_METHOD(interconnection);
	sensitive << reset;
	for(int i = 0; i < x_size*y_size; i++){
		for(int j = 0; j < NPORT - 1; j++){
			sensitive << data_out_sig[i][j];
			sensitive << header_out_sig[i][j];
			sensitive << req_out_sig[i][j];
			sensitive << ack_in_sig[i][j];

			sensitive << data_in_sig[i][j];
			sensitive << header_in_sig[i][j];
			sensitive << req_in_sig[i][j];
			sensitive << ack_out_sig[i][j];
		}
	}
}

void BrLite::interconnection()
{
	for(int y = 0; y < y_size; y++){
		for(int x = 0; x < x_size; x++){
			if(y != y_size - 1){
				req_in_sig[y*x_size + x][NORTH] = req_out_sig[y*x_size + x + x_size][SOUTH];
				ack_in_sig[y*x_size + x][NORTH] = ack_out_sig[y*x_size + x + x_size][SOUTH];
				header_in_sig[y*x_size + x][NORTH] = header_out_sig[y*x_size + x + x_size][SOUTH];
				data_in_sig[y*x_size + x][NORTH] = data_out_sig[y*x_size + x + x_size][SOUTH];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][NORTH] = 0;
				ack_in_sig[y*x_size + x][NORTH] = 1;
				header_in_sig[y*x_size + x][NORTH] = 0;
				data_in_sig[y*x_size + x][NORTH] = 0;
			}

			if(y != 0){
				req_in_sig[y*x_size + x][SOUTH] = req_out_sig[y*x_size + x - x_size][NORTH];
				ack_in_sig[y*x_size + x][SOUTH] = ack_out_sig[y*x_size + x - x_size][NORTH];
				header_in_sig[y*x_size + x][SOUTH] = header_out_sig[y*x_size + x - x_size][NORTH];
				data_in_sig[y*x_size + x][SOUTH] = data_out_sig[y*x_size + x - x_size][NORTH];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][SOUTH] = 0;
				ack_in_sig[y*x_size + x][SOUTH] = 1;
				header_in_sig[y*x_size + x][SOUTH] = 0;
				data_in_sig[y*x_size + x][SOUTH] = 0;
			}

			if(x != x_size - 1){
				req_in_sig[y*x_size + x][EAST] = req_out_sig[y*x_size + x + 1][WEST];
				ack_in_sig[y*x_size + x][EAST] = ack_out_sig[y*x_size + x + 1][WEST];
				header_in_sig[y*x_size + x][EAST] = header_out_sig[y*x_size + x + 1][WEST];
				data_in_sig[y*x_size + x][EAST] = data_out_sig[y*x_size + x + 1][WEST];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][EAST] = 0;
				ack_in_sig[y*x_size + x][EAST] = 1;
				header_in_sig[y*x_size + x][EAST] = 0;
				data_in_sig[y*x_size + x][EAST] = 0;
			}

			if(x != 0){
				req_in_sig[y*x_size + x][WEST] = req_out_sig[y*x_size + x - 1][EAST];
				ack_in_sig[y*x_size + x][WEST] = ack_out_sig[y*x_size + x - 1][EAST];
				header_in_sig[y*x_size + x][WEST] = header_out_sig[y*x_size + x - 1][EAST];
				data_in_sig[y*x_size + x][WEST] = data_out_sig[y*x_size + x - 1][EAST];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][WEST] = 0;
				ack_in_sig[y*x_size + x][WEST] = 1;
				header_in_sig[y*x_size + x][WEST] = 0;
				data_in_sig[y*x_size + x][WEST] = 0;
			}
		}
	}
}
