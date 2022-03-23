/*
 * Step_Counter_Main.c
 *
 *  Created on: 23/03/2022
 *      Author: mattr
 */

// Comment this out to disable serial plotting
#define SERIAL_PLOTTING_ENABLED


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
// #include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "buttons4.h"
#include "acc.h"
#include "math.h"

#include "circBufT.h"
#include "serial_sender.h"
#include "accl_manager.h"

/**********************************************************
 * Constants and types
 **********************************************************/
#define RATE_SYSTICK_HZ 1000
#define RATE_BUTTONS_HZ 50
#define RATE_ACCL_HZ 200
#define RATE_DISPLAY_UPDATE_HZ 5
#ifdef SERIAL_PLOTTING_ENABLED
#define RATE_SERIAL_PLOT_HZ 100
#endif // SERIAL_PLOTTING_ENABLED
//#define SLOWTICK_RATE_HZ 100
//#define ACC_DATA_RATE 200
#define MAX_STR_LEN 16

#define STEP_THRESHOLD_HIGH 270
#define STEP_THRESHOLD_LOW 235

/*******************************************
 *      Local prototypes
 *******************************************/
void SysTickIntHandler (void);
void initClock (void);
void initSysTick (void);
void initDisplay (void);
void initAccl (void);
void displayUpdate (char *str1, char *str2, int16_t num, uint8_t charLine);
vector3_t getAcclData (void);


/*******************************************
 *      Globals
 *******************************************/
unsigned long ticksElapsed = 0;



/***********************************************************
 * Initialisation functions
 ***********************************************************/
void SysTickIntHandler (void)
{
    ticksElapsed++;
}

/*void
SysTickIntHandler (void)
{
    static uint8_t tickCount = 0;
    static uint8_t acc_tickCount = 0;
//    const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;
    const uint8_t ACCticksPerSlow = ACC_DATA_RATE / SLOWTICK_RATE_HZ;
    const uint8_t ticksPerACC = SYSTICK_RATE_HZ / ACC_DATA_RATE;

    updateButtons ();       // Poll the buttons
    if (acc_tickCount >= ACCticksPerSlow)
    {                       // Signal a slow tick
        acc_tickCount = 0;
        slowTick = true;
    }
    if (++tickCount >= ticksPerACC)
    {                       // Signal a slow tick
        tickCount = 0;
        accTick = true;
        acc_tickCount++;
    }
//    if (++tickCount >= ticksPerSlow)
//    {                       // Signal a slow tick
//        tickCount = 0;
//        slowTick = true;
////        acc_tickCount++;
//    }

}*/



void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}



void
initSysTick (void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet (SysCtlClockGet () / RATE_SYSTICK_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister (SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable ();
    SysTickEnable ();
}



/***********************************************************
 * Helper functions
 ***********************************************************/
// Read the current systick value, without mangling the data
unsigned long readCurrentTick(void)
{
    unsigned long currentTick;
    SysTickIntDisable();
    currentTick = ticksElapsed;
    SysTickIntEnable();
    return currentTick;
}














































void
initDisplay (void)
{
  // intialise the Orbit OLED display
    OLEDInitialise ();
}


//*****************************************************************************
// Function to display a changing message on the display.
// The display has 4 rows of 16 characters, with 0, 0 at top left.
//*****************************************************************************
void
displayUpdate (char *str1, char *str2, int16_t num, uint8_t charLine)
{
    char text_buffer[17];           //Display fits 16 characters wide.

    // "Undraw" the previous contents of the line to be updated.
    OLEDStringDraw ("                ", 0, charLine);
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %s %3d", str1, str2, num);
    // Update line on display.
    OLEDStringDraw (text_buffer, 0, charLine);
}





int
main(void)
{
    unsigned long last_button_process= 0;
    unsigned long last_accl_process = 0;
    unsigned long last_display_process = 0;

    #ifdef SERIAL_PLOTTING_ENABLED
    unsigned long last_serial_process = 0;
    #endif // SERIAL_PLOTTING_ENABLED

    uint8_t stepping = false;
    uint32_t steps = 0;
    vector3_t mean;

    initClock ();
    initDisplay ();
    initButtons ();
    SerialInit ();
    initSysTick ();
    acclInit ();

    OLEDStringDraw ("UART Acc Test", 0, 0);
//    SerialPlot(100, 200, 300, 400);

    while(1)
    {
        unsigned long currentTick = readCurrentTick();

        if (last_button_process + RATE_SYSTICK_HZ/RATE_BUTTONS_HZ < currentTick) {
            // poll the buttons
            last_button_process = currentTick;
        }

        if (last_accl_process + RATE_SYSTICK_HZ/RATE_ACCL_HZ < currentTick) {
            // process the acceleration system
            last_accl_process = currentTick;

            acclProcess();

            mean = acclMean();

            uint16_t combined = sqrt(mean.x*mean.x + mean.y*mean.y + mean.z*mean.z);

            if (combined >= STEP_THRESHOLD_HIGH && stepping == false) {
                stepping = true;
                steps++;
            } else if (combined <= STEP_THRESHOLD_LOW) {
                stepping = false;
            }
        }

        if (last_display_process + RATE_SYSTICK_HZ/RATE_DISPLAY_UPDATE_HZ < currentTick) {
            // Send message to booster display
            last_display_process = currentTick;

            char stepsStr[MAX_STR_LEN + 1];
//            usprintf(stepsStr, "# of Steps: %d", steps);
            usprintf(stepsStr, "Accl.x: %d", mean.x);
            OLEDStringDraw (stepsStr, 0, 1);
            usprintf(stepsStr, "Accl.y: %d", mean.y);
            OLEDStringDraw (stepsStr, 0, 2);
            usprintf(stepsStr, "Accl.z: %d", mean.z);
            OLEDStringDraw (stepsStr, 0, 3);


            // OLEDStringDraw (statusStr, 0, 1);
            // Just adding this so something changes
        }

        #ifdef SERIAL_PLOTTING_ENABLED
        if (last_serial_process + RATE_SYSTICK_HZ/RATE_SERIAL_PLOT_HZ < currentTick) {
            // plot the current data over serial
            last_serial_process = currentTick;

            SerialPlot(steps, mean.x, mean.y, mean.z);
        }
        #endif // SERIAL_PLOTTING_ENABLED



/*




//      SysCtlDelay (SysCtlClockGet () / 6);    // Approx 2 Hz

        if(accTick) {
            accTick = 0;



//        usprintf(statusStr, "%d", mean.x);
//        OLEDStringDraw (statusStr, 0, 1);
        }
        // Is it time to send a message?
        if (slowTick)
        {
            slowTick = false;

        }*/


    }

}





