#include "unity.h"
#include "buttons4.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)


#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#define DEFAULT_BUT UP

#include "tiva_mocks/gpio_mock.h"
#include "tiva_mocks/sysctl_mock.h"

ButtonPressType dummy = NO_CHANGE;

/* Helper functions */      
void reset_fff(void)
{
    FFF_BUTTONS_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

// Checks if the state changes and returns the most recent change of button press type
ButtonPressType updateLoop(uint32_t loops, ButtonPressType target){
    ButtonPressType test = NO_CHANGE;
    ButtonPressType result = NO_CHANGE;
    for (volatile uint32_t i = 0; i < loops; i++) {
        test = checkPressType(DEFAULT_BUT);
        if (test != NO_CHANGE) {
            result = test;
        }
    }
    return result;
}

//Presses or releases button - alternative function if updateloop istn working
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
            if (butState == PUSHED) {
                GPIOPinRead_fake.return_val = DOWN_BUT_PIN;
            } else {
                GPIOPinRead_fake.return_val = 0;
            }
            break;
        case LEFT:
            GPIOPinRead_fake.arg0_val = LEFT_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = LEFT_BUT_PIN;
            if (butState == PUSHED) {
                GPIOPinRead_fake.return_val = LEFT_BUT_PIN;
            } else {
                GPIOPinRead_fake.return_val = 0;
            }
            break;
        case RIGHT:
            GPIOPinRead_fake.arg0_val = RIGHT_BUT_PORT_BASE;
            GPIOPinRead_fake.arg1_val = RIGHT_BUT_PIN;
            if (butState == PUSHED) {
                GPIOPinRead_fake.return_val = RIGHT_BUT_PIN;
            } else {
                GPIOPinRead_fake.return_val = 0;
            }
            break;
        default:
            return;
    }
    ButtonPressType result = NO_CHANGE;
    for (volatile int32_t i = 0; i < TEST_DURATION; i++) {
        result = updateLoop(1, NO_CHANGE);
    }
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
    if (!isUnpressed(UP)) {
        GPIOPinRead_fake.return_val = !GPIOPinRead_fake.return_val;
    }
    dummy = updateLoop(TEST_DURATION*3, NO_CHANGE);
}

void tearDown(void)
{
    
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
    uint8_t result = isUnpressed(UP);

    // Assert
    TEST_ASSERT_EQUAL(true, result);
}

void test_buttons4_initialises_up(void)
{
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;

    //Act/Assert
    dummy = updateLoop(TEST_DURATION, NO_CHANGE);

    // Act
    uint8_t result = isUnpressed(UP);

    // Assert
    TEST_ASSERT_EQUAL(false, result);
}

void test_buttons4_initialises_down(void)
{
    //Arrange
    GPIOPinRead_fake.arg0_val = DOWN_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = DOWN_BUT_PIN;
    GPIOPinRead_fake.return_val = DOWN_BUT_PIN;

    //Act/Assert
    dummy = updateLoop(TEST_DURATION, NO_CHANGE);

    // Act
    uint8_t result = isUnpressed(DOWN);

    // Assert
    TEST_ASSERT_EQUAL(false, result);
}

void test_buttons4_initialises_left(void)
{
    //Arrange
    GPIOPinRead_fake.arg0_val = LEFT_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = LEFT_BUT_PIN;
    GPIOPinRead_fake.return_val = LEFT_BUT_PIN;

    //Act/Assert
    dummy = updateLoop(TEST_DURATION, NO_CHANGE);

    // Act
    uint8_t result = isUnpressed(LEFT);

    // Assert
    TEST_ASSERT_EQUAL(false, result);
}

void test_buttons4_initialises_right(void)
{
    //Arrange
    GPIOPinRead_fake.arg0_val = RIGHT_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = RIGHT_BUT_PIN;
    GPIOPinRead_fake.return_val = RIGHT_BUT_PIN;

    //Act/Assert
    dummy = updateLoop(TEST_DURATION, NO_CHANGE);

    // Act
    uint8_t result = isUnpressed(RIGHT);

    // Assert
    TEST_ASSERT_EQUAL(false, result);
}

void test_buttons4_clear(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = 0;

    //Act/Assert
    dummy = updateLoop(TEST_DURATION, NO_CHANGE);
    TEST_ASSERT_EQUAL(true, isUnpressed(UP));

    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    dummy = updateLoop(TEST_DURATION, NO_CHANGE);
    TEST_ASSERT_EQUAL(false, isUnpressed(UP));
}

void test_buttons4_short_true(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    ButtonPressType result = NO_CHANGE;

    //Act
    result = updateLoop(TEST_DURATION/4, NO_CHANGE);

    GPIOPinRead_fake.return_val = 0;
    result = updateLoop(TEST_DURATION + 1, SHORT);
    //Assert
    TEST_ASSERT_EQUAL(SHORT, result);
}

void test_buttons4_short_false(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    ButtonPressType result = NO_CHANGE;


    //Act

    result = updateLoop(TEST_DURATION + 1, SHORT);

    GPIOPinRead_fake.return_val = 0;
    result = updateLoop(TEST_DURATION + 1, SHORT);
    //Assert
    TEST_ASSERT_EQUAL(NO_CHANGE, result);
}

void test_buttons4_long_true(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    ButtonPressType result = NO_CHANGE;


    //Act
    result = updateLoop(TEST_DURATION*5, LONG);

    //Assert
    TEST_ASSERT_EQUAL(LONG, result);
}

void test_buttons4_long_false(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    ButtonPressType result = NO_CHANGE;


    //Act
    result = updateLoop(TEST_DURATION-2, LONG);
    //Assert
    TEST_ASSERT_EQUAL(NO_CHANGE, result);
}


void test_buttons4_double_true_long(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    ButtonPressType result = NO_CHANGE;


    //Act
    dummy = updateLoop(TEST_DURATION/4, NO_CHANGE);
    GPIOPinRead_fake.return_val = 0;
    dummy = updateLoop(TEST_DURATION/4, NO_CHANGE);
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    result = updateLoop(TEST_DURATION, DOUBLE);
    GPIOPinRead_fake.return_val = 0;
    result = updateLoop(TEST_DURATION/4, DOUBLE);


    //Assert
    TEST_ASSERT_EQUAL(DOUBLE, result);
}

void test_buttons4_double_true_short(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    ButtonPressType result = NO_CHANGE;


    //Act
    dummy = updateLoop(TEST_DURATION/4, NO_CHANGE);
    GPIOPinRead_fake.return_val = 0;
    dummy = updateLoop(TEST_DURATION/4, NO_CHANGE);
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    result = updateLoop(TEST_DURATION/4, DOUBLE);
    GPIOPinRead_fake.return_val = 0;
    result = updateLoop(TEST_DURATION/8, DOUBLE);


    //Assert
    TEST_ASSERT_EQUAL(DOUBLE, result);
}


void test_buttons4_double_false_gap_too_long(void) {
    //Arrange
    GPIOPinRead_fake.arg0_val = UP_BUT_PORT_BASE;
    GPIOPinRead_fake.arg1_val = UP_BUT_PIN;
    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    ButtonPressType result = NO_CHANGE;


    //Act
    dummy = (TEST_DURATION/4, NO_CHANGE);

    GPIOPinRead_fake.return_val = 0;
    result = updateLoop(TEST_DURATION+5, NO_CHANGE);

    GPIOPinRead_fake.return_val = UP_BUT_PIN;
    result = updateLoop(TEST_DURATION/2, DOUBLE);

    //Assert
    TEST_ASSERT_EQUAL(NO_CHANGE, result);
}

