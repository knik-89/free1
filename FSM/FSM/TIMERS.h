/*
 * TIMERS.h
 *
 *  Created on: 23.05.2012
 *      Author: Masakra
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#define SEC 			1000
#define MIN 			60 * sec
#define HOUR 			60 * min

#define MAX_TIMERS 		4

#define TIMER_LED 		0
#define TIMER_LCD 		1
#define TIMER_UART     	2
#define TIMER_CMD 		3

//#define USE_GLOBAL_TIMERS

#ifdef USE_GLOBAL_TIMERS
	#define MAX_GTIMERS 	4

	#define GTIMER_1		0
	#define GTIMER_2		1
	#define GTIMER_3		2
	#define GTIMER_4		3
#endif

#define u08 unsigned char
#define	u16 unsigned int

void ProcessTimers(u08 * tick);
void InitTimers(void);
u16  GetTimer(u08 Timer);
void ResetTimer(u08 Timer);

#ifdef USE_GLOBAL_TIMERS
	u16  GetGTimer(u08 Timer);
	void StopGTimer(u08 Timer);
	void StartGTimer(u08 Timer);
	void PauseGTimer(u08 Timer);
	void ContinueGTimer(u08 Timer);
#endif

#endif /* TIMERS_H_ */
