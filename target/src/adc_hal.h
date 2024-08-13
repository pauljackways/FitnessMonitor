#ifndef ADC_HAL_H_
#define ADC_HAL_H_

#include <stdint.h>

#define ADC_ID_1 1

typedef enum {
    TIVA_ADC1 = 1,  // Assign 1 to ADC_ID_1
    TIVA_ADC2,
    FAKE_ADC1 = 10 // Fake ADC to simulate ADC from another vendor
    // Add more ADC IDs as needed
} adc_id_t;


typedef void(*callback)(uint32_t);

void adc_hal_register(adc_id_t id, callback cb);

#endif //ADC_READ_H_