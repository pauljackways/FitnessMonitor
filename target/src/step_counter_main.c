/*
 * Step_Counter_Main.c
 *
 *  Created on: 23/03/2022
 *      Authors: Matthew Suter, Daniel Rabbidge, Timothy Preston-Marshall
 *
 *  Main code for the ENCE361 step counter project
 *
 *  FitnessThur9-1
 */

// Comment this out to disable serial plotting
// #define SERIAL_PLOTTING_ENABLED 


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "circBufV.h"
#include "deviceState.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "utils/ustdlib.h"
#include "acc.h"
#include "math.h"
#include "driverlib/interrupt.h"
#include "step_counter_main.h"
#include "ADC_read.h"
#include "../libs/freertos/include/FreeRTOS.h"
#include "../libs/freertos/include/task.h"


#include "accl_manager.h"
#include "display_manager.h"
#include "button_manager.h"


#include "step_counter_main.h"

/**********************************************************
 * Constants and types
 **********************************************************/

#define STEP_GOAL_ROUNDING 100
#define STEP_THRESHOLD_HIGH 270
#define STEP_THRESHOLD_LOW 235

#define POT_SCALE_COEFF 20000/4095 // in steps, adjusting to account for the potentiometer's maximum possible reading



/*******************************************
 *      Local prototypes
 *******************************************/
void initClock (void);
void initDisplay (void);
void initAccl (void);
vector3_t getAcclData (void);

/***********************************************************
 * Initialisation functions
 ***********************************************************/

void vAssertCalled( const char * pcFile, unsigned long ulLine ) {
    (void)pcFile; // unused
    (void)ulLine; // unused
    while (true) {
        printf("%s", "fail");
    }
}

void initClock (void)
{
    // Set the clock rate to 80 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}



/***********************************************************
 * Helper functions
 ***********************************************************/
// Read the current systick value, without mangling the data
unsigned long readCurrentTick(void)
{
    return xTaskGetTickCount();
}



// Flash a message onto the screen, overriding everything else
void flashMessage(char* toShow)
{
    setFlashTicksLeft(RATE_DISPLAY_HZ * FLASH_MESSAGE_TIME);

    char tempMessage[MAX_STR_LEN + 1] = {0};

    uint8_t i = 0;
    while (toShow[i] != '\0' && i < MAX_STR_LEN) {
        tempMessage[i] = toShow[i];
        i++;
    }
    tempMessage[i] = '\0';

    setFlashMessage(tempMessage);

}

// Set newGoal value based on reading ADC buffer
void setGoal() {
    uint32_t newGoal = ((readADC() * POT_SCALE_COEFF) / STEP_GOAL_ROUNDING) * STEP_GOAL_ROUNDING;
    if (newGoal == 0) { // Prevent a goal of zero, instead setting to the minimum goal (this also makes it easier to test the goal-reaching code on a small but non-zero target)
        newGoal = STEP_GOAL_ROUNDING;
    }
    setNewGoal(newGoal); //Set the new goal value, scaling to give the desired range

}

// Check to see if a step has been taken
void stepCheck() { 
    bool stepHigh = getStepHigh();
    vector3_t mean = getMean();
    uint16_t combined = sqrt(mean.x*mean.x + mean.y*mean.y + mean.z*mean.z);

    if (combined >= STEP_THRESHOLD_HIGH && stepHigh == false) {
        setStepsTaken(getStepsTaken() + 1);
        setStepHigh(true);

        // flash a message if the user has reached their goal
        if (getStepsTaken() >= getCurrentGoal()) {
            //TODO: MESSAGE QUEUE HERE
            flashMessage("Goal reached!");
        }

    } else if (combined <= STEP_THRESHOLD_LOW) {
        setStepHigh(false);
    }
    //TODO: check how this works for efficacy
    if (getStepsTaken() == 0) {
        setWorkoutStartTick(readCurrentTick());
    }

}

// Button poll and handle
void vTaskButtons(void* pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(1000/RATE_IO_HZ); 

    for (;;) 
    {
        btnUpdateState();

        vTaskDelay(xDelay);
    }
}

// Set newGoal based on ADC
void vTaskGoal(void* pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(1000/RATE_ADC_HZ); 

    for (;;) 
    {
        pollADC();
        setGoal();

        vTaskDelay(xDelay);
    }
}

// Update accelerometer data and check for step
void vTaskPedometer(void* pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(1000/RATE_ACCL_HZ); 

    for (;;) 
    {
        acclProcess();
        setMean(acclMean());

        // Don't start the workout until the user begins walking
        stepCheck();

        vTaskDelay(xDelay);
    }
}

// Display 
void vTaskDisplay(void* pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(1000/RATE_DISPLAY_HZ); 

    for (;;)
    {
        displayUpdate();

        vTaskDelay(xDelay);
    }
}



/***********************************************************
 * Main Loop
 ***********************************************************/

int main(void)
{
    // Fitness Monitor 2.0 initialisation
    // Init libs
    initClock();
    displayInit();
    btnInit();
    acclInit();
    initADC();
    initDeviceState();
    initFlashMessage(MAX_STR_LEN);

    for (volatile int32_t i = 0; i < 10000000; i++);


    setDisplayMode(DISPLAY_STEPS);
    setCurrentGoal(TARGET_DISTANCE_DEFAULT);
    setDisplayUnits(UNITS_SI);

    IntMasterEnable();

    xTaskCreate(&vTaskButtons, "taskButtons", 256, NULL, 3, NULL);
    xTaskCreate(&vTaskGoal, "taskGoal", 256, NULL, 3, NULL);
    xTaskCreate(&vTaskPedometer, "taskPedometer", 256, NULL, 2, NULL);
    xTaskCreate(&vTaskDisplay, "taskDisplay", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0; // Should never reach here

}






