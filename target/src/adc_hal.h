#ifndef ADC_HAL_H
#define ADC_HAL_H


#include <stdint.h>

typedef enum {
    ADC_ID_1 = 1,  // Assign 1 to ADC_ID_1
    ADC_ID_2,    
    ADC_ID_3,      
    // Add more ADC IDs as needed
} adc_id_t;

// Define the callback type
typedef void(*callback)(uint32_t);

// Function to register an ADC and a callback
void adc_hal_register(adc_id_t id, callback cb);

// Function to start ADC conversion
void adc_hal_start_conversion(int id);

#endif // ADC_HAL_H