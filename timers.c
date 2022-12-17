#include <xc.h>
#include <stdio.h>
#include "timers.h"
#include "interrupts.h"

#define test 1 // 1 for test mode (1 day takes 24 seconds)

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0110; // 1:64 prescaler ratio
    T0CON0bits.T016BIT=1;	//16 bit mode	
	
    TMR0H=3035>>8;  //write High reg first, update happens when low reg is written to set back to 0
    TMR0L=3035;
    T0CON0bits.T0EN=1;	//start the timer
    __delay_ms(10);            //have time to initialize all correctly  
}

/************************************
 * Function to return the full 16bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
unsigned int get16bitTMR0val(void)
{
	//add your code to get the full 16 bit timer value here
    int Low = TMR0L; // low counter
    int Hi = TMR0H << 8; // high counter shifted 8 bits along
    return Hi | Low; // combing the two counters
}
