#ifndef IRQ_H
#define IRQ_H

#include "platform.h"

extern void IRQ_RESET_Handler(void);
extern void IRQ_HARDFAULT_Handler(void);
extern void IRQ_MEMFAULT_Handler(void);
extern void IRQ_USAGEFAULT_Handler(void);
extern void IRQ_BUSFAULT_Handler(void);

extern void IRQ_SYSTICK_Handler(void);

#if PLATFORMID == PLAT_NUCLEO32F401RE
    extern void IRQ_USART1_Handler(void);
    extern void IRQ_USART2_Handler(void);
    extern void IRQ_USART3_Handler(void);
    extern void IRQ_USART4_Handler(void);
    extern void IRQ_USART5_Handler(void);
    extern void IRQ_USART6_Handler(void);

#elif PLATFORMID == PLAT_NUCLEO32F303K8
    extern void IRQ_USART1_Handler(void);
    extern void IRQ_USART2_Handler(void);
    extern void IRQ_USART3_Handler(void);

#endif

#endif