#ifndef ADC_HAL_H_
#define ADC_HAL_H_

#include <stdint.h>

#define ADC_ID_1 1

typedef void(*callback)(uint32_t);

void adc_hal_register(uint32_t id, callback cb);

#endif //ADC_READ_H_