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

#include "deviceState.h"
#include "../target/FreeRTOSConfig.h"

/**********************************************************
 * Constants and types
 **********************************************************/

#define M_PER_STEP 9/10
#define MAX_STR_LEN 16

#define RATE_IO_HZ 15
#define RATE_ACCL_HZ 20
#define RATE_ADC_HZ 20
#define RATE_DISPLAY_HZ 200
#define FLASH_MESSAGE_TIME 2 // seconds

#ifdef SERIAL_PLOTTING_ENABLED
#define RATE_SERIAL_PLOT_HZ 100
#endif // SERIAL_PLOTTING_ENABLED

#define TARGET_DISTANCE_DEFAULT 1000

#define DEBUG_STEP_INCREMENT 100
#define DEBUG_STEP_DECREMENT 500


void flashMessage(char*);

#endif /* STEP_COUNTER_MAIN_H_ */
