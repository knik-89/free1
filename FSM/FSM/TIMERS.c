/*
 * TIMERS.c
 *
 *  Created on: 23.05.2012
 *      Author: Masakra
 */

#include "TIMERS.h"

volatile u16 Timers[MAX_TIMERS];

#ifdef USE_GLOBAL_TIMERS
	#define TIMER_STOPPED		0
	#define TIMER_RUNNING		1
	#define TIMER_PAUSED		2

	volatile u16 GTimers[MAX_GTIMERS];
	volatile u08 GTStates[MAX_GTIMERS];
#endif

/*
 * Even if main program cycle is longer than system timer ticks period all SW timers will
 * be updated correctly by adding value of ticks from HW system timer accessed by *tick
 * pointer. After all SW timers incremented system timer ticks are cleared.
 * It is easy to change clock division ratio for SW timers from HW timer (default is 1:1).
 */
void ProcessTimers(u08 * tick) {
	u08 i = 0;
	u08 x = *tick;

	if (x > 0) {
		for (i=0; i<MAX_TIMERS; i++) {
			Timers[i] += x;
#ifdef USE_GLOBAL_TIMERS
			if (GTStates[i] == TIMER_RUNNING) {
				GTimers[i] += x;
			}
#endif
		}
		*tick = 0;
	}
}

void InitTimers(void) {
	u08 i;
	for (i=0; i<MAX_TIMERS; i++) {
		Timers[i] = 0;
	}
}

u16 GetTimer(u08 Timer) {
	return Timers[Timer];
}

void ResetTimer(u08 Timer) {
	Timers[Timer] = 0;
}

#ifdef USE_GLOBAL_TIMERS
/// Global Timers functions
u16  GetGTimer(u08 Timer){
	return GTimers[Timer];
}

void StopGTimer(u08 Timer){
	GTStates[Timer] = TIMER_STOPPED;
}
void StartGTimer(u08 Timer){
	if (GTStates[Timer] == TIMER_STOPPED) {
		GTimers[Timer] = 0;
		GTStates[Timer] = TIMER_RUNNING;
	}
}
void PauseGTimer(u08 Timer) {
	if (GTStates[Timer] == TIMER_RUNNING) {
		GTStates[Timer] = TIMER_PAUSED;
	}
}
void ContinueGTimer(u08 Timer) {
	if (GTStates[Timer] == TIMER_PAUSED) {
		GTStates[Timer] = TIMER_RUNNING;
	}
}
/// *************************
#endif
