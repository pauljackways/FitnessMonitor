#include "unity.h"
#include "buttons4.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)


#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#include "tiva_mocks/gpio_mock.h"
#include "tiva_mocks/sysctl_mock.h"


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
}

void tearDown(void)
{
    
}

void test_buttons4_initialises_up(void)
{
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;

    for (volatile int32_t i = 0; i < 10000000; i++) {
        updateButtons();
    }

    // Act
    uint8_t result = checkButton(UP);

    // Assert
    TEST_ASSERT_EQUAL(PUSHED, result);
}