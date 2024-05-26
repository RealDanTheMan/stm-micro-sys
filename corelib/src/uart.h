#ifndef UART_H
#define UART_H

#include "clock.h"
#include "gpio.h"

// Depending on the platform different number of USART channel are available
// If they are not defined at this point we simply set them to NULL
// Max 8 USART channels are supported at this point in time
#ifndef USART1
#define USART1 (NULL)
#endif
#ifndef USART2
#define USART2 (NULL)
#endif
#ifndef USART3
#define USART3 (NULL)
#endif
#ifndef USART4
#define USART4 (NULL)
#endif
#ifndef USART5
#define USART5 (NULL)
#endif
#ifndef USART6
#define USART6 (NULL)
#endif
#ifndef USART7
#define USART7 (NULL)
#endif
#ifndef USART8
#define USART8 (NULL)
#endif

// USART channel enumerators
#define USART_CHANNEL1 (0UL)
#define USART_CHANNEL2 (1UL)
#define USART_CHANNEL3 (2UL)
#define USART_CHANNEL4 (3UL)
#define USART_CHANNEL5 (4UL)
#define USART_CHANNEL6 (5UL)
#define USART_CHANNEL7 (6UL)
#define USART_CHANNEL8 (7UL)

extern USART_TypeDef * USARTReg(const uint32_t channel);

extern void USARTSetGPIO(const uint32_t channel, const GPIODesc *gpio);
extern void USARTClkEnable(const uint32_t channel);
extern void USARTClkDisable(const uint32_t channel);
extern void USARTEnableIRQ(const uint32_t channel);
extern void USARTSetRate(const uint32_t channel, const uint32_t baud);
extern void USARTInit(const uint32_t channel, const GPIODesc *rx, const GPIODesc* tx);
extern void USARTWrite(const uint32_t channel, const char* msg, const uint32_t len);
extern void USARTWriteSingle(const uint32_t channel, char msg);
extern bool USARTReadSingle(const uint32_t channel, char* msg);
extern bool USARTRead(const uint32_t channel, char* msg, const uint32_t maxSize);

extern bool USARTGetReadReady(const uint32_t channel);
extern bool USARTGetBusy(const uint32_t channel);

#endif