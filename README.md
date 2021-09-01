# BrLite
Light broadcast NoC for control messages in many-core systems.
This is based in the BrNoC with removed backtracking (unicast service).
Table clearing is done by an internal timer in each router.

## Simulating

Modify [scenario.h](scenario.h) to generate the desired packets with a defined NoC size.
Build using `make`.
Then run `./brlite`.

## Acknowledgements

* Light BrNoC (VHDL Implementation) - [Fernando Gehm Moraes](https://github.com/moraesfg).
* BrNoC
```
Wachter, E., Caimi, L. L., Fochi, V., Munhoz, D., and Moraes, F. G. (2017). BrNoC: A broadcast NoC for control messages in many-core systems. Microelectronics Journal, 68:69-77.
```
