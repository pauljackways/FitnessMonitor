#include "deviceState.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"



typedef struct {
    displayMode_t displayMode;
    SemaphoreHandle_t displayModeMutex;
    uint32_t stepsTaken;
    SemaphoreHandle_t stepsTakenMutex;
    bool stepHigh;
    SemaphoreHandle_t stepHighMutex;
    uint32_t currentGoal;       // Goal the user is aiming for
    SemaphoreHandle_t currentGoalMutex;
    uint32_t newGoal;           // Value from the potentiometer, processed to give a new goal
    SemaphoreHandle_t newGoalMutex;
    bool debugMode;             // Is enable/disable debug functionality
    SemaphoreHandle_t debugModeMutex;
    vector3_t mean;
    SemaphoreHandle_t meanMutex;
    displayUnits_t displayUnits;
    SemaphoreHandle_t displayUnitsMutex;
    unsigned long workoutStartTick;
    SemaphoreHandle_t workoutStartTickMutex;
    unsigned long flashTicksLeft; // For displaying temporary messages over the top of everything else
    SemaphoreHandle_t flashTicksLeftMutex;
    char *flashMessage;
    SemaphoreHandle_t flashMessageMutex;
} deviceStateInfo_t;

static deviceStateInfo_t* deviceState = NULL;

void initFlashMessage(int32_t strLen) {
    // Allocate memory for flashMessage
    deviceState->flashMessage = (char*)calloc(strLen + 1, sizeof(char));
    if (deviceState->flashMessage == NULL) {
        printf("Failed to allocate memory for flashMessage\n");
        free(deviceState); 
        deviceState = NULL;
        return;
    }
}

void initDeviceState(void) {
    // Allocate memory for the deviceState struct
    deviceState = (deviceStateInfo_t*)calloc(1, sizeof(deviceStateInfo_t)); 
    if (deviceState == NULL) {
        printf("Failed to allocate memory for deviceState\n");
        return;
    }

    // Create mutexes
    deviceState->displayModeMutex = xSemaphoreCreateMutex();
    deviceState->stepsTakenMutex = xSemaphoreCreateMutex();
    deviceState->stepHighMutex = xSemaphoreCreateMutex();
    deviceState->currentGoalMutex = xSemaphoreCreateMutex();
    deviceState->newGoalMutex = xSemaphoreCreateMutex();
    deviceState->debugModeMutex = xSemaphoreCreateMutex();
    deviceState->meanMutex = xSemaphoreCreateMutex();
    deviceState->displayUnitsMutex = xSemaphoreCreateMutex();
    deviceState->workoutStartTickMutex = xSemaphoreCreateMutex();
    deviceState->flashTicksLeftMutex = xSemaphoreCreateMutex();
    deviceState->flashMessageMutex = xSemaphoreCreateMutex();

    if (deviceState->displayModeMutex == NULL ||
        deviceState->stepsTakenMutex == NULL || 
        deviceState->stepHighMutex == NULL || 
        deviceState->currentGoalMutex == NULL || 
        deviceState->newGoalMutex == NULL || 
        deviceState->debugModeMutex == NULL ||
        deviceState->meanMutex == NULL ||
        deviceState->displayUnitsMutex == NULL ||
        deviceState->workoutStartTickMutex == NULL ||
        deviceState->flashTicksLeftMutex == NULL ||
        deviceState->flashMessageMutex == NULL) {
        
        printf("Failed to create one or more mutexes\n");

        // Free allocated memory and destroy mutexes
        if (deviceState->flashMessage) {
            free(deviceState->flashMessage);
        }
        free(deviceState); // Free previously allocated memory
        deviceState = NULL;
        return;
    }
}

// Getter and Setter for displayMode
displayMode_t getDisplayMode() {
    xSemaphoreTake(deviceState->displayModeMutex, portMAX_DELAY);
    displayMode_t value = deviceState->displayMode;
    xSemaphoreGive(deviceState->displayModeMutex);
    return value;
}

void setDisplayMode(displayMode_t value) {
    xSemaphoreTake(deviceState->displayModeMutex, portMAX_DELAY);
    deviceState->displayMode = value;
    xSemaphoreGive(deviceState->displayModeMutex);
}

// Getter and Setter for stepsTaken
uint32_t getStepsTaken() {
    xSemaphoreTake(deviceState->stepsTakenMutex, portMAX_DELAY);
    uint32_t value = deviceState->stepsTaken;
    xSemaphoreGive(deviceState->stepsTakenMutex);
    return value;
}

void setStepsTaken(uint32_t value) {
    xSemaphoreTake(deviceState->stepsTakenMutex, portMAX_DELAY);
    deviceState->stepsTaken = value;
    xSemaphoreGive(deviceState->stepsTakenMutex);
}

// Getter and Setter for stepHigh
bool getStepHigh() {
    xSemaphoreTake(deviceState->stepHighMutex, portMAX_DELAY);
    bool value = deviceState->stepHigh;
    xSemaphoreGive(deviceState->stepHighMutex);
    return value;
}

void setStepHigh(bool value) {
    xSemaphoreTake(deviceState->stepHighMutex, portMAX_DELAY);
    deviceState->stepHigh = value;
    xSemaphoreGive(deviceState->stepHighMutex);
}

// Getter and Setter for currentGoal
uint32_t getCurrentGoal() {
    xSemaphoreTake(deviceState->currentGoalMutex, portMAX_DELAY);
    uint32_t value = deviceState->currentGoal;
    xSemaphoreGive(deviceState->currentGoalMutex);
    return value;
}

void setCurrentGoal(uint32_t value) {
    xSemaphoreTake(deviceState->currentGoalMutex, portMAX_DELAY);
    deviceState->currentGoal = value;
    xSemaphoreGive(deviceState->currentGoalMutex);
}

// Getter and Setter for newGoal
uint32_t getNewGoal() {
    xSemaphoreTake(deviceState->newGoalMutex, portMAX_DELAY);
    uint32_t value = deviceState->newGoal;
    xSemaphoreGive(deviceState->newGoalMutex);
    return value;
}

void setNewGoal(uint32_t value) {
    xSemaphoreTake(deviceState->newGoalMutex, portMAX_DELAY);
    deviceState->newGoal = value;
    xSemaphoreGive(deviceState->newGoalMutex);
}

// Getter and Setter for debugMode
bool getDebugMode() {
    xSemaphoreTake(deviceState->debugModeMutex, portMAX_DELAY);
    bool value = deviceState->debugMode;
    xSemaphoreGive(deviceState->debugModeMutex);
    return value;
}

void setDebugMode(bool value) {
    xSemaphoreTake(deviceState->debugModeMutex, portMAX_DELAY);
    deviceState->debugMode = value;
    xSemaphoreGive(deviceState->debugModeMutex);
}

// Getter and Setters for mean vector3_t
vector3_t getMean() {
    xSemaphoreTake(deviceState->meanMutex, portMAX_DELAY);
    vector3_t value = deviceState->mean;
    xSemaphoreGive(deviceState->meanMutex);
    return value;
}

void setMean(vector3_t value) {
    xSemaphoreTake(deviceState->meanMutex, portMAX_DELAY);
    deviceState->mean = value;
    xSemaphoreGive(deviceState->meanMutex);
}

// Getter and Setter for displayUnits
displayUnits_t getDisplayUnits() {
    xSemaphoreTake(deviceState->displayUnitsMutex, portMAX_DELAY);
    displayUnits_t value = deviceState->displayUnits;
    xSemaphoreGive(deviceState->displayUnitsMutex);
    return value;
}

void setDisplayUnits(displayUnits_t value) {
    xSemaphoreTake(deviceState->displayUnitsMutex, portMAX_DELAY);
    deviceState->displayUnits = value;
    initButtons();
    xSemaphoreGive(deviceState->displayUnitsMutex);
}

// Getter and Setter for workoutStartTick
unsigned long getWorkoutStartTick() {
    xSemaphoreTake(deviceState->workoutStartTickMutex, portMAX_DELAY);
    unsigned long value = deviceState->workoutStartTick;
    xSemaphoreGive(deviceState->workoutStartTickMutex);
    return value;
}

void setWorkoutStartTick(unsigned long value) {
    xSemaphoreTake(deviceState->workoutStartTickMutex, portMAX_DELAY);
    deviceState->workoutStartTick = value;
    xSemaphoreGive(deviceState->workoutStartTickMutex);
}

// Getter and Setter for flashTicksLeft
unsigned long getFlashTicksLeft() {
    xSemaphoreTake(deviceState->flashTicksLeftMutex, portMAX_DELAY);
    unsigned long value = deviceState->flashTicksLeft;
    xSemaphoreGive(deviceState->flashTicksLeftMutex);
    return value;
}

void setFlashTicksLeft(unsigned long value) {
    xSemaphoreTake(deviceState->flashTicksLeftMutex, portMAX_DELAY);
    deviceState->flashTicksLeft = value;
    xSemaphoreGive(deviceState->flashTicksLeftMutex);
}

// Getter and Setter for flashMessage
char* getFlashMessage() {
    xSemaphoreTake(deviceState->flashMessageMutex, portMAX_DELAY);
    char* value = deviceState->flashMessage;
    xSemaphoreGive(deviceState->flashMessageMutex);
    return value;
}

void setFlashMessage(char* value) {
    xSemaphoreTake(deviceState->flashMessageMutex, portMAX_DELAY);
    if (deviceState->flashMessage) {
        free(deviceState->flashMessage);
    }
    deviceState->flashMessage = strdup(value);
    xSemaphoreGive(deviceState->flashMessageMutex);
}