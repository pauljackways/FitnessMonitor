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


//********************************************************
// Init buttons and switch I/O handlers
//********************************************************
void btnInit(void)
{
    initButtons();
    initSwitch();
}


//********************************************************
// Run at a fixed rate, modifies the device's state depending on button presses
//********************************************************
void btnUpdateState()
{
    displayMode_t currentDisplayMode = getDisplayMode();
    
    updateButtons();
    updateSwitch();


    // Changing screens
    if (checkButton(LEFT) == PUSHED) {
        setDisplayMode((currentDisplayMode + 1) % DISPLAY_NUM_STATES);      //flicker when pressing button

    } else if (checkButton(RIGHT) == PUSHED) {
        // Can't use mod, as enums behave like an unsigned int, so (0-1)%n != n-1
        if (currentDisplayMode > 0) {
            setDisplayMode(currentDisplayMode - 1);
        } else {
            setDisplayMode(DISPLAY_NUM_STATES - 1);
        }
    }

    // Enable/Disable test mode
    if (isSwitchUp()) {
        setDebugMode(true);
    } else {
        setDebugMode(false);
    }


    // Usage of UP and DOWN buttons
    if (getDebugMode()) {
        // TEST MODE OPERATION
        if (checkButton(UP) == PUSHED) {
            setStepsTaken(getStepsTaken() + DEBUG_STEP_INCREMENT);
        }

        if (checkButton(DOWN) == PUSHED) {
            if (getStepsTaken() >= DEBUG_STEP_DECREMENT) {
                setStepsTaken(getStepsTaken() - DEBUG_STEP_DECREMENT);
            } else {
                setStepsTaken(0);
            }
        }

    } else {
        // NORMAL OPERATION

        // Changing units
        if (checkButton(UP) == PUSHED) {
            // Can't use mod, as enums behave like an unsigned int, so (0-1)%n != n-1
            displayUnits_t currentDisplayUnits = getDisplayUnits();
            if (currentDisplayUnits > 0) {
                setDisplayUnits(currentDisplayUnits - 1);
            } else {
                setDisplayUnits(UNITS_NUM_TYPES - 1);
            }
        }


        // Resetting steps and updating goal with long and short presses
        if ((isDown(DOWN) == true) && (getDisplayMode() != DISPLAY_SET_GOAL) && (allowLongPress)) {
            longPressCount++;
            if (longPressCount >= LONG_PRESS_CYCLES) {
                setStepsTaken(0);
                flashMessage("Reset!");
            }
        } else {
            if ((currentDisplayMode == DISPLAY_SET_GOAL) && checkButton(DOWN) == PUSHED) {
                // TODO: Make direct call to setGoal from here?
                setCurrentGoal(getNewGoal());
                setDisplayMode(DISPLAY_STEPS);

                allowLongPress = false; // Hacky solution: Protection against double-registering as a short press then a long press
            }
            longPressCount = 0;
        }

        if (checkButton(DOWN) == RELEASED) {
            allowLongPress = true;
        }


    }

}


