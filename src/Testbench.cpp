/**
 * MA-Memphis
 * @file Testbench.cpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Testbench for Light BrNoC
 */

#include "Testbench.hpp"
#include "BrLiteRouter.hpp"

Testbench::Testbench(sc_module_name _name, uint8_t _x_size, uint8_t _y_size) :
	sc_module(_name),
	x_size(_x_size),
	y_size(_y_size),
	ack_out(_x_size*_y_size),
	req_in(_x_size*_y_size),
	payload_in(_x_size*_y_size),
	address_in(_x_size*_y_size),
	id_svc_in(_x_size*_y_size),
	ack_in(_x_size*_y_size),
	req_out(_x_size*_y_size),
	payload_out(_x_size*_y_size),
	address_out(_x_size*_y_size),
	id_svc_out(_x_size*_y_size),
	local_busy(_x_size*_y_size),
	msgids(_x_size*_y_size),
	noc("NoC", _x_size, _y_size),
	lines(_x_size*_y_size),
	log("brNoC_log.txt")
{
	for(int i = 0; i < x_size*y_size; i++){
		noc.ack_in[i](ack_out[i]);
		noc.req_out[i](req_in[i]);
		noc.payload_out[i](payload_in[i]);
		noc.address_out[i](address_in[i]);
		noc.id_svc_out[i](id_svc_in[i]);

		noc.ack_out[i](ack_in[i]);
		noc.req_in[i](req_out[i]);
		noc.payload_in[i](payload_out[i]);
		noc.address_in[i](address_out[i]);
		noc.id_svc_in[i](id_svc_out[i]);
		noc.local_busy[i](local_busy[i]);
	}

	noc.clock(clock);
	noc.reset(reset);

	SC_METHOD(receive);
	for(int i = 0; i < x_size*y_size; i++){
		sensitive << req_in[i];
	}

	SC_METHOD(send);
	sensitive << clock.pos();

	SC_METHOD(timer);
	sensitive << clock.pos();

	SC_THREAD(ack);

	std::cout << "---  NUMBER_PROCESSORS_X " << (int)x_size << std::endl;
	std::cout << "---  NUMBER_PROCESSORS_Y " << (int)y_size << std::endl;
}

Testbench::~Testbench()
{
	for(int i = 0; i < x_size*y_size; i++){
		log << lines[i].str();
		log << std::endl;
	}
}

void Testbench::timer()
{
	if(reset){
		tick = 0;
		return;
	}

	tick = tick + 1;
}

void Testbench::send()
{
	if(reset){
		for(int i = 0; i < x_size*y_size; i++){
			req_out[i] = 0;
			msgids[i] = 0;
		}
		for(int i = 0; i < N_PKT; i++)
			pkt_used[i] = false;
		return;
	}

	for(int i = 0; i < N_PKT; i++){
		uint32_t timestamp = PACKETS[i][0];
		uint16_t src = PACKETS[i][1];
		if(tick >= timestamp && !pkt_used[i] && !local_busy[src]){
			req_out[src] = true;
			payload_out[src] = PACKETS[i][3];
			uint32_t address = 0;
			address |= ((((src % x_size) << 8) | (src / x_size)) << 16);
			uint16_t tgt = PACKETS[i][2];
			address |= ((((tgt % x_size) << 8) | (tgt / x_size)));
			address_out[src] = address;

			uint8_t id_svc = 0;
			id_svc |= (msgids[src] << 2);
			id_svc |= (PACKETS[i][4] & 0x3);
			id_svc_out[src] = id_svc;
			msgids[src]++;
			msgids[src] %= 0x3F;

			pkt_used[i] = true;
			std::cout << "-----------------------------------------  INSERT SERVICE " <<
				(int)src << " " << PACKETS[i][2] << " " <<
				PACKETS[i][3] << " " <<
				(PACKETS[i][4] & 0x3) << std::endl;
		}
	}

	for(int i = 0; i < x_size*y_size; i++){
		if(ack_in[i])
			req_out[i] = false;
	}

	if(PACKETS[N_PKT - 1][0] + 300 < tick){
		std::cout << "---END SIMULATION------- " << std::dec << PACKETS[N_PKT - 1][0] << std::endl;
		sc_stop();
	}
}

void Testbench::receive()
{
	for(int i = 0; i < x_size*y_size; i++){
		if(req_in[i].event() && req_in[i]){
			BrLiteRouter::Service svc = BrLiteRouter::SERVICE(id_svc_in[i]);
			if(svc == BrLiteRouter::Service::ALL){
				lines[i] << "ALL";
			} else {
				lines[i] << "TGT";
			}
			uint16_t src = BrLiteRouter::SOURCE(address_in[i]);
			lines[i] << " " << i << "   from: " << ((src >> 8) + (src & 0xFF)*x_size) << "  " << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << payload_in[i] << std::dec << std::setw(0) << "  t:" << tick << "\n";
		}
	}
}

void Testbench::ack()
{
	while(true){
		wait(10, SC_NS);
		if(reset){
			for(int i = 0; i < x_size*y_size; i++)
				ack_out[i] = 0;
		} else {
			for(int i = 0; i < x_size*y_size; i++){
				ack_out[i] = req_in[i];
			}
		}
	}
}
