#include "unity.h"
#include "adc_hal_tiva.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations
#define FAKE_ADC_VALUE 0xADCFACCE

#include "tiva_mocks/adc_mock.h"
#include "tiva_mocks/sysctl_mock.h"


/* Custom fakes */
int32_t ADCSequenceDataGet_fake_adc_value(uint32_t arg0, uint32_t arg1,
                                  uint32_t *arg2)
{
    (void)arg0;
    (void)arg1;
    *arg2 = FAKE_ADC_VALUE;
    return 0;
}

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
static uint32_t callback_value = 0;

void dummy_callback(uint32_t value) {
    callback_count++;  
    callback_value = value;
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
    callback_count = 0;
    callback_value = 0;
}

void tearDown(void)
{
    
}

// • When ADC is registered with ID 1, the correct ADC is enabled
void test_adc_hal_registers_correct_adc(void) {
    //Arrange
    adc_hal_register(TIVA_ADC1, dummy_callback);

    //Act
    //Assert
    TEST_ASSERT_EQUAL(1, SysCtlPeripheralEnable_fake.call_count);
    TEST_ASSERT_EQUAL(SYSCTL_PERIPH_ADC0, SysCtlPeripheralEnable_fake.arg0_val);
}

// • When ADC is registered with ID 1, the sequence is enabled with correct arguments
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
    TEST_ASSERT_EQUAL((ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END), ADCSequenceStepConfigure_fake.arg3_val);
}

// • ... More test cases for the registration function to initialise ADC
void test_adc_hal_registration_validation(void) {
    //Arrange

    //Act

    //Assert
    TEST_ASSERT_EQUAL(1,1);
}

// • When ADC is registered with an invalid ID, ADC is not initialised
void test_adc_hal_incorrect_id_doesnt_initialise(void) {
    //Arrange
    adc_hal_register(FAKE_ADC1, dummy_callback);

    //Act

    //Assert
    TEST_ASSERT_EQUAL(0, ADCSequenceConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCSequenceStepConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCSequenceEnable_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCIntRegister_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCIntEnable_fake.call_count);

}
// • When ADC is registered with ID 1 and a null function pointer, ADC is not initialised
void test_adc_hal_null_callback_not_initialised(void) {
    //Arrange
    adc_hal_register(TIVA_ADC1, (void*)0);

    //Act

    //Assert
    TEST_ASSERT_EQUAL(0, ADCSequenceConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCSequenceStepConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCSequenceEnable_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCIntRegister_fake.call_count);
    TEST_ASSERT_EQUAL(0, ADCIntEnable_fake.call_count);
}

// • When start conversion is called with ID 1, the ADC is triggered
void test_adc_hal_triggers_conversion(void) {
    //Arrange
    adc_hal_register(TIVA_ADC1, dummy_callback);

    //Act
    adc_hal_start_conversion(TIVA_ADC1);

    //Assert
    TEST_ASSERT_EQUAL(1, ADCProcessorTrigger_fake.call_count);
}

// • When start conversion is called with invalid ID, no ADC is triggered
void test_adc_hal_invalid_id_doesnt_trigger_conversion(void) {
    //Arrange
    adc_hal_register(TIVA_ADC1, dummy_callback);

    //Act
    adc_hal_start_conversion(FAKE_ADC1);

    //Assert
    TEST_ASSERT_EQUAL(0, ADCProcessorTrigger_fake.call_count);
}

// • When start conversion is called with valid ID, but before registering, no ADC is triggered
void test_adc_hal_unregistered_id_doesnt_trigger_conversion(void) {
    //Arrange

    //Act
    adc_hal_start_conversion(FAKE_ADC1);

    //Assert
    TEST_ASSERT_EQUAL(0, ADCProcessorTrigger_fake.call_count);
}

// • Given ADC is registered with ID 1, when the ISR is called, the correct ADC channel is read
void test_adc_hal_int_reads_correct_channel_and_sequence(void)
{
    //Arrange
    adc_hal_register(TIVA_ADC1, dummy_callback);
    
    //Act
    ADC1_IntHandler();

    //Assert
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceDataGet_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceDataGet_fake.arg1_val);

}

void test_adc_int_clears_interrupt(void)
{
    //Arrange
    adc_hal_register(TIVA_ADC1, dummy_callback);
    
    //Act
    ADC1_IntHandler();

    //Assert
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntClear_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntClear_fake.arg1_val);
    TEST_ASSERT_EQUAL(1, ADCIntClear_fake.call_count);

}

// • Given ADC is registered with ID 1, when the ISR is called, the correct call-back is called
void test_adc_hal_int_calls_callback(void) {
    //Arrange
    adc_hal_register(TIVA_ADC1, dummy_callback);
    
    //Act
    ADC1_IntHandler();

    //Assert
    TEST_ASSERT_EQUAL(1, callback_count);
}

// • Given ADC is registered with ID 1, when the ISR is called, the correct value is passed into the
// call-back
void test_adc_hal_callback_value_correct(void) {
    //Arrange
    adc_hal_register(TIVA_ADC1, dummy_callback);
    ADCSequenceDataGet_fake.custom_fake = ADCSequenceDataGet_fake_adc_value;
    uint32_t ticks_wait = 1000;

    //Act
    ADC1_IntHandler();
    while (callback_count == 0 && ticks_wait > 0) {
        ticks_wait--;
    }

    //Assert
    TEST_ASSERT_EQUAL(FAKE_ADC_VALUE, callback_value);
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