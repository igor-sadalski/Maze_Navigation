#ifndef RETURN_H
#define	RETURN_H

#define number_of_action 50
#include "dc_motor.h"

typedef struct struct_stack{
    int stack[number_of_action];               //array to store past values
    int top;                               //after first action you will start at
    int time[number_of_action];
} action_stack;

void memory_pushAction(int action, action_stack *a_stack);        // Push element to the top of the stack
void memory_pushTime(int time, action_stack *a_stack);       // push the most recent time
int memory_popAction(action_stack *a_stack);                // Remove and return the top most element of the stack
int memory_popTime(action_stack *a_stack);    // returns the time from the stack
int stack_empty (action_stack *a_stack);    //go back in a reverse order
void buggy_makeReverseActions(DC_motor *mL, DC_motor *mR,action_stack *a_stack); // reverse through the maze

#endif	/* RETURN_H */