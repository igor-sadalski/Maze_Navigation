#include <xc.h>
#include "Actions.h"
#include "dc_motor.h"
#include "color.h"

extern int second;
extern int end;

void memory_pushAction(int action, action_stack *a_stack) { //increase the pointer
    a_stack->stack[a_stack->top] = action; //save the most recent action
}

void memory_pushTime(int time, action_stack *a_stack) { //increase the pointer
    a_stack->time[++a_stack->top] = time; //save the most recent action
}

int memory_popAction(action_stack *a_stack) {
    int top = a_stack->stack[a_stack->top]; //save the most recent action
    return top;
}

int memory_popTime(action_stack *a_stack) {
    int time_top = a_stack->time[a_stack->top--]; //save the most recent action
    return time_top;
}

int stack_empty(action_stack *a_stack) {
    int empty;
    if (a_stack->top == -1) {
        empty = 1;
    } else {
        empty = 0;
    }
    return empty;
}

void buggy_makeColorActions(DC_motor *mL, DC_motor *mR,action_stack *stack)
{
    
    float colorValue = color_getColorValue(); 
    int isColor = color_getColor(colorValue);
    
    
    /*  
     * 0 - red
     * 1 - blue
     * 2 - light_blue
     * 3 - green
     * 4 - orange
     * 5 - white
     * 6 - yellow
     * 7 - black
     * 8 - pink
     * 9 - repeat
     * 10 - error
     */    
    //use for color testing
    //after each action do for a while full speed ahead to be perfectly aligned with the wall
    switch (isColor)
    {
        case red:
            reverse(mL,mR); // reverse half a square to get a better turn space
            // turn right 90
            memory_pushAction(red, stack);
            turn(mL, mR, 90, RIGHT);    // 1 is right 
            break;
        case blue:
            reverse(mL,mR); // reverse half a square to get a better turn space
            //turn 180
            memory_pushAction(blue, stack);
            turn(mL, mR, 180, LEFT);   // 0 is left.
            break;
        case light_blue: 
            reverse(mL,mR); // reverse half a square to get a better turn space
            //turn left 135
            memory_pushAction(light_blue,stack);
            turn(mL, mR, 135, LEFT);
            break;
        case green:
            reverse(mL,mR); // reverse half a square to get a better turn space
            //TURN LEFT 90
            memory_pushAction(green,stack);
            turn(mL, mR, 90, LEFT);
            break;
        case orange:
            reverse(mL,mR); // reverse half a square to get a better turn space
            //turn right 135
            memory_pushAction(orange,stack);
            turn(mL, mR, 135, RIGHT);
            break;
        case white:
            reverse(mL,mR); // reverse half a square to get a better turn space
            // maze end - retrace the steps back and come home
            memory_pushAction(white,stack);
            buggy_makeReverseActions(mL, mR, stack);
            break;
        case yellow:
            reverse(mL,mR); // reverse half a square to get a better turn space
            // reverse 1 square and turn right 90
            memory_pushAction(yellow,stack);
            reverse(mL,mR);
            reverse(mL,mR);
            turn(mL, mR, 90, RIGHT);
            break;
        case black:
             memory_pushAction(black,stack);
            // enter search mode
            break;
        case pink:    //pink is not working
            reverse(mL,mR); // reverse half a square to get a better turn space
            // reverse 1 square and turn left 90
            memory_pushAction(pink,stack);
            reverse(mL,mR);
            reverse(mL,mR);
            turn(mL, mR, 90, LEFT);
            break;      //pink is not working
        case repeat:  break;
        default: break;
    }
}

void buggy_makeReverseActions(DC_motor *mL, DC_motor *mR, action_stack *stack) {
    led_off();
    Main_beams = 1;
    Low_beams = 1;
    while (!stack_empty(stack)) {
        switch (memory_popAction(stack)) {
            case red:
                forward(mL, mR);
                // turn left 90
                turn(mL, mR, 90, LEFT); // 1 is right
                break;
            case blue:
                forward(mL, mR);
                //turn 180
                turn(mL, mR, 180, RIGHT); // 0 is left
                break;
            case light_blue:
                forward(mL, mR);
                //turn right 135
                turn(mL, mR, 135, RIGHT);
                break;
            case green:
                forward(mL, mR);
                //TURN right 90
                turn(mL, mR, 90, RIGHT);
                break;
            case orange:
                forward(mL, mR);
                //turn left 135
                turn(mL, mR, 135, LEFT);
                break;
            case white:
                // do nothing other than reverse
                break;
            case yellow:
                forward(mL, mR);
                //forward(mL, mR); //same way move a bit so rotation is easier
                turn(mL, mR, 90, LEFT); 
                forward(mL, mR);
                forward(mL, mR);
                forward_to_parallel(mL,mR);
                break;
            case black:
                // enter search mode
                break;
            case pink:
                forward(mL, mR);
                turn(mL, mR, 90, RIGHT); 
                forward(mL, mR);
                forward(mL, mR);
                forward_to_parallel(mL,mR);
                break;
            case repeat: break;
            default: break;
        }
       second = 0;
        int run_time = memory_popTime(stack);
        while (second <= run_time) {
            fullSpeedReverse(mL, mR, speed);
        }
        stop_slow(mL, mR);
        reverse_to_parallel(mL,mR); // ram into wall to make parrell
    }
    Brake_light = 1;
    end = 1; //would be nicer to just trigger an interrupt in the system
}