/******************************************************************************
 * File: frdmk64f-synchsm_template.c
 * Author: Justin Fababier
 * Date Created: 11/14/2024
 * Description: 
 *   The provided code sets up a simple cooperative multitasking system using a 
 *   Periodic Interrupt Timer (PIT) to run synchronous finite-state machines.
 *****************************************************************************/

#include "fsl_device_registers.h"

// Macro definition for the number of tasks in the system
#define NUM_TASKS 2

// Task structure definition
typedef struct _task {
    signed char state;              // Task's current state
    unsigned long period;           // Task period (in ms)
    unsigned long elapsedTime;      // Time elapsed since last task tick (in ms)
    int (*TickFct)(int);            // Pointer to the task's state machine function
} task;

// Array to hold the tasks in the system
task tasks[NUM_TASKS];

// Task periods (in ms). Adjust based on desired behavior.
const unsigned long GCD_PERIOD = 1; // Common divisor period
const unsigned long SM1_PERIOD = 1; // State machine 1 period
const unsigned long SM2_PERIOD = 1; // State machine 2 period

// PIT0 Interrupt Service Routine (ISR)
void PIT0_IRQHandler(void) {
    // Clear the interrupt flag for PIT channel 0
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;

    // Iterate over all tasks and handle execution
    for (unsigned char i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].elapsedTime >= tasks[i].period) {
            // Execute the task's tick function and update its state
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            // Reset elapsed time for the task
            tasks[i].elapsedTime = 0;
        }
        // Increment elapsed time by the GCD_PERIOD
        tasks[i].elapsedTime += GCD_PERIOD;
    }
}

// Function to configure the PIT timer for the desired period
void TimerSet(unsigned long period) {
    // Enable clock for PIT module
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

    // Enable the PIT module
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;

    // Calculate the timer load value for a 1 ms interrupt
    uint32_t timerLoadValue = (period * (SystemCoreClock / 1000)) - 1;
    PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(timerLoadValue);

    // Enable interrupts for PIT channel 0
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
}

// Function to start the PIT timer
void TimerOn() {
    // Start the PIT timer on channel 0
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
    // Enable the PIT interrupt in the NVIC
    NVIC_EnableIRQ(PIT0_IRQn);
}

// State machine enumerations and declarations
enum SM1_Tick { SM1_INIT, SM1_S1 }; // States for state machine 1
enum SM2_Tick { SM2_INIT, SM2_S1 }; // States for state machine 2
int SM1_Tick(int state);            // Tick function for state machine 1
int SM2_Tick(int state);            // Tick function for state machine 2

// State machine 1 tick function
int SM1_Tick(int state) {
    // State transitions
    switch (state) {
        case SM1_INIT:
            state = SM1_S1;         // Transition to state S1
            break;

        case SM1_S1:
            state = SM1_S1;         // Stay in state S1
            break;

        default:
            break;
    }

    // State actions
    switch (state) {
        case SM1_INIT:
            break;                  // Actions for SM1_INIT

        case SM1_S1:
            break;                  // Actions for SM1_S1

        default:
            break;
    }
	
    return state;
}

// State machine 2 tick function
int SM2_Tick(int state) {
    // State transitions
    switch (state) {
        case SM2_INIT:
            state = SM2_S1;         // Transition to state S1
            break;

        case SM2_S1:
            state = SM2_S1;         // Stay in state S1
            break;

        default:
            break;
    }

    // State actions
    switch (state) {
        case SM2_INIT:
            break;                  // Actions for SM2_INIT

        case SM2_S1:
            break;                  // Actions for SM2_S1

        default:
            break;
    }

    return state;
}

// Main function
int main(void) {
    // Initialize hardware (clock-gating, GPIO, etc.)
    // Add relevant initialization code as needed.

    // Configure Task 1
    unsigned char i = 0;
    tasks[i].state = SM1_INIT;      // Set initial state
    tasks[i].period = SM1_PERIOD;   // Set task period
    tasks[i].elapsedTime = tasks[i].period; // Initialize elapsed time
    tasks[i].TickFct = &SM1_Tick;   // Set tick function

    // Configure Task 2
    i++;
    tasks[i].state = SM2_INIT;      // Set initial state
    tasks[i].period = SM2_PERIOD;   // Set task period
    tasks[i].elapsedTime = tasks[i].period; // Initialize elapsed time
    tasks[i].TickFct = &SM2_Tick;   // Set tick function

    // Configure and start the timer
    TimerSet(GCD_PERIOD);           // Set timer period
    TimerOn();                      // Start timer

    // Infinite loop to allow PIT interrupts to execute tasks
    while (1) {}

    return 0;                       // Program should never reach here
}
