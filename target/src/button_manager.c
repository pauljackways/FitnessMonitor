/*
 * Button_management.c
 *
 * Modifies the device's state according to the user's button and switch input
 *
 *  Created on: 31/03/2022
 *      Author: Daniel Rabbidge
 *
 *  FitnessThur9-1
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"
#include "buttons4.h"
#include "display_manager.h"
#include "deviceState.h"
#include "button_manager.h"
#include "step_counter_main.h"
#include "switches.h"



//********************************************************
// Constants and static vars
//********************************************************
#define LONG_PRESS_CYCLES 20

static uint16_t longPressCount = 0;
static bool allowLongPress = true;
static uint8_t but_press[NUM_BUTS] = {0};



//********************************************************
// Init buttons and switch I/O handlers
//********************************************************

void btnInit(void)
{
    initButtons();
    initSwitch();
}

// Possible key bindings

// Cycle through unit options
void bindUnits(uint8_t butName, uint8_t butPressType) {
        // Changing units
    if (but_press[butName] == butPressType) {
        // Can't use mod, as enums behave like an unsigned int, so (0-1)%n != n-1
        displayUnits_t currentDisplayUnits = getDisplayUnits();
        if (currentDisplayUnits > 0) {
            setDisplayUnits(currentDisplayUnits - 1);
        } else {
            setDisplayUnits(UNITS_NUM_TYPES - 1);
        }
    }
}

//Increase steps (debug)
void bindStepsUp(uint8_t butName, uint8_t butPressType) {
    if (but_press[butName] == butPressType) {
        setStepsTaken(getStepsTaken() + DEBUG_STEP_INCREMENT);
    }
}

//Decrease steps (debug)
void bindStepsDown(uint8_t butName, uint8_t butPressType) {
    if (but_press[butName] == butPressType) {
        uint32_t stepsTaken = getStepsTaken();
        if (stepsTaken >= DEBUG_STEP_DECREMENT) {
            setStepsTaken(stepsTaken - DEBUG_STEP_DECREMENT);
        } else {
            setStepsTaken(0);
        }
    }
}

//Cycle forward through displays
void bindNavUp(uint8_t butName, uint8_t butPressType) {
    displayMode_t currentDisplayMode = getDisplayMode();
    if (but_press[butName] == butPressType) {
        setDisplayMode((currentDisplayMode + 1) % DISPLAY_NUM_STATES);      //flicker when pressing button

    }
}

//Cycle backward through displays
void bindNavDown(uint8_t butName, uint8_t butPressType) {
    displayMode_t currentDisplayMode = getDisplayMode();
    if (but_press[butName] == butPressType) {
        // Can't use mod, as enums behave like an unsigned int, so (0-1)%n != n-1
        if (currentDisplayMode > 0) {
            setDisplayMode(currentDisplayMode - 1);
        } else {
            setDisplayMode(DISPLAY_NUM_STATES - 1);
        }
    }
}

//Set goal
void bindSetGoal(uint8_t butName, uint8_t butPressType) {
    if (but_press[butName] == butPressType) {
        setCurrentGoal(getNewGoal());
    }
}

// RESET
void bindReset(uint8_t butName, uint8_t butPressType) {
    if (but_press[butName] == butPressType) {
        setStepsTaken(0);
        flashMessage("Reset!");
    }
}



//Screen options
void displaySteps(void) {
    bindUnits(UP, SHORT);
    bindNavUp(LEFT, SHORT);
    bindNavDown(RIGHT, SHORT);
    bindReset(DOWN, DOUBLE);

}

void displayDistance(void) {
    bindUnits(UP, SHORT);
    bindNavUp(LEFT, SHORT);
    bindNavDown(RIGHT, SHORT);
    bindReset(DOWN, DOUBLE);
}

void displaySetGoal(void) {
    bindUnits(UP, SHORT);
    bindNavUp(LEFT, SHORT);
    bindNavDown(RIGHT, SHORT);
    bindSetGoal(DOWN, SHORT);
    bindReset(DOWN, LONG);
}

void debugMode(void) {
    // TEST MODE OPERATION
    bindNavUp(LEFT, SHORT);
    bindNavDown(RIGHT, SHORT);
    bindStepsUp(UP, SHORT);
    bindSetGoal(DOWN, DOUBLE);
    bindReset(DOWN, LONG);
    bindStepsDown(DOWN, SHORT);
}





//********************************************************
// Run at a fixed rate, modifies the device's state depending on button presses
//********************************************************
void btnUpdateState()
{
    updateSwitch();

    for (uint8_t i=0; i< NUM_BUTS; i++) {
        but_press[i] = checkPressType(i);
    }

    // Enable/Disable test mode
    if (isSwitchUp()) {
        setDebugMode(true);
    } else {
        setDebugMode(false);
    }

    // Usage of UP and DOWN buttons
    if (getDebugMode()) {
        debugMode();
    } else {
        // NORMAL OPERATION
        switch(getDisplayMode()) {
            case DISPLAY_STEPS:
                displaySteps();
                break;
            case DISPLAY_DISTANCE:
                displayDistance();
                break;
            case DISPLAY_SET_GOAL:
                displaySetGoal();
                break;
            default:
                break;
        }
    }

}


