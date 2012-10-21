/*
 * timer.h
 *
 *  Created on: 13.9.2012
 *      Author: one
 */

#ifndef TIMER_H_
#define TIMER_H_

typedef unsigned int (*pf_getTimerVal_us)();

typedef struct {
	unsigned int savedTime;
} ttimer;

int initTimer(pf_getTimerVal_us gett);
void saveTime(ttimer *tim);
unsigned int getTime_ms(ttimer *tim);
unsigned int getTime_us(ttimer *tim);

#endif /* TIMER_H_ */
