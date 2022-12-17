#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "interrupts.h"

// initiliazes all the LED
void led_init(void){
    TRISGbits.TRISG0 = 0; //set RED pin as output
    TRISEbits.TRISE7 = 0; //set Green pin as output
    TRISAbits.TRISA3 = 0; //set Blue pin as output
    tri_Blue = 1;
    tri_Red = 1;
    tri_Green=1; 
}

void led_off(){
    tri_Blue = 0;
    tri_Red = 0;
    tri_Green=0; 
}

/*==========================================INTIALIZATIONS FUNCTIONS*/

void color_initializeClick(void)
{   
    I2C_2_Master_Init();                    //Initialise i2c Master
	color_writeToAddres(0x00, 0x01);          //set device PON
    __delay_ms(3);                          //need to wait 3ms for everthing to start up
	color_writeToAddres(0x00, 0x03);          //turn on device ADC
	color_writeToAddres(0x01, 0xD5);          //set integration time to 101 ms
    Interrupts_initColorClick();            //start the color click interrupts
                     
}

/*==========================================COMMUNICATION FUNCTIONS*/

void color_writeToAddres(char address, char value){
    I2C_2_Master_Start();                   //Start condition
    I2C_2_Master_Write(0x52 | 0x00);        //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);     //acknowledge bit + command code with register address
    I2C_2_Master_Write(value);              //write value
    I2C_2_Master_Stop();                    //Stop condition
}

unsigned int color_readStatus(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();                   //Start condition
	I2C_2_Master_Write(0x52 | 0x00);        //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x13);        //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();                // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);        //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);               //acknowledge, clear buffour and read the Red LSB
	I2C_2_Master_Stop();                    //Stop condition
	return tmp;
}

void color_readColorValues(RGB_val *colour){
    unsigned int R,G,B,W;
	I2C_2_Master_Start();                   //Start condition
	I2C_2_Master_Write(0x52 | 0x00);        //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);        // command (auto-increment protocol transaction) + start at white low register
	I2C_2_Master_RepStart();                // start a repeated transmission
	
    // Red read
    I2C_2_Master_Write(0x52 | 0x01);        //7 bit address + Read (1) mode
	W=I2C_2_Master_Read(1);                 //acknowledge, clear buffour and read the Red LSB
	W=W | (I2C_2_Master_Read(1)<<8);        //read the Red MSB (don't acknowledge as this is the last read)
    
    //Green read
	R=I2C_2_Master_Read(1);                 //acknowledge, clear buffour and read the Red LSB
	R=R | (I2C_2_Master_Read(1)<<8);        //read the Red MSB (don't acknowledge as this is the last read)
    
    //Blue read
	G=I2C_2_Master_Read(1);                 //acknowledge, clear buffour and read the Red LSB
	G=G | (I2C_2_Master_Read(1)<<8);        //read the Red MSB (don't acknowledge as this is the last read)
    
    //white read
	B=I2C_2_Master_Read(1);                 //acknowledge, clear buffour and read the Red LSB
	B=B | (I2C_2_Master_Read(0)<<8);        //read the Red MSB (don't acknowledge as this is the last read)
            
	I2C_2_Master_Stop();                    //Stop condition
    
    colour->R = R;                          //updating all values
    colour->G = G;
    colour->B = B;
    colour->W = W;
    
}

/*==========================================PRIVATE SUPPLEMENTARY FUNCTIONS*/
//these private functions has been adopted from the TS supplier github repository

static float _maxColor( float color_A, float color_B )
{
    if(color_A > color_B)
    {
        return color_A;
    }
    else
    {
        return color_B;
    }
}

static float _minColor( float color_A, float color_B )
{
    if(color_A < color_B)
    {
        return color_A;
    }
    else
    {
        return color_B;
    }
}

static float _RGB_To_Hue(float red, float green, float blue)
{
    float fmax;
    float fmin;
    float Hue_Value;
    float Saturation_Value;
    float Luminance_Value;

    fmax = _maxColor(_maxColor(red, green), blue);
    fmin = _minColor(_minColor(red, green), blue);

    Luminance_Value = fmax;

    if (fmax > 0)
    {
        Saturation_Value = (fmax - fmin) / fmax;
    }
    else
    {
        Saturation_Value = 0;
    }

    if (Saturation_Value == 0)
    {
        Hue_Value = 0;
    }
    else
    {
        if (fmax == red)
        {
             Hue_Value = (green - blue) / (fmax - fmin);
        }
        else if (fmax == green)
        {
             Hue_Value = 2 + (blue - red) / (fmax - fmin);
        }
        else
        {
             Hue_Value = 4 + (red - green) / (fmax - fmin);
        }
        Hue_Value = Hue_Value / 6;

        if (Hue_Value < 0)
        {
             Hue_Value = Hue_Value + 1;
        }
    }
    return Hue_Value;
}    

/*==========================================COLOR DETECTION FUNCTIONS*/

float color_getColorValue()
{
    RGB_val colour;                         // declares one new structure of RGB value type 
    RGB_val* ptr_to_colour = &colour;       //esier to navigate in code

    float sumHue = 0;
    int iter = 20;                         //integration time is 101ms i.e. 200x0.101 = 20.2s
    
    for (int counter = 0; counter < iter; counter++) 
    {
        color_readColorValues(ptr_to_colour); //each loop pool new reading
        
        float Red_Ratio     = (float) ptr_to_colour->R / ptr_to_colour->W;
        float Green_Ratio   = (float) ptr_to_colour->G / ptr_to_colour->W;
        float Blue_Ratio    = (float) ptr_to_colour->B / ptr_to_colour->W;
        
        float Hue           = _RGB_To_Hue(Red_Ratio, Green_Ratio, Blue_Ratio);
        sumHue = sumHue + Hue;
    }
    float avgHue = sumHue / iter;

    return avgHue;
}

int color_getColor(float color_value) //change to color hue
{    
/*  these values are very depended on the SPEED of APPROACHING THE CARD and when the interrupt is triggered
                                    +---------+---------+---------+
                                    |         | MIN - % | MAX + % |
                                    +---------+---------+---------+
                                    | RED     | 0.9611  | 1.0000  |
                                    +---------+---------+---------+
                                    | BLUE    | 0.3499  | 0.5075  |
                                    +---------+---------+---------+
                                    | L. BLUE | 0.2236  | 0.2609  |
                                    +---------+---------+---------+
                                    | GREEN   | 0.1796  | 0.2220  |
                                    +---------+---------+---------+
                                    | WHITE   | 0.0712  | 0.0782  |
                                    +---------+---------+---------+
                                    | YELLOW  | 0.0619  | 0.0668  |
                                    +---------+---------+---------+
                                    | BLACK   | 0.0547  | 0.0614  |
                                    +---------+---------+---------+
                                    | PINK    | 0.0446  | 0.0518  |
                                    +---------+---------+---------+
                                    | ORANGE  | 0.0205  | 0.0315  |
                                    +---------+---------+---------+
 * inaccuracy interval at 3% inaccuracy THERE ARE NO INTERVAL OVERLAPS!
 * in between REPEAT READING if nothing happens for 0 sec measured with timer just guess
 * try to implement better estimation using the statistical methods and normal distirbution fitting
 */
    int number_of_colors = 9;
    
    struct pair {
        float min;
        float max;
    };
        
    struct pair pairs_array[] = {
        [red]           =  {0.9611,     1},
        [blue]          =  {0.3499,     0.5075},
        [light_blue]    =  {0.2236,     0.2609},
        [green]         =  {0.1796,     0.2220},
        [white]         =  {0.0712,     0.0782},
        [yellow]        =  {0.0619,     0.0668},
        [black]         =  {0.0547,     0.0614},
        [pink]          =  {0.0446,     0.0518},
        [orange]        =  {0.0205,     0.0315}
    }; 
    
    for (int i = 0; i < number_of_colors; i++){
        if ((pairs_array[i].max > color_value) && (pairs_array[i].min < color_value)) 
            return i;
    }
     return 10; //if nothing is found return error
}
