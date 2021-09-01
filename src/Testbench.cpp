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
	data_in(_x_size*_y_size),
	header_in(_x_size*_y_size),
	ack_in(_x_size*_y_size),
	req_out(_x_size*_y_size),
	data_out(_x_size*_y_size),
	header_out(_x_size*_y_size),
	local_busy(_x_size*_y_size),
	msgids(_x_size*_y_size),
	noc("NoC", _x_size, _y_size),
	lines(_x_size*_y_size),
	log("brNoC_log.txt")
{
	for(int i = 0; i < x_size*y_size; i++){
		noc.ack_in[i](ack_out[i]);
		noc.req_out[i](req_in[i]);
		noc.data_out[i](data_in[i]);
		noc.header_out[i](header_in[i]);

		noc.ack_out[i](ack_in[i]);
		noc.req_in[i](req_out[i]);
		noc.data_in[i](data_out[i]);
		noc.header_in[i](header_out[i]);
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
		uint8_t src = PACKETS[i][1];
		if(tick >= timestamp && !pkt_used[i] && !local_busy[src]){
			req_out[src] = true;
			data_out[src] = PACKETS[i][3];
			uint32_t header = 0;
			header |= (msgids[src] << 18);
			msgids[src]++;
			header |= ((((src % x_size) << 4) | (src / x_size)) << 10);

			uint8_t tgt = PACKETS[i][2];
			header |= ((((tgt % x_size) << 4) | (tgt / x_size)) << 2);

			header |= (PACKETS[i][4] & 0x3);
			header_out[src] = header;
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
		std::cout << "---END SIMULATION------- " << PACKETS[N_PKT - 1][0] << std::endl;
		sc_stop();
	}
}

void Testbench::receive()
{
	for(int i = 0; i < x_size*y_size; i++){
		if(req_in[i].event() && req_in[i]){
			BrLiteRouter::Service svc = static_cast<BrLiteRouter::Service>(header_in[i] & 0x3);
			// lines[i] << "PE " << (i % x_size) << "x" << (i / x_size) << ": ";
			if(svc == BrLiteRouter::Service::ALL){
				lines[i] << "ALL";
			} else {
				lines[i] << "TGT";
			}
			uint16_t src = (header_in[i] >> 10) & 0xFF;
			lines[i] << " " << i << "   from: " << ((src >> 4) + (src & 0xF)*x_size) << "  " << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << data_in[i] << std::dec << std::setw(0) << "  t:" << tick << "\n";
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
