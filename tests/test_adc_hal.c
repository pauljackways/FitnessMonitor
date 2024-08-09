#include "unity.h"
#include "adc_hal.h"
#include "adc_hal_tiva.h"

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

void assert_f1_called_before_f2(void* f1, void* f2)
{
    int8_t i_f1 = -1;
    int8_t i_f2 = -1;

    for (uint8_t i = 0; i < FFF_CALL_HISTORY_LEN; i++)
    {
        if(fff.call_history[i] == NULL)
            break;

        if (i_f1 == -1 && fff.call_history[i] == f1) i_f1 = i;
        if (i_f2 == -1 && fff.call_history[i] == f2) i_f2 = i;        
    }
    
    TEST_ASSERT(i_f1 < i_f2);
}

static uint32_t callback_count = 0;

void dummy_callback(uint32_t) {
    callback_count++;    
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
}

void tearDown(void)
{
    
}

void test_adc_hal_enables_adc(void)
{
    // Act
    adc_hal_register(TIVA_ADC1, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, SysCtlPeripheralEnable_fake.call_count);
    TEST_ASSERT_EQUAL(SYSCTL_PERIPH_ADC0, SysCtlPeripheralEnable_fake.arg0_val);
}

void test_adc_hal_initialises_adc_sequence(void)
{
    // Act
    adc_hal_register(TIVA_ADC1, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceConfigure_fake.arg1_val);  
    TEST_ASSERT_EQUAL(ADC_TRIGGER_PROCESSOR, ADCSequenceConfigure_fake.arg2_val);
    TEST_ASSERT_EQUAL(0, ADCSequenceConfigure_fake.arg3_val);
}

void test_adc_hal_initialises_adc_sequence_step(void)
{
    // Act
    adc_hal_register(TIVA_ADC1, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceStepConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceStepConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceStepConfigure_fake.arg1_val);  
    TEST_ASSERT_EQUAL(0, ADCSequenceStepConfigure_fake.arg2_val);
    TEST_ASSERT_EQUAL((ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END), ADCSequenceStepConfigure_fake.arg3_val);
}

void test_adc_hal_enables_adc_sequence(void)
{
    // Act
    adc_hal_register(TIVA_ADC1, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceEnable_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceEnable_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceEnable_fake.arg1_val);  
}

void test_adc_hal_registers_adc_interrupt(void)
{
    // Act
    adc_hal_register(TIVA_ADC1, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCIntRegister_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntRegister_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntRegister_fake.arg1_val);  
    TEST_ASSERT_EQUAL(ADC1_IntHandler, ADCIntRegister_fake.arg2_val);
}

void test_adc_hal_enables_adc_before_other_adc_operations(void)
{
    // Act
    adc_hal_register(TIVA_ADC1, dummy_callback);

    // Assert
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceConfigure);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceStepConfigure);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceEnable);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCIntRegister);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCIntEnable);
}

void test_adc_hal_enables_adc_interrupt(void)
{
    // Act
    adc_hal_register(TIVA_ADC1, dummy_callback);
    
    // Assert
    TEST_ASSERT_EQUAL(1, ADCIntEnable_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntEnable_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntEnable_fake.arg1_val);  
}