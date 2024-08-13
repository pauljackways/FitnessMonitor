#ifndef ADC_HAL_H
#define ADC_HAL_H


#include <stdint.h>


typedef enum {
    TIVA_ADC1 = 1,  // Assign 1 to ADC_ID_1
    TIVA_ADC2,
    FAKE_ADC1 = 10 // Fake ADC to simulate ADC from another vendor
    // Add more ADC IDs as needed
} adc_id_t;

// Ranges for ADC definitions - each hardware vendor gets 10 allocations
#define TIVA_ADC_START TIVA_ADC1
#define TIVA_ADC_END (TIVA_ADC1 + 9)
#define FAKE_ADC_START FAKE_ADC1
#define FAKE_ADC_END (FAKE_ADC + 9)

// Declare interrupt handlers for each ADC
void ADC1_IntHandler(void);
void ADC2_IntHandler(void);

// Define the callback type
typedef void(*callback)(uint32_t);

// Function to register an ADC and a callback
void adc_hal_register(adc_id_t id, callback cb);

// Function to start ADC conversion
void adc_hal_start_conversion(adc_id_t id);

#endif // ADC_HAL_H;