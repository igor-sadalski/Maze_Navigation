#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "dc_motor.h"
#include "interrupts.h"
#include "timers.h"
#include "Actions.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz

volatile int at_wall = 0; // this is when the buggy detects a wall and should read a colour
volatile int second = 0;  // second count that will increase from buggy turn on
volatile int end = 0;  // second count that will increase from buggy turn on

void buggy_makeColorActions(DC_motor *mL, DC_motor *mR,action_stack *stack);

void main(void) {

    // motor initializations
    DC_motor motorL, motorR; //declare two DC_motor structures 
    initDCmotorsvar(&motorL, &motorR, 199); // sets all initial variable states and PWM

    action_stack a_stack; // defines the memory stack  
    a_stack.top = -1; // start with top being -1 showing empty stack

    color_initializeClick();    // initialaize the I2C communications

    start_motors(); // endless loop preventing anything else being done before button is pressed
    led_init(); // turns on colour clicker lights
    second = 0; // start the counter at 0 when motors start spinning
    fullSpeedAhead(&motorL, &motorR, speed);

    Interrupts_initPIC();   // start the interrupts

    while (!end) {
        while (at_wall) { 
            memory_pushTime(second, &a_stack);                  //begin with pushing the time that have hit the flag
            stop_slow(&motorL, &motorR);                        // Stop the buggy
            __delay_ms(1000);                                   //color cards are VERY DEPENDED ON THE DISTANCE, make sure you are touching the card for scanning
            forward_to_parallel(&motorL, &motorR);              //use to make sure you are perpendicular to the wall
            buggy_makeColorActions(&motorL, &motorR, &a_stack); // read the colour
            at_wall = 0;                                        // Reset the at wall command
            Interrupts_initColorClick();                        // After wall has been hit and colours are read have to start up colour interrupts again
            if (!end) {
                second = 0;                                  // reset the drive timer
                fullSpeedAhead(&motorL, &motorR, speed);        // full speed ahead after the wall has been hit
            }
        }
    }
}