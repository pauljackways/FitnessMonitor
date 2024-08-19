// *******************************************************
// 
// buttons4.c
//
// Support for a set of FOUR specific buttons on the Tiva/Orbit.
// ENCE361 sample code.
// The buttons are:  UP and DOWN (on the Orbit daughterboard) plus
// LEFT and RIGHT on the Tiva.
//
// Note that pin PF0 (the pin for the RIGHT pushbutton - SW2 on
//  the Tiva board) needs special treatment - See PhilsNotesOnTiva.rtf.
//
// P.J. Bones UCECE
// Last modified:  7.2.2018
// 
// *******************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "buttons4.h"


// *******************************************************
// Globals to module
// *******************************************************
static bool but_state[NUM_BUTS];	// Corresponds to the electrical state
static uint8_t but_count[NUM_BUTS];
static bool but_flag[NUM_BUTS];
static bool but_normal[NUM_BUTS];   // Corresponds to the electrical state

static uint8_t press_count[NUM_BUTS];
static uint8_t unpress_count[NUM_BUTS];
static uint8_t test_count[NUM_BUTS];
static uint8_t but_count[NUM_BUTS];
static bool already_pressed[NUM_BUTS];
static bool prevent_long[NUM_BUTS];


// Function which returns current button state. Abstracts GPIO function calls for other modules
bool isDown(uint8_t butName) {
    return but_state[butName];
}


// Function which returns current button state. Abstracts GPIO function calls for other modules
bool isUnpressed(uint8_t butName) {
    return but_state[butName] == but_normal[butName];
}

// *******************************************************
// initButtons: Initialise the variables associated with the set of buttons
// defined by the constants in the buttons2.h header file.
void
initButtons (void)
{
	int i;

	// UP button (active HIGH)
    SysCtlPeripheralEnable (UP_BUT_PERIPH);
    GPIOPinTypeGPIOInput (UP_BUT_PORT_BASE, UP_BUT_PIN);
    GPIOPadConfigSet (UP_BUT_PORT_BASE, UP_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    but_normal[UP] = UP_BUT_NORMAL;
	// DOWN button (active HIGH)
    SysCtlPeripheralEnable (DOWN_BUT_PERIPH);
    GPIOPinTypeGPIOInput (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    GPIOPadConfigSet (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    but_normal[DOWN] = DOWN_BUT_NORMAL;
    // LEFT button (active LOW)
    SysCtlPeripheralEnable (LEFT_BUT_PERIPH);
    GPIOPinTypeGPIOInput (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOPadConfigSet (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    but_normal[LEFT] = LEFT_BUT_NORMAL;
    // RIGHT button (active LOW)
      // Note that PF0 is one of a handful of GPIO pins that need to be
      // "unlocked" before they can be reconfigured.  This also requires
      //      #include "inc/tm4c123gh6pm.h"
    SysCtlPeripheralEnable (RIGHT_BUT_PERIPH);
    //---Unlock PF0 for the right button:
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
    GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    GPIOPinTypeGPIOInput (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOPadConfigSet (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    but_normal[RIGHT] = RIGHT_BUT_NORMAL;

	for (i = 0; i < NUM_BUTS; i++)
	{
		but_state[i] = but_normal[i];
		but_count[i] = 0;
		but_flag[i] = false;
	}
}

// *******************************************************
// updateButtons: Function designed to be called regularly. It polls all
// buttons once and updates variables associated with the buttons if
// necessary.  It is efficient enough to be part of an ISR, e.g. from
// a SysTick interrupt.
// Debounce algorithm: A state machine is associated with each button.
// A state change occurs only after NUM_BUT_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_BUT_POLLS according to the polling rate.
void
updateButtons (void)
{
	bool but_value[NUM_BUTS];
	int i;
	
	// Read the pins; true means HIGH, false means LOW
	but_value[UP] = (GPIOPinRead (UP_BUT_PORT_BASE, UP_BUT_PIN) == UP_BUT_PIN);
	but_value[DOWN] = (GPIOPinRead (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN) == DOWN_BUT_PIN);
    but_value[LEFT] = (GPIOPinRead (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN) == LEFT_BUT_PIN);
    but_value[RIGHT] = (GPIOPinRead (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN) == RIGHT_BUT_PIN);
	// Iterate through the buttons, updating button variables as required
	for (i = 0; i < NUM_BUTS; i++)
	{
        if (but_value[i] != but_state[i])
        {
        	but_count[i]++;
        	if (but_count[i] >= NUM_BUT_POLLS)
        	{
        		but_state[i] = but_value[i];
        		but_flag[i] = true;	   // Reset by call to checkButton()
        		but_count[i] = 0;
        	}
        }
        else
        	but_count[i] = 0;
	}
}

void resetButton(uint8_t butName) {
    press_count[butName] = 0;
    unpress_count[butName] = 0;
    test_count[butName] = 0;
    already_pressed[butName] = false;
    prevent_long[butName] = false;
}


ButtonPressType checkPressType(uint8_t butName) // Returns button press state on state change
{
    updateButtons();
    //ButtonPressType pressType = NO_CHANGE; // Initialised instead of just returned so as to avoid concurrency issues
    if (but_flag[butName] == true) {
        but_flag[butName] = false;
        prevent_long[butName] = false;
        if (!isUnpressed(butName)) { 
            press_count[butName] = 0;
            test_count[butName] = 0;
            if (unpress_count[butName] > 0 && unpress_count[butName] < TEST_DURATION) {
                resetButton(butName);
                return DOUBLE;
            }
        } else {
            unpress_count[butName] = 0;
        }
        return NO_CHANGE;
    } else {
        if (!isUnpressed(butName) && !prevent_long[butName]) {
            test_count[butName]++;
            press_count[butName]++;
            if (press_count[butName] > TEST_DURATION * 4) {
                resetButton(butName);
                return LONG;
            }
        } else {
            test_count[butName]++;
            unpress_count[butName]++;
            if (test_count[butName] > TEST_DURATION && unpress_count[butName] > 0 && unpress_count[butName] < TEST_DURATION && press_count[butName] > 0 && press_count[butName] < TEST_DURATION) {
                resetButton(butName);
                return SHORT;
            }
            if (unpress_count[butName] > TEST_DURATION * 8) {
                unpress_count[butName] = TEST_DURATION * 6;
            }

        }
        return NO_CHANGE;
    }

}





