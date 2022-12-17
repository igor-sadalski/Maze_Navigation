#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000
#define low_thres 0
#define high_thres 2000

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
int Interrupts_distance_calibrate(void);
/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_initPIC(void);
/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_initColorClick(void);
/************************************
 * Send command to color click to clear the interrupt flag
************************************/
void color_turnOffInterruptsFlag(void);
/************************************
 * Use on the way back to the station
************************************/
void color_disableInterrupts(void);
/************************************
 * High priority interrupt service routine
 * used for timer overflows is our code is running to long i.e. robot is stuck
************************************/
void __interrupt(high_priority) HighISR();
/************************************
 * Low priority interrupt service routine
 * used when we detected a new color and must stop to read it
************************************/
void __interrupt(low_priority) LowISR();

#endif

//activation values for high-tress differ based on a colour card, thus, they should
//be normalized to black or track side in my room at night e.g. this was around 1500