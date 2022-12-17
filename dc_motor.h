#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

#define Brake_light LATDbits.LATD4  //Brake lights
#define Right_signal LATHbits.LATH0 //Right turn signal
#define Left_signal LATFbits.LATF0  // Left turn signal
#define Low_beams LATHbits.LATH1    // Low intensity Lights
#define Main_beams LATDbits.LATD3   // turn(mL, mR, 90, LEFT); Main Beams

#define LEFT 0
#define RIGHT 1

#define turn_gain_left 1540
#define turn_gain_right 1540
#define speed 70

// defines amount of loops in turn function lower gain means smaller turn

//THERE IS A NEED FOR BOTH FORWARD AND BACKWARD CALIBRATION!

typedef struct DC_motor_struct { //definition of DC_motor structure
    int power;         //motor power, out of 100
    int direction;     //motor direction, forward(1), reverse(0)
    int brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
void initDCmotorsPWM(unsigned int PWMperiod); // function to setup PWM
void initDCmotorsvar(DC_motor *motorL,DC_motor *motorR,int PWMcycle);
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void stop_slow(DC_motor *mL, DC_motor *mR);
void turnLeft(DC_motor *mL, DC_motor *mR);
void turnRight(DC_motor *mL, DC_motor *mR);
void reverse(DC_motor *mL, DC_motor *mR);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR,int power);
void fullSpeedReverse(DC_motor *mL, DC_motor *mR, int power);
void start_motors();
void turn(DC_motor *mL, DC_motor *mR, int deg, int dir);
//move forward one square
void forward(DC_motor *mL, DC_motor *mR);
void forward_to_parallel(DC_motor *mL, DC_motor *mR);
void reverse_to_parallel(DC_motor *mL, DC_motor *mR);

#endif
