# Real Time Embedded Systems Assignment #1, AUTh [2019]
> Test sampling accuracy using minimum energy possible

This is an *experimental* application developed as part of the course "Real Time Embedded Systems" assignment, that took place in the Department of Electrical & Computer Engineering at Aristotle University of Thessaloniki in 2019.

The goal is to test the accuracy of sampling in embedded systems using different techniques, while trying to minimize the energy consumption of the system. 
To this end, three approaches have been implemented and tested.

	1. NonAdaptive	-> Sleep for the amount of requested seconds [usleep() and sleep() used].
	2. Adaptive	-> Sleep for the amount of requested seconds, but check the divergence of the actual requested time and adapt for the next sample [usleep() and sleep() used].
	3. Interrupt	-> Set an interrupt and do other tasks, until the alarm rings.
---

## Execution

To execute the code, you first need to compile it using:
```sh
gcc main.c -o main
```
and then run using:
```sh
./main [Oper] [time] [delta]
```
where:

 - **Oper** is the requested implementation from the three described above.
 - **time** is the total duration of the sampling in seconds.
 - **delta** is the sampling period in seconds.

all parameters are required.

---

## Status

As of the completion of the project, it will NOT be maintained. By no means should it ever be considered stable or safe to use, as it may contain incomplete parts, critical bugs and security vulnerabilities.

---

## Support

Reach out to me:

- [mpalaourg's email](mailto:gbalaouras@gmail.com "gbalaouras@gmail.com")

---

## License

[![Beerware License](https://img.shields.io/badge/license-beerware%20%F0%9F%8D%BA-blue.svg)](https://github.com/mpalaourg/RTES_TASK1/blob/master/LICENCE.md)