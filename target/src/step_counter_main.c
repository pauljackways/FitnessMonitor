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
#include <semphr.h>
#include "../libs/freertos/include/task.h"

#include "accl_manager.h"
#include "display_manager.h"
#include "button_manager.h"


#include "step_counter_main.h"

/**********************************************************
 * Constants and types
 **********************************************************/
#define RATE_IO_HZ 15
#define RATE_ACCL_HZ 20
#define RATE_DISPLAY_HZ 200
#define FLASH_MESSAGE_TIME 2 // seconds

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
    deviceState->flashTicksLeft = RATE_DISPLAY_HZ * FLASH_MESSAGE_TIME;

    uint8_t i = 0;
    while (toShow[i] != '\0' && i < MAX_STR_LEN) {
        (deviceState->flashMessage)[i] = toShow[i];

        i++;
    }

    deviceState->flashMessage[i] = '\0';
}

void getNewGoal(deviceStateInfo_t* deviceState) {
    xSemaphoreTake(deviceState->newGoalMutex, portMAX_DELAY);
    deviceState->newGoal = readADC() * POT_SCALE_COEFF; // Set the new goal value, scaling to give the desired range
    deviceState->newGoal = (deviceState->newGoal / STEP_GOAL_ROUNDING) * STEP_GOAL_ROUNDING; // Round to the nearest 100 steps
    if (deviceState->newGoal == 0) { // Prevent a goal of zero, instead setting to the minimum goal (this also makes it easier to test the goal-reaching code on a small but non-zero target)
        deviceState->newGoal = STEP_GOAL_ROUNDING;
    }
    xSemaphoreGive(deviceState->newGoalMutex);
}

void vTaskButtons(void* pvParameters) {
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;
    const TickType_t xDelay = pdMS_TO_TICKS(1000/RATE_IO_HZ); 

    for (;;) 
    {
        btnUpdateState(deviceState);

//         vTaskDelay(xDelay);
//     }
// }

// void vTaskGoal(void* pvParameters) {
//     deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;
//     const TickType_t xDelay = pdTICKS_TO_MS(75); 

//     for (;;) 
//     {
        pollADC();
        getNewGoal(deviceState);

        vTaskDelay(xDelay);
    }
}

void stepCheck(void* pvParameters) {
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;
    
    uint16_t combined = sqrt(deviceState->mean.x*deviceState->mean.x + deviceState->mean.y*deviceState->mean.y + deviceState->mean.z*deviceState->mean.z);

    xSemaphoreTake(deviceState->stepsTakenMutex, portMAX_DELAY);
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
    xSemaphoreGive(deviceState->stepsTakenMutex);

}

void vTaskPedometer(void* pvParameters) {
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;
    const TickType_t xDelay = pdMS_TO_TICKS(1000/RATE_ACCL_HZ); 

    for (;;) 
    {
        acclProcess();
        deviceState->mean = acclMean();

        // Don't start the workout until the user begins walking
        stepCheck(deviceState);

        vTaskDelay(xDelay);
    }
}

void vTaskDisplay(void* pvParameters)
{
    deviceStateInfo_t* deviceState = (deviceStateInfo_t *)pvParameters;
    const TickType_t xDelay = pdMS_TO_TICKS(1000/RATE_DISPLAY_HZ); 

    for (;;)
    {

        if (deviceState->flashTicksLeft > 0) {
            deviceState->flashTicksLeft--;
        }

        displayUpdate(deviceState);

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

    for (volatile int32_t i = 0; i < 10000000; i++);

    // Initialize the deviceState struct
    deviceStateInfo_t* deviceState = calloc(1, sizeof(deviceStateInfo_t)); 
    if (deviceState == NULL) {
        while (true) {
            printf("Failed to allocate memory for deviceState\n");
        }
    }
    deviceState->displayMode = DISPLAY_STEPS;
    deviceState->stepsTaken = 0;
    deviceState->stepHigh = false;
    deviceState->currentGoal = TARGET_DISTANCE_DEFAULT;
    deviceState->newGoal = 0;
    deviceState->debugMode = false;
    deviceState->mean = (vector3_t){0, 0, 0};
    deviceState->displayUnits = UNITS_SI;
    deviceState->workoutStartTick = 0;
    deviceState->flashTicksLeft = 0;
    deviceState->flashMessage = calloc(MAX_STR_LEN + 1, sizeof(char));
    if (deviceState->flashMessage == NULL) {
        while (true) {
            printf("Failed to allocate memory for flashMessage\n");
        }
    }
    deviceState->stepsTakenMutex = xSemaphoreCreateMutex();
    deviceState->newGoalMutex = xSemaphoreCreateMutex();
    if (deviceState->stepsTakenMutex == NULL || deviceState->newGoalMutex == NULL) {
        // Mutex creation failed
    }

    IntMasterEnable();

    xTaskCreate(&vTaskButtons, "taskButtons", 512, deviceState, 3, NULL);
    //xTaskCreate(&vTaskGoal, "taskGoal", 512, deviceState, 3, NULL);
    xTaskCreate(&vTaskPedometer, "taskPedometer", 512, deviceState, 2, NULL);
    xTaskCreate(&vTaskDisplay, "taskDisplay", 512, deviceState, 1, NULL);
    vTaskStartScheduler();

    return 0; // Should never reach here

}






