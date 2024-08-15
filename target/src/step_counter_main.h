/*
 * display_manager.h
 *
 *  Created on: 10/04/2022
 *      Author: Matthew Suter, Tim Preston-Marshall, Daniel Rabbidge
 *
 *  FitnessThur9-1
 */

#ifndef STEP_COUNTER_MAIN_H_
#define STEP_COUNTER_MAIN_H_

#include "circBufV.h"
#include "../libs/freertos/include/FreeRTOS.h"
#include <semphr.h>
#include "deviceState.h"

/**********************************************************
 * Constants and types
 **********************************************************/

#define M_PER_STEP 9/10
#define MAX_STR_LEN 16

#define RATE_IO_HZ 15
#define RATE_ACCL_HZ 20
#define RATE_DISPLAY_HZ 200
#define FLASH_MESSAGE_TIME 2 // seconds

#ifdef SERIAL_PLOTTING_ENABLED
#define RATE_SERIAL_PLOT_HZ 100
#endif // SERIAL_PLOTTING_ENABLED

#define TARGET_DISTANCE_DEFAULT 1000

#define DEBUG_STEP_INCREMENT 100
#define DEBUG_STEP_DECREMENT 500







typedef struct {
    displayMode_t displayMode;
    uint32_t stepsTaken;
    SemaphoreHandle_t stepsTakenMutex;
    bool stepHigh;
    uint32_t currentGoal;       // Goal the user is aiming for
    uint32_t newGoal;           // Value from the potentiometer, processed to give a new goal
    SemaphoreHandle_t newGoalMutex;
    bool debugMode;             // Is enable/disable debug functionality
    vector3_t mean;
    displayUnits_t displayUnits;
    unsigned long workoutStartTick;
    unsigned long flashTicksLeft; // For displaying temporary messages over the top of everything else
    char *flashMessage;
} deviceStateInfo_t;


void flashMessage(char*);

#endif /* STEP_COUNTER_MAIN_H_ */
