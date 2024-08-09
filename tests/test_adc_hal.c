#include "unity.h"
#include "adc_hal.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#include "tiva_mocks/adc_mock.h"
#include "tiva_mocks/sysctl_mock.h"

// • When ADC is registered with ID 1, the correct ADC is enabled
// • When ADC is registered with ID 1, the sequence is enabled with correct arguments
// • ... More test cases for the registration function to initialise ADC
// • When ADC is registered with an invalid ID, ADC is not initialised
// • When ADC is registered with ID 1 and a null function pointer, ADC is not initialised
// • When start conversion is called with ID 1, the ADC is triggered
// • When start conversion is called with invalid ID, no ADC is triggered
// • Given ADC is registered with ID 1, when the ISR is called, the correct ADC channel is read
// • Given ADC is registered with ID 1, when the ISR is called, interrupt is cleared
// • Given ADC is registered with ID 1, when the ISR is called, the correct call-back is called
// • Given ADC is registered with ID 1, when the ISR is called, the correct value is passed into the
// call-back

/* Helper functions */
void reset_fff(void)
{
    FFF_ADC_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

void dummy_callback(uint32_t) {

}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
}

void tearDown(void)
{
    
}

void test_adc_hal_registers_an_adc(void) {

    //Arrange
    adc_hal_register(ADC_ID_1, dummy_callback);

    //Act


    //Assert
    //TEST_ASSERT_EQUAL(1,1);

}