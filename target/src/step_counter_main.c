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
#include "step_counter_main.h"
#include "ADC_read.h"
#include "../libs/freertos/include/FreeRTOS.h"
#include "../libs/freertos/include/task.h"

#ifdef SERIAL_PLOTTING_ENABLED
#include "serial_sender.h"
#endif //SERIAL_PLOTTING_ENABLED

#include "accl_manager.h"
#include "display_manager.h"
#include "button_manager.h"

#include "step_counter_main.h"

/**********************************************************
 * Constants and types
 **********************************************************/
#define RATE_SYSTICK_HZ 1000
#define RATE_IO_HZ 75
#define RATE_ACCL_HZ 200
#define RATE_DISPLAY_UPDATE_HZ 5
#define FLASH_MESSAGE_TIME 3/2 // seconds

#ifdef SERIAL_PLOTTING_ENABLED
#define RATE_SERIAL_PLOT_HZ 100
#endif // SERIAL_PLOTTING_ENABLED


#define STEP_GOAL_ROUNDING 100
#define STEP_THRESHOLD_HIGH 270
#define STEP_THRESHOLD_LOW 235

#define TARGET_DISTANCE_DEFAULT 1000

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
void flashMessage(deviceStateInfo_t* deviceState, char* toShow)
{
    deviceState->flashTicksLeft = RATE_DISPLAY_UPDATE_HZ * FLASH_MESSAGE_TIME;

    uint8_t i = 0;
    while (toShow[i] != '\0' && i < MAX_STR_LEN) {
        (deviceState->flashMessage)[i] = toShow[i];

        i++;
    }

    deviceState->flashMessage[i] = '\0';
}

void vTaskUpdateButtons(void *pvParameters)
{
    const TickType_t xDelay = pdTICKS_TO_MS(75); 
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;

    for (;;)
    {
        btnUpdateState(deviceState);
        vTaskDelay(xDelay); // Delay to achieve 1000 Hz frequency
    }
}

void vTaskPollADC(void *pvParameters)
{
    const TickType_t xDelay = pdTICKS_TO_MS(75); // 75 ticks delay for 13.33 Hz

    for (;;)
    {
        pollADC();
        vTaskDelay(xDelay); // Delay to achieve ~13.33 Hz frequency
    }
}

void vTaskGetNewGoal(void *pvParameters)
{
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;
    const TickType_t xDelay = pdTICKS_TO_MS(75); // 75 ticks delay for 13.33 Hz

    for (;;)
    {
        get_new_goal(deviceState);
        vTaskDelay(xDelay); // Delay to achieve ~13.33 Hz frequency
    }
}

void get_new_goal(deviceStateInfo_t* deviceState) 
{
    deviceState->newGoal = ((readADC() * POT_SCALE_COEFF) / STEP_GOAL_ROUNDING) * STEP_GOAL_ROUNDING; // Round to the nearest 100 steps
    if (deviceState->newGoal == 0) { // Prevent a goal of zero, instead setting to the minimum goal (this also makes it easier to test the goal-reaching code on a small but non-zero target)
        deviceState->newGoal = STEP_GOAL_ROUNDING;
    }
}

void vTaskAcclProcess(void *pvParameters)
{
    const TickType_t xDelay = pdTICKS_TO_MS(200);

    for (;;)
    {
        acclProcess();
        vTaskDelay(xDelay);
    }
}

void vTaskGetAcclMean(void *pvParameters)
{
    const TickType_t xDelay = pdTICKS_TO_MS(200); 
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;

    for (;;)
    {
        deviceState->mean = acclMean();
        vTaskDelay(xDelay); 
    }
}

void vTaskStepTest(void *pvParameters)
{
    const TickType_t xDelay = pdTICKS_TO_MS(200); 
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;

    for (;;)
    {
        uint16_t combined = sqrt(deviceState->mean.x * deviceState->mean.x + deviceState->mean.y * deviceState->mean.y + deviceState->mean.z * deviceState->mean.z);        
        if (combined >= STEP_THRESHOLD_HIGH && deviceState->stepHigh == false) {
            deviceState->stepHigh = true;
            deviceState->stepsTaken++;

            // flash a message if the user has reached their goal
            if (deviceState->stepsTaken == deviceState->currentGoal && deviceState->flashTicksLeft == 0) {
                flashMessage(deviceState, "Goal reached!");
            }

        } else if (combined <= STEP_THRESHOLD_LOW) {
            deviceState->stepHigh = false;
        }
        if (deviceState->stepsTaken == 0) {
            deviceState->workoutStartTick = readCurrentTick();
        }
        vTaskDelay(xDelay); 
    }
}

void vTaskDisplayUpdate(void *pvParameters)
{
    const TickType_t xDelay = pdTICKS_TO_MS(5); 
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;

    for (;;)
    {
        if (deviceState->flashTicksLeft > 0) {
            deviceState->flashTicksLeft--;
        }

        uint16_t secondsElapsed = (readCurrentTick() - deviceState->workoutStartTick)/RATE_SYSTICK_HZ;
        displayUpdate(deviceState, secondsElapsed);
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


    #ifdef SERIAL_PLOTTING_ENABLED
    //unsigned long lastSerialProcess = 0;
    #endif // SERIAL_PLOTTING_ENABLED

    deviceStateInfo_t* deviceState = malloc(sizeof(deviceStateInfo_t)); // Stored as one global so it can be accessed by other helper libs within this main module

    // Device state
    // Omnibus struct that holds loads of info about the device's current state, so it can be updated from any function
    deviceState->displayMode = DISPLAY_STEPS;
    deviceState->stepsTaken = 0;
    deviceState->stepHigh = false;
    deviceState->currentGoal = TARGET_DISTANCE_DEFAULT;
    deviceState->debugMode = false;
    deviceState->mean = (vector3_t){0,0,0};
    deviceState->displayUnits= UNITS_SI;
    deviceState->workoutStartTick = 0;
    deviceState->flashTicksLeft = 0;
    deviceState->flashMessage = "Test";
    // deviceState.flashMessage = calloc(MAX_STR_LEN + 1, sizeof(char));

    // if (deviceState.flashMessage == NULL) {
    //     return;
    // }

    #ifdef SERIAL_PLOTTING_ENABLED
    // if (lastSerialProcess + RATE_SYSTICK_HZ/RATE_SERIAL_PLOT_HZ < currentTick) {
    //     lastSerialProcess = currentTick;

    //     SerialPlot(deviceState.stepsTaken, mean.x, mean.y, mean.z);
    // }
    #endif // SERIAL_PLOTTING_ENABLED

    #ifdef SERIAL_PLOTTING_ENABLED
    SerialInit ();
    #endif // SERIAL_PLOTTING_ENABLED

    xTaskCreate(vTaskDisplayUpdate, "display_update", 1024, deviceState, 2, NULL);
    xTaskCreate(vTaskUpdateButtons, "UpdateButtons", 1024, deviceState, 1, NULL);
    xTaskCreate(vTaskPollADC, "PollADC", 1024, NULL, 1, NULL);
    xTaskCreate(vTaskGetNewGoal, "get_new_goal", 1024, NULL, 1, NULL);
    xTaskCreate(vTaskAcclProcess, "AcclProcess", 1024, NULL, 1, NULL);
    xTaskCreate(vTaskGetAcclMean, "get_accl_mean", 1024, deviceState, 1, NULL);
    xTaskCreate(vTaskStepTest, "step_test", 1024, deviceState, 1, NULL);
    vTaskStartScheduler();

    return 0; // Should never reach here


}






