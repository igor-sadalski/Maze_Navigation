#include <xc.h>
#include "i2c.h"
#include "color.h"
#include "dc_motor.h" //#REMOVE WHEN TESTING DONE
#include "timers.h"
#include "interrupts.h"

extern int at_wall;
extern int second;

int Interrupts_distance_calibrate(void){
    //function takes ambient light at start up and adds 100 to this value
    int clear;
    RGB_val colour;                        // declares one new structure of RGB value type 
    color_readColorValues(&colour);
    clear = colour.W;
    clear = clear + 200;
    return clear;
}

void Interrupts_initPIC(void)
{
    //LED OUTPUTS
    TRISBbits.TRISB0=1;         //set pin on buggy as an input (mapped to mikro bus port 1 INT pin))
    ANSELBbits.ANSELB0 = 0;     //ST and TTL input buffers are enabled
    
    //INITALIZATIONS
    Interrupts_initColorClick();   //color click
    
    Timer0_init();                 //timer
    
    //PERIPHERAL INTERRUPTS
    INTCONbits.PEIE = 1;        // enable peripheral interrupts
    INTCONbits.IPEN = 1;        // enables priority 
    
    //INTERRUPTS for TIMMER 
    PIE0bits.TMR0IE = 1;        // enable timer interrupt
    IPR0bits.INT0IP = 1;        // set timer as high priority interupt
    
    //INTERRUPTS for COLOR CLICK 
    PIE0bits.INT0IE = 1;        // enable interupt
    IPR0bits.INT0IP = 0;        // set this as low priority interupt
    PIR0bits.INT0IF = 0;        // start with flag off
    INTCONbits.INT0EDG = 0;     // int pin is low when interrupt is triggered
   
    //GLOBAL START
    INTCONbits.GIE = 1;         //enable global interupts
}

void Interrupts_initColorClick() {
    int high = Interrupts_distance_calibrate(); // calibrates the stopping distance at the start
    
    color_turnOffInterruptsFlag();                    //begin intializization with flags off
    color_writeToAddres(0x00, 0b00010011);            //enable AIEN, AEN, PON
    color_writeToAddres(0x0C, 0b0010);                 //enable persistence register and write that every 3rd Clear channel cycle
    //low threshold
    color_writeToAddres(0x04, low_thres & 0xff);      //set RGBC interrupt low threshold low byte  
    color_writeToAddres(0x05, low_thres >> 8);        //set RGBC interrupt low threshold high byte
    // high threshold
    color_writeToAddres(0x06, high & 0xff); 
    color_writeToAddres(0x07, high >> 8);
}

void color_turnOffInterruptsFlag(){
    I2C_2_Master_Start();                   // Start writing comms 
	I2C_2_Master_Write(0x52 | 0x00);        // 7 bit address + Write mode
    I2C_2_Master_Write(0b11100110);         // Clear RGBC interrupt by writing to command register
    I2C_2_Master_Stop();    
}

void color_disableInterrupts(){
    color_turnOffInterruptsFlag();
    color_writeToAddres(0x00, 0x03);        //toggle interrupts off to read the color values
    color_turnOffInterruptsFlag();
}

void __interrupt(high_priority) HighISR()
{
    if(PIR0bits.TMR0IF){
        // add some code to increase the timer by 1 second
        PIR0bits.TMR0IF = 0;    //clear the flag at start so counter starts straight away in the buggy
        second = second + 1;    // increase the second count
    }      
}

void __interrupt(low_priority) LowISR()
{
    if(PIR0bits.INT0IF){
        at_wall = 1;
        color_disableInterrupts();         //otherwise reading values will not be possible
        color_turnOffInterruptsFlag();      // clear the flag and turn interrupts off
        PIR0bits.INT0IF = 0;             //clear the flag in the buggy
    }       
}