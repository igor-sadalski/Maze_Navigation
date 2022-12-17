#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

#define tri_Red LATGbits.LATG0
#define tri_Blue LATAbits.LATA3
#define tri_Green LATEbits.LATE7

//definition of RGB structure
typedef struct RGB_val { 
	unsigned int R;
	unsigned int G;
	unsigned int B;
    unsigned int W;
} RGB_val;

enum color_flags {red, blue, light_blue, green, white, yellow, black, pink, orange, repeat} colors; //big letters


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_initializeClick(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to _RED_COLOR_FLAGwrite to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writeToAddres(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_readStatus(void);

/********************************************//**
 *  Function to read all channels
 *	Returns a 16 bit ADC value representing colour intensity for each chanell
 ***********************************************/
void color_readColorValues(RGB_val *colour);

/********************************************//**
 *  Functions for read color value and return color in Hue
 *	This functions reas 3 color filters and Clear Filters and converts the resulting color from RGBC to HSL
 * The color range is between 0.0650 and 1.0000.
 ***********************************************/
float color_getColorValue();

/********************************************
 * Functions for detect colors, takes color_value  in Hue and return color flag
 * Detect color
      ORANGE color - from 0.992  to 0.999
      RED color    - from 0.9750 to 0.9919
      PINK color   - from 0.920  to 0.9749
      PURPLE color - from 0.6201 to 0.919
      BLUE color   - from 0.521  to 0.6200
      CYAN color   - from 0.4700 to 0.520
      GREEN color  - from 0.210  to 0.469
      YELLOW color - from 0.0650 to 0.209
***********************************************/
int color_getColor(float color_value);
void led_init(void);
void led_off(void);


#endif
