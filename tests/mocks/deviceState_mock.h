#include "fff.h"
#include <stdbool.h>
#include <stdint.h>
#include "deviceState.h"
#include "circBufV.h"


#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_DEVICE_STATE_FAKES_LIST(FUNC)   \
    FUNC(initDeviceState)                   \
    FUNC(initFlashMessage)                  \
    FUNC(setDisplayMode)                    \
    FUNC(setStepsTaken)                     \
    FUNC(setStepHigh)                       \
    FUNC(setCurrentGoal)                    \
    FUNC(setNewGoal)                        \
    FUNC(setDebugMode)                      \
    FUNC(setMean)                           \
    FUNC(setDisplayUnits)                   \
    FUNC(setWorkoutStartTick)               \
    FUNC(setFlashTicksLeft)                 \
    FUNC(setFlashMessage)                   \
    FUNC(getDisplayMode)                    \
    FUNC(getStepsTaken)                     \
    FUNC(getStepHigh)                       \
    FUNC(getCurrentGoal)                    \
    FUNC(getNewGoal)                        \
    FUNC(getDebugMode)                      \
    FUNC(getMean)                           \
    FUNC(getDisplayUnits)                   \
    FUNC(getWorkoutStartTick)               \
    FUNC(getFlashTicksLeft)                 \
    FUNC(getFlashMessage)

VOID_FUNC(initDeviceState);
VOID_FUNC(initFlashMessage, int32_t);
VOID_FUNC(setDisplayMode, displayMode_t);
VOID_FUNC(setStepsTaken, uint32_t);
VOID_FUNC(setStepHigh, bool);
VOID_FUNC(setCurrentGoal, uint32_t);
VOID_FUNC(setNewGoal, uint32_t);
VOID_FUNC(setDebugMode, bool);
VOID_FUNC(setMean, vector3_t);
VOID_FUNC(setDisplayUnits, displayUnits_t);
VOID_FUNC(setWorkoutStartTick, unsigned long);
VOID_FUNC(setFlashTicksLeft, unsigned long);
VOID_FUNC(setFlashMessage, char*);

VALUE_FUNC(displayMode_t, getDisplayMode);
VALUE_FUNC(uint32_t, getStepsTaken);
VALUE_FUNC(bool, getStepHigh);
VALUE_FUNC(uint32_t, getCurrentGoal);
VALUE_FUNC(uint32_t, getNewGoal);
VALUE_FUNC(bool, getDebugMode);
VALUE_FUNC(vector3_t, getMean);
VALUE_FUNC(displayUnits_t, getDisplayUnits);
VALUE_FUNC(unsigned long, getWorkoutStartTick);
VALUE_FUNC(unsigned long, getFlashTicksLeft);
VALUE_FUNC(char*, getFlashMessage);
