/******************************************************************************
 * File: frdmk64f-synchsm_template.c
 * Author: Justin Fababier
 * Date Created: 11/14/2024
 * Description: 
 *   The provided code sets up a simple cooperative multitasking system using a 
 *   Periodic Interrupt Timer (PIT) to run synchronous state machines.
 * 
 * Usage:
 *   [Any specific usage instructions or prerequisites for this file, e.g., 
 *   required libraries, classes or functions that depend on this header, etc.]
 *   
 * Version History:
 *   - [Version 1.0] - [11/14/2024]: Initial creation.
 * 
 * Notes:
 *   - This code is based off the synchronous state machine design taught at
 *     University of California, Riverside's EE 120B course.
 *   - The template is designed to use 1ms-per-tick.
 *****************************************************************************/


#include "fsl_device_registers.h"

#define NUM_TASKS 2

typedef struct _task{
	signed char state;              // Task's current state
	unsigned long period;           // Task period
	unsigned long elapsedTime;      // Time elapsed since last task tick
	int (*TickFct)(int);            // Task tick function
} task;

task tasks[NUM_TASKS];              // Declared task array

// Periods for each task (in ms)
const unsigned long GCD_PERIOD = 1;
const unsigned long SM1_PERIOD = 1;
const unsigned long SM2_PERIOD = 1;

// PIT0 ISR
void PIT0_IRQHandler(void) {
    // Clear the interrupt flag
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;

    // Iterate over all tasks
    for (unsigned char i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].elapsedTime >= tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state); // Execute task tick function
            tasks[i].elapsedTime = 0;                          // Reset elapsed time
        }
        tasks[i].elapsedTime += GCD_PERIOD;                    // Increment elapsed time
    }
}

// Function to set up the PIT timer for a 1 ms period
void TimerSet(unsigned long period) {
    // Enable clock for PIT
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

    // Enable PIT module
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;

    // Set the timer load value for a 1 ms interrupt
    uint32_t timerLoadValue = (period * (SystemCoreClock / 1000)) - 1;
    PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(timerLoadValue);

    // Enable timer interrupts for PIT channel 0
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
}

// Function to start the PIT timer
void TimerOn() {
    // Start the timer
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
    // Enable PIT interrupt in NVIC
    NVIC_EnableIRQ(PIT0_IRQn);
}

// SM enumerations and declarations
enum SM1_Tick{SM1_INIT, SM1_S1};
enum SM2_Tick{SM2_INIT, SM2_S1};
int SM1_Tick(int state);
int SM2_Tick(int state);

int SM1_Tick(int state) {           // State machine 1
    switch (state) {                // State transitions start
        case SM1_INIT:
            state = SM1_S1;
            break;

        case SM1_S1:
            state = SM1_S1;
            break;

        default:
            break;
    }                               // State transitions end

    switch (state) {                // State actions start
        case SM1_INIT:
            state = SM1_S1;
            break;

        case SM1_S1:
            state = SM1_S1;
            break;

        default:
            break;
    }                               // State actions end
}

int SM2_Tick(int state) {           // State machine 2
    switch (state) {                // State transitions start
        case SM2_INIT:
            state = SM1_S1;
            break;

        case SM2_S1:
            state = SM2_S1;
            break;

        default:
            break;
    }                               // State transitions end

    switch (state) {                // State actions start
        case SM2_INIT:
            state = SM1_S1;
            break;

        case SM2_S1:
            state = SM2_S1;
            break;

        default:
            break;
    }                               // State actions end
}

int main(void) {
    // Initialize clock-gating

    // Initialize pins GPIO

    // Initialize I/O mode

    // Task 1
    unsigned char i = 0;
    tasks[i].state = SM1_INIT;
    tasks[i].period = SM1_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &SM1_Tick;

    // Task 2
    i++;
    tasks[i].state = SM2_INIT;
    tasks[i].period = SM2_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &SM2_Tick;

    // Timer functions
    TimerSet(GCD_PERIOD);
    TimerOn();

    while(1) {}

    return 0;
}
