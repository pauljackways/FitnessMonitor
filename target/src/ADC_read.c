//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones	UCECE
// Last modified:	8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "adc_hal.h"
#include "circBufT.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define ADC_BUF_SIZE 10

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t ADC_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void pollADC(void)
{
    //
    // Initiate a conversion
    //
    adc_hal_start_conversion(TIVA_ADC1);
//    g_ulSampCnt++;
}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void ADCIntHandler(uint32_t ulValue)
{	
	//
	// Place it in the circular buffer (advancing write index)
	writeCircBuf (&ADC_inBuffer, ulValue);
	//                         
}

void adc_callback(uint32_t ulValue) {
    ADCIntHandler(ulValue);
}

//*****************************************************************************
// Initialisation functions for the ADC
//*****************************************************************************

void initADC (void)
{
    //
    initCircBuf (&ADC_inBuffer, ADC_BUF_SIZE);
    // The ADC0 peripheral must be enabled for configuration and use.
    adc_hal_register(TIVA_ADC1, adc_callback);

}

uint32_t readADC() {
      uint32_t sum = 0;
      for (uint16_t i = 0; i < ADC_BUF_SIZE; i++)
          sum += readCircBuf (&ADC_inBuffer);

    return (sum + ADC_BUF_SIZE / 2) / ADC_BUF_SIZE;
}

