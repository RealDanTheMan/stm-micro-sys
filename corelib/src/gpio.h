#ifndef GPIO_H
#define GPIO_H

#include "platform.h"

typedef struct 
{
    uint32_t m_mode;
    uint32_t m_pin;
    uint32_t m_port;

} GPIODesc;

#define GPIO_PORT_OFFSET (0x0400U)

#define GPIO_MODE_READ (0U)
#define GPIO_MODE_WRITE (1U)
#define GPIO_MODE_ALT (2U)
#define GPIO_MODE_ANALOG (3U)

#define GPIO_ALT_FREQ_LO (0U)
#define GPIO_ALT_FREQ_HI (1U)

#define GPIO_OUT_TYPE_PUSHPULL (0U)
#define GPIO_OUT_TYPE_OPENDRAIN (1U)

#define GPIO_SPEED_LOW      (0U)
#define GPIO_SPEED_MEDIUM   (2U)
#define GPIO_SPEED_HIGH     (3U)

#define GPIO_VAL_LOW (0U)
#define GPIO_VAL_HIGH (1U)

#if PLATFORMID == PLAT_NUCLEO32F401RE
    // GPIO Port enumerators
    #define GPIO_PORTA (0U)
    #define GPIO_PORTB (1U)
    #define GPIO_PORTC (2U)
    #define GPIO_PORTD (3U)
    #define GPIO_PORTE (4U)
    #define GPIO_PORTH (7U)

#elif PLATFORMID == PLAT_NUCLEO32F303K8
    #define GPIO_PORTA (0U)
    #define GPIO_PORTB (1U)
    #define GPIO_PORTC (2U)
    #define GPIO_PORTD (3U)
    #define GPIO_PORTF (5U)

#endif

// GPIO Pins id enumerators
#define GPIO_PIN0 (0U)
#define GPIO_PIN1 (1U)
#define GPIO_PIN2 (2U)
#define GPIO_PIN3 (3U)
#define GPIO_PIN4 (4U)
#define GPIO_PIN5 (5U)
#define GPIO_PIN6 (6U)
#define GPIO_PIN7 (7U)
#define GPIO_PIN8 (8U)
#define GPIO_PIN9 (9U)
#define GPIO_PIN10 (10U)
#define GPIO_PIN11 (11U)
#define GPIO_PIN12 (12U)
#define GPIO_PIN13 (13U)
#define GPIO_PIN14 (14U)
#define GPIO_PIN15 (15U)

// GPIO Function Multiplexing Enumerators
#define GPIO_FN0 (0U)
#define GPIO_FN1 (1U)
#define GPIO_FN2 (2U)
#define GPIO_FN3 (3U)
#define GPIO_FN4 (4U)
#define GPIO_FN5 (5U)
#define GPIO_FN6 (6U)
#define GPIO_FN7 (7U)
#define GPIO_FN8 (8U)
#define GPIO_FN9 (9U)
#define GPIO_FN10 (10U)
#define GPIO_FN11 (11U)
#define GPIO_FN12 (12U)
#define GPIO_FN13 (13U)
#define GPIO_FN14 (14U)
#define GPIO_FN15 (15U)

extern void GPIOEnableBusClock(const uint32_t port);
extern GPIO_TypeDef* GPIOGetReg(const uint32_t port);
extern void GPIOEnable(const uint32_t port, const uint32_t pin, const uint32_t mode, GPIODesc *outGPIO);
extern void GPIODisable(const GPIODesc* gpio);
extern void GPIOSetMode(const GPIODesc* gpio, const uint32_t mode);
extern void GPIOSetAltFn(const GPIODesc* gpio, const uint32_t fn);
extern void GPIOSetOutputType(const GPIODesc* gpio, const uint32_t type);
extern void GPIOSetOutputSpeed(const GPIODesc* gpio, const uint32_t speed);
extern void GPIOWrite(const GPIODesc* gpio, const uint32_t val);
extern void GPIORead(const GPIODesc* gpio, uint32_t *val);
extern void GPIOToggle(const GPIODesc* gpio);



#endif