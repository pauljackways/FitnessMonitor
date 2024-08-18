#ifndef ADC_READ_H_
#define ADC_READ_H_

#include <stdint.h>

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void adc_callback(uint32_t);

void initADC (void);
void pollADC(void);         // Called periodically at high frequency
uint32_t readADC(void);     // Call to read the averaged readings from the buffer

#endif //ADC_READ_H_
