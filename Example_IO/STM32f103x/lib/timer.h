/*
 * timer.h
 *
 *  Created on: 13.9.2012
 *      Author: one
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "co_driver.h"

typedef unsigned int (*pf_getTimerVal_us)();

typedef struct {
	UNSIGNED32 savedTime;
} ttimer;

int initTimer(pf_getTimerVal_us gett);
void saveTime(ttimer *tim);
UNSIGNED32 getTime_ms(ttimer *tim);
UNSIGNED32 getTime_us(ttimer *tim);

#endif /* TIMER_H_ */
