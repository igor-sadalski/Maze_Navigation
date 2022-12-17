//arthur gallios

#ifndef _timers_H
#define _timers_H

#include <xc.h>

#define _XTAL_FREQ 64000000

void Timer0_init(void);
unsigned int get16bitTMR0val(void);
void time();
void date();
void daylight_saving();
void time_correction();

#endif
