#ifndef ADC_HAL_TIVA_H
#define ADC_HAL_TIVA_H


#include <stdint.h>
#include "inc/hw_memmap.h"
#include "adc_hal.h"

typedef void (*adc_handler)(void);  // Type for the ADC interrupt handler

typedef struct { // Struct object for ADC configuration
    uint32_t base;
    uint32_t sequence;
    callback callback;
    adc_handler handler;
} adc_config_t;

static adc_config_t adc_configs[] = {  // ADC configuration object initialiations 
    [TIVA_ADC1] = {ADC0_BASE, 3, (void*)0, ADC1_IntHandler},
    [TIVA_ADC2] = {ADC1_BASE, 3, (void*)0, ADC2_IntHandler}
}; // Add more ADC configurations as needed

#endif // ADC_HAL_H;