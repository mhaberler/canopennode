/*
 * timer.c
 *
 *  Created on: 13.9.2012
 *      Author: one
 */
#include "timer.h"

static pf_getTimerVal_us gettimerfunc = 0;

int initTimer(pf_getTimerVal_us gett) {
	gettimerfunc = gett;
	return 0;
}

void saveTime(ttimer *tim) {
	if (!tim)
		return;
	tim->savedTime = gettimerfunc();
}

UNSIGNED32 getTime_ms(ttimer *tim) {
	if (!tim)
		return 0;
	return (gettimerfunc() - tim->savedTime) / 1000;
}

UNSIGNED32 getTime_us(ttimer *tim) {
	if (!tim)
		return 0;
	UNSIGNED32 now = gettimerfunc();
	return (now - tim->savedTime);
}
