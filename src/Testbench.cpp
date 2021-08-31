#include "Testbench.hpp"

const uint16_t n_pkt = 29;
bool pkt_used[n_pkt] = {};
const uint32_t packets[n_pkt][5] = {
	{   4, 0x40, 0x00, 0x01, SVC_ALL},
	{  80, 0x00, 0x00, 0x02, SVC_ALL},
	{  80, 0x30, 0x00, 0x03, SVC_ALL},
	{  80, 0x50, 0x00, 0x04, SVC_ALL},

	{ 150, 0x01, 0x60, 0xBA, SVC_TGT},

	{ 250, 0x01, 0x50, 0xA8, SVC_TGT},
	{ 300, 0x00, 0x50, 0xA0, SVC_TGT},
	{ 350, 0x60, 0x50, 0xA6, SVC_TGT},
	{ 400, 0x10, 0x50, 0xA1, SVC_TGT},
	{ 410, 0x30, 0x50, 0xA3, SVC_TGT},
	{ 410, 0x70, 0x50, 0xA7, SVC_TGT},
	{ 420, 0x40, 0x50, 0xA4, SVC_TGT},

	{ 500, 0x20, 0x00, 0xEE, SVC_TGT},
	{ 550, 0x10, 0x00, 0xFF, SVC_TGT},
	{ 650, 0x50, 0x00, 0x88, SVC_TGT},

	{ 650, 0x60, 0x40, 0x66, SVC_TGT},
	{ 680, 0x63, 0x00, 0x9F, SVC_ALL},
	{ 750, 0x30, 0x40, 0x77, SVC_ALL},
	{ 770, 0x00, 0x40, 0xCA, SVC_TGT},
	{ 790, 0x20, 0x40, 0xBE, SVC_TGT},

	{ 850, 0x60, 0x00, 0x11, SVC_TGT},
	{ 900, 0x10, 0x70, 0x22, SVC_ALL},
	{ 950, 0x01, 0x70, 0x33, SVC_TGT},
	{ 950, 0x20, 0x70, 0x44, SVC_TGT},

	{1100, 0x70, 0x00, 0xAF, SVC_TGT},
	{1150, 0x40, 0x00, 0xDE, SVC_TGT},
	{1200, 0x01, 0x00, 0xBC, SVC_ALL},
	{1200, 0x50, 0x00, 0xF1, SVC_ALL},
	{1300, 0x20, 0x70, 0x33, SVC_TGT},
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
		for(int i = 0; i < n_pkt; i++)
			pkt_used[i] = false;
		return;
	}

	for(int i = 0; i < n_pkt; i++){
		uint32_t timestamp = packets[i][0];
		uint8_t src = packets[i][1];
		uint8_t src_idx = (src >> 4) + (src & 0xF)*x_size;
		if(tick >= timestamp && !pkt_used[i] && !local_busy[src_idx]){
			req_out[src_idx] = true;
			data_out[src_idx] = packets[i][3];
			uint32_t header = 0;
			header |= (msgids[src_idx] << 18);
			msgids[src_idx]++;
			header |= ((src & 0xFF) << 10);
			header |= ((packets[i][2] & 0xFF) << 2);
			header |= (packets[i][4] & 0x3);
			header_out[src_idx] = header;
			pkt_used[i] = true;
			std::cout << "-----------------------------------------  INSERT SERVICE " <<
				(int)src << " " << packets[i][2] << " " <<
				packets[i][3] << " " <<
				(packets[i][4] & 0x3) << std::endl;
		}
	}

	for(int i = 0; i < x_size*y_size; i++){
		if(ack_in[i])
			req_out[i] = false;
	}

	if(packets[n_pkt - 1][0] + 300 < tick){
		std::cout << "---END SIMULATION------- " << packets[n_pkt - 1][0] << std::endl;
		sc_stop();
	}
}

void Testbench::receive()
{
	for(int i = 0; i < x_size*y_size; i++){
		if(req_in[i].event() && req_in[i]){
			uint8_t svc = header_in[i] & 0x3;
			// lines[i] << "PE " << (i % x_size) << "x" << (i / x_size) << ": ";
			if(svc == SVC_ALL){
				lines[i] << "ALL";
			} else {
				lines[i] << "TGT";
			}
			uint16_t src = (header_in[i] >> 10) & 0xFF;
			lines[i] << " " << i << "   from: " << ((src >> 4) + (src & 0xF)*x_size) << "  " << std::hex << std::setfill('0') << std::setw(2) << data_in[i] << std::dec << std::setw(0) << "  t:" << tick << "\n";
		}
	}
}

void Testbench::ack()
{
	while(true){
		wait(20, SC_NS);
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
