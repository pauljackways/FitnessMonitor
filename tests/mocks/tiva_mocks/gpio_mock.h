#include "fff.h"
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_BUTTONS_FAKES_LIST(FUNC)        \
    FUNC(GPIOPinTypeGPIOInput)           \
    FUNC(GPIOPadConfigSet)            \
    FUNC(GPIOPinRead) \
    // FUNC(GPIO_PORTF_LOCK_R) \
    // FUNC(GPIO_PORTF_CR_R) 

//typedef void (*void_function_ptr_type)(void);

// VALUE_FUNC(uint32_t, GPIO_PORTF_LOCK_R);
// VALUE_FUNC(uint32_t, GPIO_PORTF_CR_R);
VOID_FUNC(GPIOPinTypeGPIOInput, uint32_t, uint8_t);
VOID_FUNC(GPIOPadConfigSet, uint32_t, uint8_t, uint32_t, uint32_t);
VALUE_FUNC(int32_t, GPIOPinRead, uint32_t, uint8_t);
