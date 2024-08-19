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

void press_button(uint8_t butName) {
    switch(butName) {
        case UP:
            GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
            GPIOPinRead_fake.return_val = UP_BUT_PIN;
            break;
        case DOWN:
            GPIOPinRead_fake.arg0_val = DOWN_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = DOWN_BUT_PIN;
            GPIOPinRead_fake.return_val = DOWN_BUT_PIN;
            break;
        case LEFT:
            GPIOPinRead_fake.arg0_val = LEFT_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = LEFT_BUT_PIN;
            GPIOPinRead_fake.return_val = LEFT_BUT_PIN;
            break;
        case RIGHT:
            GPIOPinRead_fake.arg0_val = RIGHT_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = RIGHT_BUT_PIN;
            GPIOPinRead_fake.return_val = RIGHT_BUT_PIN;
            break;
        default:
            GPIOPinRead_fake.arg0_val = DOWN_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = DOWN_BUT_PIN;
            GPIOPinRead_fake.return_val = 0;
    }
    //To get past debouncing
    for (volatile int32_t i = 0; i < 10; i++) {
        updateButtons();
    }
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
    press_button(UP);

    // Act
    uint8_t result = checkButton(UP);

    // Assert
    TEST_ASSERT_EQUAL(PUSHED, result);
}

void test_buttons4_initialises_down(void)
{
    //Arrange
    press_button(DOWN);


    // Act
    uint8_t result = checkButton(DOWN);

    // Assert
    TEST_ASSERT_EQUAL(PUSHED, result);
}

void test_buttons4_initialises_left(void)
{
    //Arrange

    press_button(LEFT);


    // Act
    uint8_t result = checkButton(LEFT);

    // Assert
    TEST_ASSERT_EQUAL(PUSHED, result);
}

void test_buttons4_initialises_right(void)
{
    //Arrange

    press_button(RIGHT);


    // Act
    uint8_t result = checkButton(RIGHT);

    // Assert
    TEST_ASSERT_EQUAL(PUSHED, result);
}

void test_buttons4_debouncing(void)
{
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;

    //One update should not be considered a push
    updateButtons();

    // Act
    uint8_t result = checkButton(UP);

    // Assert
    TEST_ASSERT_EQUAL(NO_CHANGE, result);
}

void test_buttons4_isDown_true(void) {

    press_button(DOWN);
    TEST_ASSERT_EQUAL(true, isDown(DOWN));
}

void test_buttons4_isDown_false(void) {
    press_button(DOWN);
    press_button(-1);
    TEST_ASSERT_EQUAL(false, isDown(DOWN));
}
