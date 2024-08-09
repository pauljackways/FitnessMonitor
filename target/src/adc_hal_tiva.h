#ifndef ADC_HAL_TIVA_H
#define ADC_HAL_TIVA_H


#include <stdint.h>


// Declare interrupt handlers for each ADC
void ADC1_IntHandler(void);
void ADC2_IntHandler(void);

#endif // ADC_HAL_H;