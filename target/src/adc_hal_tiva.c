#include <stdint.h>
#include <stdbool.h>
#include "adc_hal.h"
#include "adc_hal_tiva.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"

// Define macros for generating interrupt handler names

typedef void (*adc_handler)(void);  // Define a type for the ADC interrupt handler

typedef struct {
    uint32_t base;
    uint32_t sequence;
    callback callback;
    adc_handler handler;  // Pointer to the interrupt handler
} adc_config_t;

static adc_config_t adc_configs[] = {
    [TIVA_ADC1] = {ADC0_BASE, 3, (void*)0, ADC1_IntHandler},
    [TIVA_ADC2] = {ADC1_BASE, 3, (void*)0, ADC2_IntHandler}
};

void ADC1_IntHandler(void) {
    uint32_t value;
    ADCSequenceDataGet(ADC0_BASE, 3, &value);
    if (adc_configs[TIVA_ADC1].callback) {
        adc_configs[TIVA_ADC1].callback(value);
    }
    ADCIntClear(ADC0_BASE, 3);
}

void ADC2_IntHandler(void) {
    uint32_t value;
    ADCSequenceDataGet(ADC1_BASE, 3, &value);
    if (adc_configs[TIVA_ADC2].callback) {
        adc_configs[TIVA_ADC2].callback(value);
    }
    ADCIntClear(ADC1_BASE, 3);
}

void adc_hal_register(adc_id_t id, callback cb) {
    adc_config_t* config = &adc_configs[id];

    if (config->base == 0) {
        // Invalid ADC ID, handle error
        return;
    }

    config->callback = cb;

    // Enable the ADC peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Configure the ADC sequence
    ADCSequenceConfigure(config->base, config->sequence, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(config->base, config->sequence, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

    // Enable the ADC sequence
    ADCSequenceEnable(config->base, config->sequence);

    // Register the interrupt handler
    ADCIntRegister(config->base, config->sequence, config->handler);

    // Enable ADC interrupts
    ADCIntEnable(config->base, config->sequence);
}

void adc_hal_start_conversion(adc_id_t id) {
    switch (id) {
        case TIVA_ADC1:
            ADCProcessorTrigger(ADC0_BASE, 3);
            break;
        case TIVA_ADC2:
            ADCProcessorTrigger(ADC1_BASE, 3);
            break;
        default:
            break;
    }
}