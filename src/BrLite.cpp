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
 * @brief Light BrNoC -- Router interconnection
 */

#include "BrLite.hpp"

BrLite::BrLite(sc_module_name _name, uint8_t _x_size, uint8_t _y_size) :
	sc_module(_name),
	ack_in(_x_size*_y_size),
	req_in(_x_size*_y_size),
	payload_in(_x_size*_y_size),
	address_in(_x_size*_y_size),
	id_svc_in(_x_size*_y_size),
	ack_out(_x_size*_y_size),
	req_out(_x_size*_y_size),
	payload_out(_x_size*_y_size),
	address_out(_x_size*_y_size),
	id_svc_out(_x_size*_y_size),
	local_busy(_x_size*_y_size),
	payload_in_sig(_x_size*_y_size),
	address_in_sig(_x_size*_y_size),
	id_svc_in_sig(_x_size*_y_size),
	req_in_sig(_x_size*_y_size),
	ack_out_sig(_x_size*_y_size),
	payload_out_sig(_x_size*_y_size),
	address_out_sig(_x_size*_y_size),
	id_svc_out_sig(_x_size*_y_size),
	req_out_sig(_x_size*_y_size),
	ack_in_sig(_x_size*_y_size),
	x_size(_x_size),
	y_size(_y_size)
{
	routers.reserve(x_size*y_size);

	for(int y = 0; y < y_size; y++){
		for(int x = 0; x < x_size; x++){
			char name[255];
			snprintf(name, 255, "router_%x", x << 8 | y);
			routers.push_back(new BrLiteRouter(name, x << 8 | y));
		}
	}

	for(int y = 0; y < y_size; y++){
		for(int x = 0; x < x_size; x++){
			for(int i = 0; i < NPORT - 1; i++){
				routers[y*x_size + x]->payload_out[i](payload_out_sig[y*x_size+x][i]);
				routers[y*x_size + x]->address_out[i](address_out_sig[y*x_size+x][i]);
				routers[y*x_size + x]->id_svc_out[i](id_svc_out_sig[y*x_size+x][i]);
				routers[y*x_size + x]->req_out[i](req_out_sig[y*x_size+x][i]);
				routers[y*x_size + x]->ack_in[i](ack_in_sig[y*x_size+x][i]);

				routers[y*x_size + x]->payload_in[i](payload_in_sig[y*x_size+x][i]);
				routers[y*x_size + x]->address_in[i](address_in_sig[y*x_size+x][i]);
				routers[y*x_size + x]->id_svc_in[i](id_svc_in_sig[y*x_size+x][i]);
				routers[y*x_size + x]->req_in[i](req_in_sig[y*x_size+x][i]);
				routers[y*x_size + x]->ack_out[i](ack_out_sig[y*x_size+x][i]);
			}
			routers[y*x_size + x]->clock(clock);
			routers[y*x_size + x]->reset(reset);

			routers[y*x_size + x]->req_out[LOCAL](req_out[y*x_size + x]);
			routers[y*x_size + x]->ack_out[LOCAL](ack_out[y*x_size + x]);
			routers[y*x_size + x]->payload_out[LOCAL](payload_out[y*x_size + x]);
			routers[y*x_size + x]->address_out[LOCAL](address_out[y*x_size + x]);
			routers[y*x_size + x]->id_svc_out[LOCAL](id_svc_out[y*x_size + x]);

			routers[y*x_size + x]->req_in[LOCAL](req_in[y*x_size + x]);
			routers[y*x_size + x]->ack_in[LOCAL](ack_in[y*x_size + x]);
			routers[y*x_size + x]->payload_in[LOCAL](payload_in[y*x_size + x]);
			routers[y*x_size + x]->address_in[LOCAL](address_in[y*x_size + x]);
			routers[y*x_size + x]->id_svc_in[LOCAL](id_svc_in[y*x_size + x]);

			routers[y*x_size + x]->local_busy(local_busy[y*x_size + x]);
		}
	}

	SC_METHOD(interconnection);
	sensitive << reset;
	for(int i = 0; i < x_size*y_size; i++){
		for(int j = 0; j < NPORT - 1; j++){
			sensitive << payload_out_sig[i][j];
			sensitive << address_out_sig[i][j];
			sensitive << id_svc_out_sig[i][j];
			sensitive << req_out_sig[i][j];
			sensitive << ack_in_sig[i][j];

			sensitive << payload_in_sig[i][j];
			sensitive << address_in_sig[i][j];
			sensitive << id_svc_in_sig[i][j];
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
				address_in_sig[y*x_size + x][NORTH] = address_out_sig[y*x_size + x + x_size][SOUTH];
				payload_in_sig[y*x_size + x][NORTH] = payload_out_sig[y*x_size + x + x_size][SOUTH];
				id_svc_in_sig[y*x_size + x][NORTH] = id_svc_out_sig[y*x_size + x + x_size][SOUTH];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][NORTH] = 0;
				ack_in_sig[y*x_size + x][NORTH] = 1;
				address_in_sig[y*x_size + x][NORTH] = 0;
				payload_in_sig[y*x_size + x][NORTH] = 0;
				id_svc_in_sig[y*x_size + x][NORTH] = 0;
			}

			if(y != 0){
				req_in_sig[y*x_size + x][SOUTH] = req_out_sig[y*x_size + x - x_size][NORTH];
				ack_in_sig[y*x_size + x][SOUTH] = ack_out_sig[y*x_size + x - x_size][NORTH];
				address_in_sig[y*x_size + x][SOUTH] = address_out_sig[y*x_size + x - x_size][NORTH];
				payload_in_sig[y*x_size + x][SOUTH] = payload_out_sig[y*x_size + x - x_size][NORTH];
				id_svc_in_sig[y*x_size + x][SOUTH] = id_svc_out_sig[y*x_size + x - x_size][NORTH];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][SOUTH] = 0;
				ack_in_sig[y*x_size + x][SOUTH] = 1;
				address_in_sig[y*x_size + x][SOUTH] = 0;
				payload_in_sig[y*x_size + x][SOUTH] = 0;
				id_svc_in_sig[y*x_size + x][SOUTH] = 0;
			}

			if(x != x_size - 1){
				req_in_sig[y*x_size + x][EAST] = req_out_sig[y*x_size + x + 1][WEST];
				ack_in_sig[y*x_size + x][EAST] = ack_out_sig[y*x_size + x + 1][WEST];
				address_in_sig[y*x_size + x][EAST] = address_out_sig[y*x_size + x + 1][WEST];
				payload_in_sig[y*x_size + x][EAST] = payload_out_sig[y*x_size + x + 1][WEST];
				id_svc_in_sig[y*x_size + x][EAST] = id_svc_out_sig[y*x_size + x + 1][WEST];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][EAST] = 0;
				ack_in_sig[y*x_size + x][EAST] = 1;
				address_in_sig[y*x_size + x][EAST] = 0;
				payload_in_sig[y*x_size + x][EAST] = 0;
				id_svc_in_sig[y*x_size + x][EAST] = 0;
			}

			if(x != 0){
				req_in_sig[y*x_size + x][WEST] = req_out_sig[y*x_size + x - 1][EAST];
				ack_in_sig[y*x_size + x][WEST] = ack_out_sig[y*x_size + x - 1][EAST];
				address_in_sig[y*x_size + x][WEST] = address_out_sig[y*x_size + x - 1][EAST];
				payload_in_sig[y*x_size + x][WEST] = payload_out_sig[y*x_size + x - 1][EAST];
				id_svc_in_sig[y*x_size + x][WEST] = id_svc_out_sig[y*x_size + x - 1][EAST];
			} else {
				/* Ground signals */
				req_in_sig[y*x_size + x][WEST] = 0;
				ack_in_sig[y*x_size + x][WEST] = 1;
				address_in_sig[y*x_size + x][WEST] = 0;
				payload_in_sig[y*x_size + x][WEST] = 0;
				id_svc_in_sig[y*x_size + x][WEST] = 0;
			}
		}
	}
}
