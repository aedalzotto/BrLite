#include "Testbench.hpp"

const uint16_t n_pkt = 30;
bool pkt_used[n_pkt] = {};
const uint32_t packets[n_pkt][5] = {
	{ 4, 4, 0, 0x1, SVC_ALL},
	{80, 0, 0, 0x2, SVC_ALL},
	{80, 3, 0, 0x3, SVC_ALL},
	{80, 5, 0, 0x4, SVC_ALL},

	{150, 8, 6, 0xBA, SVC_TGT},

	{250, 8, 5, 0xA8, SVC_TGT},
	{300, 0, 5, 0xA0, SVC_TGT},
	{350, 6, 5, 0xA6, SVC_TGT},
	{400, 1, 5, 0xA1, SVC_TGT},
	{410, 3, 5, 0xA3, SVC_TGT},
	{410, 8, 5, 0xA7, SVC_TGT},
	{420, 4, 5, 0xA4, SVC_TGT},

	{500, 2, 0, 0xEE, SVC_TGT},
	{550, 1, 0, 0xFF, SVC_TGT},
	{650, 5, 0, 0x88, SVC_TGT},

	{650,  6, 4, 0x66, SVC_TGT},
	{680, 30, 0, 0x9F, SVC_ALL},
	{750,  3, 4, 0x77, SVC_ALL},
	{770,  0, 4, 0xCA, SVC_TGT},
	{790,  2, 4, 0xBE, SVC_TGT},

	{850,  6, 0, 0x11, SVC_TGT},
	{900,  1, 7, 0x22, SVC_ALL},
	{950,  8, 7, 0x33, SVC_TGT},
	{950,  2, 7, 0x44, SVC_TGT},

	{1100,  7, 0, 0xAF, SVC_TGT},
	{1150,  4, 0, 0xDE, SVC_TGT},
	{1200,  8, 0, 0xBC, SVC_ALL},
	{1200,  5, 0, 0xF1, SVC_ALL},
	{1300,  2, 7, 0x33, SVC_TGT},
};

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
	sensitive << clock.pos();

	SC_METHOD(send);
	sensitive << clock.pos();

	SC_METHOD(timer);
	sensitive << clock.pos();

	std::cout << "---  NUMBER_PROCESSORS_X " << x_size << std::endl;
	std::cout << "---  NUMBER_PROCESSORS_Y " << y_size << std::endl;
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
		for(int i = 0; i < n_pkt; i++)
			pkt_used[i] = false;
		return;
	}

	for(int i = 0; i < n_pkt; i++){
		uint32_t timestamp = packets[i][0];
		uint8_t src = packets[i][1];
		if(tick >= timestamp && !pkt_used[i] && !local_busy[src]){
			req_out[src] = true;
			data_out[src] = packets[i][3];
			uint32_t header = 0;
			header |= (msgids[src] << 18);
			msgids[src]++;
			header |= ((src & 0xFF) << 10);
			header |= ((packets[i][2] & 0xFF) << 2);
			header |= (packets[i][4] & 0x3);
			header_out[src] = header;
			pkt_used[i] = true;
			std::cout << "-----------------------------------------  INSERT SERVICE " <<
				src << " " << packets[i][2] << " " <<
				packets[i][3] << " " <<
				(packets[i][4] & 0x3) << std::endl;
		}
	}

	for(int i = 0; i < x_size*y_size; i++){
		if(ack_in[i])
			req_out[i] = false;
	}

	if(packets[n_pkt - 1][0] + 300 > tick)
		sc_stop();
}

void Testbench::receive()
{
	if(reset){
		for(int i = 0; i < x_size*y_size; i++)
			ack_out[i] = 0;
		return;
	}

	for(int i = 0; i < x_size*y_size; i++){
		ack_out[i] = req_in[i];

		if(req_in[i].posedge()){
			uint8_t svc = header_in[i] & 0x3;
			if(svc == SVC_ALL){
				log << "ALL";
			} else {
				log << "TGT";
			}
			uint16_t src = (header_in[i] << 10) & 0xFF;
			log << " from: " <<  src << " " << data_in[i] << " t:" << tick << "\n";
		}
	}
}
