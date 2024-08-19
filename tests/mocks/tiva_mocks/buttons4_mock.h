#include "fff.h"
#include "buttons4.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_BUTTONS_FAKES_LIST(FUNC) \
    FUNC(initButtons)               \
    FUNC(updateButtons)             \
    FUNC(checkButton)               \
    FUNC(isDown)

VOID_FUNC(initButtons);
VOID_FUNC(updateButtons);
VALUE_FUNC(uint8_t, checkButton, uint8_t);
VALUE_FUNC(bool, isDown, uint8_t);