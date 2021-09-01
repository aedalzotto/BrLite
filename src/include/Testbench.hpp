#include <systemc.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "BrLite.hpp"

#include "../../scenario.h"

SC_MODULE(Testbench){
public:
	sc_in<bool> clock;
	sc_in<bool> reset;

	SC_HAS_PROCESS(Testbench);
	Testbench(sc_module_name _name, uint8_t _x_size, uint8_t _y_size);

	~Testbench();
private:
	uint8_t x_size;
	uint8_t y_size;

	std::vector<sc_signal<bool>> ack_out;
	std::vector<sc_signal<bool>> req_in;
	std::vector<sc_signal<uint32_t>> data_in;
	std::vector<sc_signal<uint32_t>> header_in;

	std::vector<sc_signal<bool>> ack_in;
	std::vector<sc_signal<bool>> req_out;
	std::vector<sc_signal<uint32_t>> data_out;
	std::vector<sc_signal<uint32_t>> header_out;

	std::vector<sc_signal<bool>> local_busy;
	std::vector<uint16_t>	msgids;

	BrLite noc;

	sc_signal<uint32_t>	tick;

	std::vector<std::stringstream>	lines;
	std::ofstream log;
	bool pkt_used[N_PKT];

	void timer();
	void send();
	void receive();
	void ack();
};
