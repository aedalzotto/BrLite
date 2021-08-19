/**
 * MA-Memphis
 * @file BrLiteRouter.cpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Light BrNoC router module -- Removed backtrack (unicast)
 */

#include <BrLiteRouter.hpp>
#include <iostream>

BrLiteRouter::BrLiteRouter(sc_module_name _name, uint8_t _address) :
	sc_module(_name),
	router_address(_address)
{
	SC_METHOD(input);
	sensitive << clock.pos();

	SC_METHOD(output);
	sensitive << clock.pos();

	SC_METHOD(input_output);
	sensitive << clock.pos();
}

void BrLiteRouter::input()
{
	if(reset){
		for(int i = 0; i < NPORT; i++)
			ack_out[i] = 0;
		
		for(int i = 0; i < BRLITE_CAM_SIZE; i++)
			data_table[i] = 0;

		in_state = IN_INIT;
		selected_port = LOCAL;
		source_idx = 0;
		return;
	}

	bool is_in_table;
	uint8_t port;
	uint8_t source_in;
	uint8_t svc;
	uint16_t id_in;

	switch(in_state){
		case IN_INIT:
			if(!clear_local){
				bool has_request = 0;
				for(int i = 0; i < NPORT; i++)
					has_request |= req_in[i];
				
				if(has_request)
					in_state = IN_ARBITRATION;
			}
			break;
		case IN_ARBITRATION:
			while(port != selected_port){
				if(req_in[port]){
					selected_port = port;
					break;
				}
				port++;
				port %= NPORT;
			}

			in_state = IN_TEST_SPACE;
			break;
		case IN_TEST_SPACE:
			port = (selected_port + 1) % NPORT;
			is_in_table = false;
			source_in = (header_in[selected_port] >> 10) & 0xFF;
			id_in = (header_in[selected_port] >> 18) & 0x3FFF;
			for(int i = 0; i < BRLITE_CAM_SIZE; i++){
				if(used_table[i]){
					uint8_t source_table = (header_table[i] >> 10) & 0xFF;
					uint16_t id_table = (header_table[i] >> 18) & 0x3FFF;

					if(id_table == id_in && source_in == source_table){
						is_in_table = true;
						source_idx = i;
						break;
					}
				}
			}

			svc = header_in[selected_port] & 0x3;
			if(!is_in_table && (svc == SVC_TGT || svc == SVC_ALL)){
				/* Message not in CAM */
				bool table_full = true;
				for(int i = 0; i < BRLITE_CAM_SIZE; i++){
					if(!used_table[i]){
						table_full = false;
						free_idx = i;
						break;
					}
				}

				if(table_full){
					std::cout << "++++++++++++++++++++++++++++++++++  CAM CHEIA  SEND LOCAL:  Address: " << router_address << std::endl;
					in_state = IN_INIT;
				} else {
					in_state = IN_WRITE;
				}
			} else if(is_in_table && svc == SVC_CLEAR){
				in_state = IN_CLEAR;
			} else {
				ack_out[selected_port] = 1;
				in_state = IN_WAIT_REQ_DOWN;
			}
			break;
		case IN_WAIT_REQ_DOWN:
			if(!req_in[selected_port]){
				ack_out[selected_port] = 0;
				in_state = IN_INIT;
			}
			break;
		case IN_WRITE:
			data_table[free_idx] = data_in[selected_port];
			input_table[free_idx] = selected_port;

			ack_out[selected_port] = 1;

			in_state = IN_WAIT_REQ_DOWN;
			break;
		case IN_CLEAR:
			ack_out[selected_port] = 1;
			in_state = IN_WAIT_REQ_DOWN;
			break;
	}
}

void BrLiteRouter::output()
{
	static std::array<bool,	NPORT>	ack_ports;

	if(reset){
		for(int i = 0; i < NPORT; i++){
			ack_ports[i] = 0;
			req_out[i] = 0;
		}
		
		selected_line = BRLITE_CAM_SIZE - 1;
		out_state = OUT_INIT;
		return;
	}

	uint8_t line;
	uint8_t svc;
	uint8_t target;
	bool ack;

	switch(out_state){
		case OUT_INIT:
			if(!clear_local){
				bool has_pending = false;
				uint8_t pending_idx = 0;
				for(int i = 0; i < BRLITE_CAM_SIZE; i++){
					if(used_table[i] && pending_table[i]){
						has_pending = true;
						break;
					}
				}

				if(has_pending)
					out_state = OUT_ARBITRATION;
			}
			break;
		case OUT_ARBITRATION:
			line = (selected_line + 1) % BRLITE_CAM_SIZE;
			while(line != selected_line){
				if(used_table[line] & pending_table[line]){
					selected_line = line;
					break;
				}
				line++;
				line %= BRLITE_CAM_SIZE;
			}

			out_state = OUT_TEST_SVC;
			break;
		case OUT_TEST_SVC:
			svc = header_table[selected_line] & 0x3;
			target = (header_table[selected_line] >> 2) & 0xFF;

			if(svc == SVC_CLEAR || svc == SVC_ALL || (svc == SVC_TGT && target != router_address)){
				/* Propagate */
				for(int i = 0; i < NPORT - 1; i++){
					if(i != input_table[selected_line]){
						req_out[i] = 1;
						header_out[i] = header_table[selected_line];
						data_out[i] = data_table[selected_line];
					} else {
						ack_ports[i] = 1;	/* Ack on the port the packet has entered */
					}
				}

				uint8_t source = (header_table[selected_line] >> 10) & 0xFF;
				if((svc == SVC_ALL && source != router_address) || (svc == SVC_TGT && target == router_address)){
					req_out[LOCAL] = 1;
					header_out[LOCAL] = header_table[selected_line];
					data_out[LOCAL] = data_table[selected_line];
				} else {
					ack_ports[LOCAL] = 1;
				}

				out_state = OUT_WAIT_ACK_PORTS;
			} else if(svc == SVC_TGT && target == router_address){
				req_out[LOCAL] = 1;
				header_out[LOCAL] = header_table[selected_line];
				data_out[LOCAL] = data_table[selected_line];
				
				out_state = OUT_SEND_LOCAL;
			} else {
				out_state = OUT_INIT;
			}
			break;
		case OUT_SEND_LOCAL:
			if(ack_in[LOCAL]){
				uint8_t source = (header_table[selected_line] >> 10) & 0xFF;
				uint8_t src_x = source >> 4;
				uint8_t src_y = source & 0xF;
				uint8_t target = (header_table[selected_line] >> 2) & 0xFF;
				uint8_t tgt_x = target >> 4;
				uint8_t tgt_y = target & 0xF;
				uint8_t svc = header_table[selected_line] & 0x3;
				std::cout << ">>>>>>>>>>>>>>>>> SEND LOCAL: [[" << 
					src_x << " " << src_y << " " <<
					tgt_x << " " << tgt_y << "]] " << 
					svc << " " <<
					data_table[selected_line] << " " <<
					"Address: " << router_address << 
					std::endl;
			}

			if(ack_in[LOCAL]){
				req_out[LOCAL] = 0;
				out_state = OUT_WAIT_ACK_DOWN;
			}

			break;
		case OUT_WAIT_ACK_PORTS:
			ack = 1;
			for(int i = 0; i < NPORT; i++){
				if(ack_in[i]){
					ack_ports[i] = 1;
					req_out[i] = 0;
				} else {
					ack &= ack_ports[i];
				}
			}

			if(ack){
				for(int i = 0; i < NPORT; i++)
					ack_ports[i] = 0;

				uint8_t svc = header_table[selected_line] & 0x3;
				if(svc == SVC_CLEAR)
					out_state = OUT_CLEAR_TABLE;
				else
					out_state = OUT_INIT;
			}
			break;
		case OUT_WAIT_ACK_DOWN:			
			if(!ack_in[LOCAL])
				out_state = OUT_INIT;
			
			break;
		case OUT_CLEAR_TABLE:
			target = (header_table[selected_line] >> 2) & 0xFF;
			if(target == router_address){
				uint8_t source = (header_table[selected_line] >> 10) & 0xFF;
				uint8_t src_x = source >> 4;
				uint8_t src_y = source & 0xF;
				uint8_t tgt_x = target >> 4;
				uint8_t tgt_y = target & 0xF;
				uint8_t svc = header_table[selected_line] & 0x3;

				std::cout << ">>>>>>>>>>>>>>>>> end CLEAR: " << 
					src_x << " " << src_y << " " <<
					tgt_x << " " << tgt_y << "]] " << 
					svc << " " <<
					data_table[selected_line] <<
					std::endl;
			}

			out_state = OUT_INIT;
			break;
	}
}

void BrLiteRouter::input_output()
{
	static uint32_t current_tick = 0;
	static uint32_t wrote_tick = 0;
	static uint8_t wrote_idx = 0;
	static bool wrote_local = false;

	if(reset){
		for(int i = 0; i < BRLITE_CAM_SIZE; i++){
			pending_table[i] = 0;
			used_table[i] = 0;
			header_table[i] = 0;
		}

		current_tick = 0;

		clear_local = false;
		wrote_local = false;
		wrote_idx = 0;
		wrote_tick = 0;

		return;
	}

	current_tick = current_tick + 1;

	if(wrote_local && wrote_tick < current_tick){
		clear_local = true;
	}

	if(clear_local && in_state == IN_INIT && out_state == OUT_INIT){
		clear_local = false;
		wrote_local = false;
		header_table[wrote_idx] = (header_table[wrote_idx] & 0xFFFFFFFC) | SVC_CLEAR;
		pending_table[wrote_idx] = true;
	}

	switch(in_state){
		case IN_WRITE:
			used_table[free_idx] = 1;
			pending_table[free_idx] = true;
			header_table[free_idx] = header_in[selected_port];
			if(selected_port == LOCAL){
				wrote_local = true;
				wrote_tick = current_tick + BRLITE_CLEAR_INTERVAL;
				wrote_idx = free_idx;
			}
			break;
		case IN_CLEAR:
			header_table[source_idx] = (header_table[source_idx] & 0xFFFFFFFC) | SVC_CLEAR;
			break;
		default:
			break;
	}

	switch(out_state){
		case OUT_TEST_SVC:
			pending_table[selected_line] = 0;
			break;
		case OUT_CLEAR_TABLE:
			used_table[selected_line] = 0;
			break;
		default:
			break;
	}

}
