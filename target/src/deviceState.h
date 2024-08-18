#ifndef DEVICE_STATE_H_
#define DEVICE_STATE_H_

#include <stdbool.h>
#include <stdint.h>
#include "circBufV.h"
#include "../libs/freertos/include/FreeRTOS.h"
#include <semphr.h>

typedef enum {
    DISPLAY_STEPS = 0,
    DISPLAY_DISTANCE,
    DISPLAY_SET_GOAL,
    DISPLAY_NUM_STATES, // Automatically enumerates to the number of display states there can be
} displayMode_t;


typedef enum {
    UNITS_SI = 0,       // Steps  /km
    UNITS_ALTERNATE,    // Percent/miles
    UNITS_NUM_TYPES,
} displayUnits_t;

// Initialisation functions
void initDeviceState(void);
void initFlashMessage(int32_t strLen);

// Getter and Setter functions for device state
displayMode_t getDisplayMode(void);
void setDisplayMode(displayMode_t value);

uint32_t getStepsTaken(void);
void setStepsTaken(uint32_t value);

bool getStepHigh(void);
void setStepHigh(bool value);

uint32_t getCurrentGoal(void);
void setCurrentGoal(uint32_t value);

uint32_t getNewGoal(void);
void setNewGoal(uint32_t value);

bool getDebugMode(void);
void setDebugMode(bool value);

vector3_t getMean(void);
void setMean(vector3_t value);

displayUnits_t getDisplayUnits(void);
void setDisplayUnits(displayUnits_t value);

unsigned long getWorkoutStartTick(void);
void setWorkoutStartTick(unsigned long value);

unsigned long getFlashTicksLeft(void);
void setFlashTicksLeft(unsigned long value);

char* getFlashMessage(void);
void setFlashMessage(char* value);

#endif