#ifndef SWITCHES_H_
#define SWITCHES_H_

// *******************************************************
// Header file for switch reading module
// Daniel Rabbidge
//
// FitnessThur9-1
//
// *******************************************************

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// Constants
//*****************************************************************************
//enum SWNames {UP = 0, DOWN, LEFT, RIGHT, NUM_BUTS};
enum SWStates {SW_UP = 0, SW_DOWN, SW_NO_CHANGE};
// SW1
#define SW1_PERIPH  SYSCTL_PERIPH_GPIOA
#define SW1_PORT_BASE  GPIO_PORTA_BASE
#define SW1_PIN  GPIO_PIN_7
#define SW1_NORMAL  false


#define NUM_SW_POLLS 3

// Debounce algorithm: A state machine is associated with the switch.
// A state change occurs only after NUM_SW_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_SW_POLLS according to the polling rate.

// *******************************************************
// initSwitch: Initialise the variables associated with the switch
// defined by the constants above.
void initSwitch (void);


// *******************************************************
// updateSwitch: Function designed to be called regularly. It polls the switch
// and updates variables associated with the switch if
// necessary.
void updateSwitch (void);


// *******************************************************
// Function that returns the state of the switch. Used to abstract GPIO function calls in other modules
bool isSwitchUp(void);

#endif /*SWITCHES_H_*/
