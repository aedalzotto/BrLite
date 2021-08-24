#include <systemc.h>

SC_MODULE(ClockGen){
public:
	sc_out<bool> clock;
	sc_out<bool> reset;

	SC_HAS_PROCESS(ClockGen);
	ClockGen(sc_module_name _name);

private:
	void clock_gen();
	void reset_gen();
};
