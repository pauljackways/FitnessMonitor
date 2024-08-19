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

#include "tiva_mocks/gpio_mock.h"
#include "tiva_mocks/sysctl_mock.h"
#include "tiva_mocks/buttons4_mock.h"
#include "deviceState_mock.h"
#include "display_manager_mock.h"
#include "switches_mock.h"
#include "step_counter_main_mock.h"


/* Helper functions */      
void reset_fff(void)
{
    FFF_BUTTONS_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
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
}

void tearDown(void)
{
    
}

void test_button_second_state_on_right(void)
{
    //Arrange
    // First state is DISPLAY_STEPS according to displayMode_t enum
    getDisplayMode_fake.return_val = DISPLAY_STEPS;
    checkButton_fake.arg0_val = RIGHT;
    checkButton_fake.return_val = PUSHED;

    // Act
    btnUpdateState();

    // Assert
    TEST_ASSERT_EQUAL(DISPLAY_DISTANCE, setDisplayMode_fake.arg0_val);
}

