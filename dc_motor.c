#include <xc.h>
#include "dc_motor.h"

// function initialise motor varialbes from the strucutre

void initDCmotorsvar(DC_motor *motorL, DC_motor *motorR, int PWMcycle) {
    motorL->power = 0; //zero power to start
    motorL->direction = 1; //set default motor direction
    motorL->brakemode = 1; // brake mode (slow decay)
    motorL->posDutyHighByte = (unsigned char *) (&CCPR1H); //store address of CCP1 duty high byte
    motorL->negDutyHighByte = (unsigned char *) (&CCPR2H); //store address of CCP2 duty high byte
    motorL->PWMperiod = PWMcycle; //store PWMperiod for motor (value of T2PR in this case)

    //same for motorR but different CCP registers
    motorR->power = 0; //zero power to start
    motorR->direction = 1; //set default motor direction
    motorR->brakemode = 1; // brake mode (slow decay)
    motorR->posDutyHighByte = (unsigned char *) (&CCPR3H); //store address of CCP3 duty high byte
    motorR->negDutyHighByte = (unsigned char *) (&CCPR4H); //store address of CCP4 duty high byte
    motorR->PWMperiod = PWMcycle; //store PWMperiod for motor (value of T2PR in this case)
    initDCmotorsPWM(PWMcycle); // sets up PWM for all motors
}

// function initialise T2 and CCP for DC motor control

void initDCmotorsPWM(unsigned int PWMperiod) {
    //initialise your TRIS and LAT registers for PWM  
    // PWM-A Left side motor
    TRISEbits.TRISE2 = 0;
    LATEbits.LATE2 = 0;

    // PWM-B Left side motor
    TRISEbits.TRISE4 = 0;
    LATEbits.LATE4 = 0;

    // PWM-C right side motor
    TRISCbits.TRISC7 = 0;
    LATCbits.LATC7 = 0;

    // PWM-D Right side motor
    TRISGbits.TRISG6 = 0;
    LATGbits.LATG6 = 0;

    // initialise lighting system
    TRISDbits.TRISD4 = 0; // Brake lights
    TRISHbits.TRISH0 = 0; // Right turn signal
    TRISFbits.TRISF0 = 0; // Left turn signal
    TRISHbits.TRISH1 = 0; // Low intensity Lights
    TRISDbits.TRISD3 = 0; // Main Beams

    // initialise state of lights
    Brake_light = 0; //Brake lights
    Right_signal = 0; //Right turn signal
    Left_signal = 0; // Left turn signal
    Low_beams = 0; // Low intensity Lights
    Main_beams = 0; // Main Beams

    //configure PPS to map CCP modules to pins
    RE2PPS = 0x05; //CCP1 on RE2
    RE4PPS = 0x06; //CCP2 on RE4
    RC7PPS = 0x07; //CCP3 on RC7
    RG6PPS = 0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS = 0b011; // 1:8 prescaler
    T2HLTbits.MODE = 0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS = 0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/8 -1 = 199
    T2PR = PWMperiod; //Period reg 10kHz base period
    T2CONbits.ON = 1;

    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H = 0;
    CCPR2H = 0;
    CCPR3H = 0;
    CCPR4H = 0;
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL = 0;
    CCPTMRS0bits.C2TSEL = 0;
    CCPTMRS0bits.C3TSEL = 0;
    CCPTMRS0bits.C4TSEL = 0;

    //configure each CCP
    CCP1CONbits.FMT = 1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE = 0b1100; //PWM mode  
    CCP1CONbits.EN = 1; //turn on

    CCP2CONbits.FMT = 1; // left aligned
    CCP2CONbits.CCP2MODE = 0b1100; //PWM mode  
    CCP2CONbits.EN = 1; //turn on

    CCP3CONbits.FMT = 1; // left aligned
    CCP3CONbits.CCP3MODE = 0b1100; //PWM mode  
    CCP3CONbits.EN = 1; //turn on

    CCP4CONbits.FMT = 1; // left aligned
    CCP4CONbits.CCP4MODE = 0b1100; //PWM mode  
    CCP4CONbits.EN = 1; //turn on

}

// function to set CCP PWM output from the values in the motor structure

void setMotorPWM(DC_motor *m) {
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor

    if (m->brakemode) {
        posDuty = m->PWMperiod - ((unsigned int) (m->power)*(m->PWMperiod)) / 100; //inverted PWM duty
        negDuty = m->PWMperiod; //other side of motor is high all the time
    } else {
        posDuty = 0; //other side of motor is low all the time
        negDuty = ((unsigned int) (m->power)*(m->PWMperiod)) / 100; // PWM duty
    }

    if (m->direction) {
        *(m->posDutyHighByte) = posDuty; //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte) = negDuty;
    } else {
        *(m->posDutyHighByte) = negDuty; //do it the other way around to change direction
        *(m->negDutyHighByte) = posDuty;
    }
}

//function to stop the robot gradually 

void stop(DC_motor *mL, DC_motor *mR) {
    Brake_light = 1;

    mL->brakemode = 1;
    mR->brakemode = 1;

    int moving = 1;
    while (moving) {
        if (mR->power > 0) {
            mR->power = (mR->power) - 20;
        }
        if (mL->power > 0) {
            mL->power = (mL->power) - 20;
        }
        if ((mR->power) <= 0 && (mL->power) <= 0) {
            moving = 0;
            mL->power = 0;
            mR->power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        __delay_us(100);
    }
    Brake_light = 0;
}

// slow stop function
void stop_slow(DC_motor *mL, DC_motor *mR) {
    Brake_light = 1;

    mL->brakemode = 0;
    mR->brakemode = 0;

    int moving = 1;
    while (moving) {
        if (mR->power > 0) {
            mR->power = (mR->power) - 10;
        }
        if (mL->power > 0) {
            mL->power = (mL->power) - 10;
        }
        if ((mR->power) <= 0 && (mL->power) <= 0) {
            moving = 0;
            mL->power = 0;
            mR->power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        __delay_ms(50);
    }
    Brake_light = 0;
}

//function to make the robot turn left 

void turnLeft(DC_motor *mL, DC_motor *mR) {
    Left_signal = 1; //turns on turning signal
    int deg = turn_gain_left; //turn gain needed to turn 45 degrees

    // set wheel direction
    mR->direction = 1;
    mL->direction = 0;
    
    int time = 0;
    
    while (deg) {
        if (mR->power <= 80) {
            mR->power = mR->power + 20;
        }
        if (mL->power <= 80) {
            mL->power = mL->power + 20;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        Left_signal = !Left_signal; // flash the indicator
        __delay_us(1);
        deg = deg - 1;
        if (deg <= 0) {
            deg = 0;
        }
    }
    stop(mL, mR);
    Left_signal = 0; //turns off turning signal
}

//function to make the robot turn right 45 degrees

void turnRight(DC_motor *mL, DC_motor *mR) {
    int deg = turn_gain_right; 
    Right_signal = 1; //turns on turning signal

    // set wheel direction
    mR->direction = 0;
    mL->direction = 1;

    while (deg) {
        if (mR->power <= 80) {
            mR->power = mR->power + 20;
        }
        if (mL->power <= 80) {
            mL->power = mL->power + 20;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        Right_signal = !Right_signal;
        deg = deg - 1;
        __delay_us(1);
        if (deg <= 0) {
            deg = 0;
        }
    }
    stop(mL, mR);
    Right_signal = 0; //turns off turning signal
}

//function to make the robot go straight

void fullSpeedAhead(DC_motor *mL, DC_motor *mR, int power) {
    // power is 0 to 100
    mR->direction = 1;
    mL->direction = 1;

    while (power) {
        if (mR->power <= power) {
            mR->power = (mR->power) + 20;
        }
        if (mL->power <= power) {
            mL->power = (mL->power) + 20;
        }
        // exit when power = desired power to do this set power = 0
        if ((mR->power) >= power && (mL->power) >= power) {
            power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
            __delay_ms(100);
    }
}

//function to make the robot go straight

void fullSpeedReverse(DC_motor *mL, DC_motor *mR, int power) {
    // power is 0 to 100
    mR->direction = 0;
    mL->direction = 0;

    while (power) {
        if (mR->power <= power) {
            mR->power = (mR->power) + 20;
        }
        if (mL->power <= power) {
            mL->power = (mL->power) + 20;
        }
        // exit when power = desired power to do this set power = 0
        if ((mR->power) >= power && (mL->power) >= power) {
            power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        __delay_ms(100);
    }
}



// function to freeze everything until unplugged

void start_motors() {
    TRISFbits.TRISF3 = 1; // setting pin as input
    LATFbits.LATF3 = 1; // start at 1
    TRISHbits.TRISH3 = 0; // setting pin as input
    LATHbits.LATH3 = 1; // start at zero
    ANSELFbits.ANSELF3 = 0; // turn off analogue input on pin

    int plugged_in = 1;
    while (plugged_in) { //empty while loop (wait for button press)
        LATHbits.LATH3 = !LATHbits.LATH3; // start at zero
        if (!PORTFbits.RF3) {
            plugged_in = 0;
        }
        __delay_ms(100); // call built in delay function  
    }
}

void turn(DC_motor *mL, DC_motor *mR, int deg, int dir) {
    if (dir == 0) {
        while (deg) {
            deg = deg - 45;
            turnLeft(mL, mR);
            __delay_ms(100);
        }
    }
    if (dir == 1) {
        while (deg) {
            turnRight(mL, mR);
            deg = deg - 45;
            __delay_ms(100);
            }
        }
}

void reverse(DC_motor *mL, DC_motor *mR){
    // power is 0 to 100
    mR->direction = 0;
    mL->direction = 0;

    int power = 60;
    
    while (power) {
        if (mR->power <= power) {
            mR->power = (mR->power) + 20;
        }
        if (mL->power <= power) {
            mL->power = (mL->power) + 20;
        }
        // exit when power = desired power to do this set power = 0
        if ((mR->power) >= power && (mL->power) >= power) {
            power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        __delay_ms(170);
    }
    __delay_ms(100);
    stop_slow(mL,mR);
}

void forward(DC_motor *mL, DC_motor *mR){
    // power is 0 to 100
    mR->direction = 1;
    mL->direction = 1;

    int power = 60;
    
    while (power) {
        if (mR->power <= power) {
            mR->power = (mR->power) + 20;
        }
        if (mL->power <= power) {
            mL->power = (mL->power) + 20;
        }
        // exit when power = desired power to do this set power = 0
        if ((mR->power) >= power && (mL->power) >= power) {
            power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        __delay_ms(180);
    }
    __delay_ms(100);
    stop_slow(mL,mR);
}

void forward_to_parallel(DC_motor *mL, DC_motor *mR){
    // power is 0 to 100
    mR->direction = 1;
    mL->direction = 1;

    int power = 100;
    
    while (power) {
        if (mR->power <= power) {
            mR->power = (mR->power) + 20;
        }
        if (mL->power <= power) {
            mL->power = (mL->power) + 20;
        }
        // exit when power = desired power to do this set power = 0
        if ((mR->power) >= power && (mL->power) >= power) {
            power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        __delay_ms(100);
    }
    __delay_ms(100);
    stop_slow(mL,mR);
}

void reverse_to_parallel(DC_motor *mL, DC_motor *mR){
    // power is 0 to 100
    mR->direction = 0;
    mL->direction = 0;

    int power = 100;
    
    while (power) {
        if (mR->power <= power) {
            mR->power = (mR->power) + 20;
        }
        if (mL->power <= power) {
            mL->power = (mL->power) + 20;
        }
        // exit when power = desired power to do this set power = 0
        if ((mR->power) >= power && (mL->power) >= power) {
            power = 0;
        }
        setMotorPWM(mR);
        setMotorPWM(mL);
        __delay_ms(100);
    }
    __delay_ms(100);
    stop_slow(mL,mR);
}