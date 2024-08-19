#include <stdint.h>
#include <stdbool.h>

#include "unity.h"
#include "buttons4.h"
#include "circBufV.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "deviceState.h"
#include "button_manager.h"
#include "step_counter_main.h"
#include "switches.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#define LONG_PRESS_CYCLES 20

#include "tiva_mocks/gpio_mock.h"
#include "tiva_mocks/sysctl_mock.h"
//#include "tiva_mocks/buttons4_mock.h"
#include "deviceState_mock.h"
#include "display_manager_mock.h"
#include "switches_mock.h"
#include "step_counter_main_mock.h"


/* Helper functions */      
void reset_fff(void)
{
    FFF_BUTTONS_FAKES_LIST(RESET_FAKE);
    //FFF_BUTTONS4_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_DISPLAY_MANAGER_FAKES_LIST(RESET_FAKE);
    FFF_STEP_COUNTER_MAIN_FAKES_LIST(RESET_FAKE);
    FFF_SWITCHES_FAKES_LIST(RESET_FAKE);
    FFF_DEVICE_STATE_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

//For keeping track of display mode
static displayMode_t current_display_mode = 0;

displayMode_t getDisplayMode_callback(void) {
    return current_display_mode;
}

void setDisplayMode_callback(displayMode_t mode) {
    current_display_mode = mode;
}

//For keeping track of units
static displayMode_t current_display_units = 0;

displayUnits_t getDisplayUnits_callback(void) {
    return current_display_units;
}

void setDisplayUnits_callback(displayUnits_t mode) {
    current_display_units = mode;
}

//Presses or releases button
void press_button(uint8_t butName, uint8_t butState) {
    switch(butName) {
        case UP:
            GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
            if (butState == PUSHED) {
                GPIOPinRead_fake.return_val = UP_BUT_PIN;
            } else {
                GPIOPinRead_fake.return_val = 0;
            }
            break;
        case DOWN:
            GPIOPinRead_fake.arg0_val = DOWN_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = DOWN_BUT_PIN;
            GPIOPinRead_fake.return_val = DOWN_BUT_PIN;
            if (butState == PUSHED) {
                GPIOPinRead_fake.return_val = DOWN_BUT_PIN;
            } else {
                GPIOPinRead_fake.return_val = 0;
            }
            break;
        case LEFT:
            GPIOPinRead_fake.arg0_val = LEFT_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = LEFT_BUT_PIN;
            GPIOPinRead_fake.return_val = LEFT_BUT_PIN;
            if (butState == PUSHED) {
                GPIOPinRead_fake.return_val = LEFT_BUT_PIN;
            } else {
                GPIOPinRead_fake.return_val = 0;
            }
            break;
        case RIGHT:
            GPIOPinRead_fake.arg0_val = RIGHT_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = RIGHT_BUT_PIN;
            GPIOPinRead_fake.return_val = RIGHT_BUT_PIN;
            if (butState == PUSHED) {
                GPIOPinRead_fake.return_val = RIGHT_BUT_PIN;
            } else {
                GPIOPinRead_fake.return_val = 0;
            }
            break;
        default:
            return;
    }
    //To get past debouncing
    for (volatile int32_t i = 0; i < 10; i++) {
        btnUpdateState();
    }
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
    getDisplayMode_fake.return_val = 0;
    getStepsTaken_fake.return_val = 0;
    getStepHigh_fake.return_val = false;
    getCurrentGoal_fake.return_val = 0;
    getNewGoal_fake.return_val = 0;
    getDebugMode_fake.return_val = false;
    getMean_fake.return_val = (vector3_t){0.0, 0.0, 0.0};
    getDisplayUnits_fake.return_val = 0;
    getWorkoutStartTick_fake.return_val = 0UL;
    getFlashTicksLeft_fake.return_val = 0UL;
    getFlashMessage_fake.return_val = NULL;
    getDisplayMode_fake.custom_fake = getDisplayMode_callback;
    setDisplayMode_fake.custom_fake = setDisplayMode_callback;
    getDisplayUnits_fake.custom_fake = getDisplayUnits_callback;
    setDisplayUnits_fake.custom_fake = setDisplayUnits_callback;
}

void tearDown(void)
{
    
}

void test_button_second_state_on_left(void)
{
    //Arrange
    getDisplayMode_fake.return_val = 0;
    press_button(LEFT, PUSHED);

    // Act
    btnUpdateState();

    // Assert
    TEST_ASSERT_EQUAL(1, setDisplayMode_fake.arg0_val);
}

void test_button_final_state_on_right(void)
{
    //Arrange
    getDisplayMode_fake.return_val = 0;

    // Act
    press_button(RIGHT, PUSHED);

    // Assert
    TEST_ASSERT_EQUAL(DISPLAY_NUM_STATES-1, setDisplayMode_fake.arg0_val);
}

void test_button_num_states_back_to_start(void)
{
    //Arrange
    getDisplayMode_fake.return_val = 0;

    // Act
    for (uint8_t i = 0; i < DISPLAY_NUM_STATES; i++) {
        press_button(RIGHT, PUSHED);
        press_button(RIGHT, RELEASED);
    }

    // Assert
    TEST_ASSERT_EQUAL(0, setDisplayMode_fake.arg0_val);
}

void test_button_long_down_resets(void)
{
    //Arrange
    getStepsTaken_fake.return_val = 100;
    getDebugMode_fake.return_val = false;
    getDisplayMode_fake.return_val = 0;

    // Act
    for (uint8_t i = 0; i < LONG_PRESS_CYCLES + 1; i++) {
        press_button(DOWN, PUSHED);
    }

    // Assert
    TEST_ASSERT_EQUAL(0, setStepsTaken_fake.arg0_val);
}

void test_button_debug_up_increments_all_states(void)
{
    //Arrange
    getDebugMode_fake.return_val = true;

    //Act
    //Assert
    for (uint8_t i = 0; i < DISPLAY_NUM_STATES; i++) {
        getStepsTaken_fake.return_val = 100;
        getDisplayMode_fake.return_val = i;
        press_button(UP, PUSHED);
        press_button(UP, RELEASED);
        TEST_ASSERT_EQUAL(100 + DEBUG_STEP_INCREMENT, setStepsTaken_fake.arg0_val);
    }

}

void test_button_debug_down_decrements_all_states(void)
{
    //Arrange
    getDebugMode_fake.return_val = true;

    //Act
    //Assert
    for (uint8_t i = 0; i < DISPLAY_NUM_STATES; i++) {
        getStepsTaken_fake.return_val = 1000;
        getDisplayMode_fake.return_val = i;
        press_button(DOWN, PUSHED);
        press_button(DOWN, RELEASED);
        TEST_ASSERT_EQUAL(1000 - DEBUG_STEP_DECREMENT, setStepsTaken_fake.arg0_val);
    }

}

void test_button_debug_down_decrement_stops_at_zero(void)
{
    //Arrange
    getDebugMode_fake.return_val = true;

    //Act
    //Assert
    for (uint8_t i = 0; i < DISPLAY_NUM_STATES; i++) {
        getStepsTaken_fake.return_val = DEBUG_STEP_DECREMENT - 1;
        getDisplayMode_fake.return_val = i;
        press_button(DOWN, PUSHED);
        press_button(DOWN, RELEASED);
        TEST_ASSERT_EQUAL(0, setStepsTaken_fake.arg0_val);
    }

}

void test_button_up_changes_units_all_states(void)
{
    //Arrange
    getDebugMode_fake.return_val = false;
    //Act
    //Assert
    for (uint8_t i = 0; i < DISPLAY_NUM_STATES; i++) {
        getDisplayUnits_fake.return_val = UNITS_NUM_TYPES;
        for (uint8_t j = UNITS_NUM_TYPES-1; j <0 ; j--) {
            getDisplayMode_fake.return_val = i;
            press_button(UP, PUSHED);
            press_button(UP, RELEASED);
            TEST_ASSERT_EQUAL(j, setDisplayUnits_fake.arg0_val);
        }
    }

}
