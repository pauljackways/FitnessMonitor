#ifndef ADC_HAL_TIVA_H
#define ADC_HAL_TIVA_H


#include <stdint.h>
#include "inc/hw_memmap.h"
#include "adc_hal.h"

typedef void(*callback)(uint32_t);

void adc_hal_register(adc_id_t id, callback cb);

#endif