/**
 * MA-Memphis
 * @file BrLiteRouter.hpp
 * 
 * @author Angelo Elias Dalzotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date August 2021
 * 
 * @brief Light BrNoC router module -- Removed backtrack (unicast)
 */

#include <systemc.h>

/**
 * @todo Remove -- it is already in standards.h
 */
#define NPORT 5
#define NORTH 2
#define SOUTH 3
#define EAST  0
#define WEST  1
#define LOCAL 4

static const uint8_t BRLITE_MID_SIZE = 14;
static const uint8_t BRLITE_SRC_SIZE = 8;
static const uint8_t BRLITE_TGT_SIZE = 8;
static const uint8_t BRLITE_SVC_SIZE = 2;
static const uint8_t BRLITE_HDR_SIZE =
	BRLITE_MID_SIZE +
	BRLITE_SRC_SIZE +
	BRLITE_TGT_SIZE +
	BRLITE_SVC_SIZE;

static const uint8_t BRLITE_PLD_SIZE = 32;

static const uint8_t BRLITE_CAM_SIZE = 8;

static const uint16_t BRLITE_CLEAR_INTERVAL = 180;

static const uint8_t SVC_NULL = 0x0;
static const uint8_t SVC_CLEAR = 0x1;
static const uint8_t SVC_ALL = 0x2;
static const uint8_t SVC_TGT = 0x3;

SC_MODULE(BrLiteRouter){
public:
	/* Router signals */
	sc_in<bool>	clock;
	sc_in<bool>	reset;

	/* Data Inputs */
	sc_in<uint32_t>		data_in[NPORT];
	sc_in<uint32_t>		header_in[NPORT];
	sc_in<bool>			req_in[NPORT];
	sc_out<bool>		ack_out[NPORT];

	/* Data Outputs */
	sc_out<uint32_t>	data_out[NPORT];
	sc_out<uint32_t>	header_out[NPORT];
	sc_out<bool>		req_out[NPORT];
	sc_in<bool>			ack_in[NPORT];

	sc_out<bool> local_busy;

	SC_HAS_PROCESS(BrLiteRouter);
	BrLiteRouter(sc_module_name _name, uint8_t _address);

private:
	enum IN_FSM {
		IN_INIT,
		IN_ARBITRATION,
		IN_TEST_SPACE,
		IN_WRITE,
		IN_CLEAR,
		IN_WAIT_REQ_DOWN,
	};

	enum OUT_FSM {
		OUT_INIT,
		OUT_ARBITRATION,
		OUT_TEST_SVC,
		OUT_WAIT_ACK_PORTS,
		OUT_CLEAR_TABLE,
		OUT_SEND_LOCAL,
		OUT_WAIT_ACK_DOWN,
	};

	sc_signal<uint8_t>	selected_line;
	sc_signal<uint8_t>	selected_port;
	sc_signal<uint8_t>	source_idx;
	sc_signal<uint8_t>	free_idx;
	sc_signal<bool>		clear_local;

	sc_signal<enum IN_FSM> in_state;
	sc_signal<enum OUT_FSM> out_state;

	sc_signal<bool>		used_table[BRLITE_CAM_SIZE];
	sc_signal<bool>		pending_table[BRLITE_CAM_SIZE];
	sc_signal<uint8_t>	input_table[BRLITE_CAM_SIZE];
	sc_signal<uint32_t>	header_table[BRLITE_CAM_SIZE];
	sc_signal<uint32_t>	data_table[BRLITE_CAM_SIZE];

	uint8_t router_address;

	std::array<bool,	NPORT>	ack_ports;
	bool wrote_local;
	uint8_t wrote_idx;
	uint32_t wrote_tick;
	uint32_t current_tick;

	void input();
	void output();
	void input_output();
};
