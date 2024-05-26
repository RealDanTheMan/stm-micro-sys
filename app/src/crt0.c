// Startup code initializes MCU, its pretty low level and its  tightly coupled with the linker script
// Any changes in the linker script will have to be reflected here
// Any symbol prefixed with '__' is hard referenced in the linker script

#include "../../corelib/src/irq.h"
#include "../../corelib/src/sys.h"

// Forward declarations
void FlashToRAM(unsigned long *src, unsigned long *dest, unsigned long *end);
void ZeroMemory(unsigned long *from, unsigned long *to);
extern void main(void);

// Memory addresses from the linker script
extern unsigned long __data_flash;
extern unsigned long __data_begin;
extern unsigned long __data_end;
extern unsigned long __bss_start__;
extern unsigned long __bss_end__;
extern unsigned long __stack_end;

// Dummy handler used for all error states
void __err(void)
{
    while(1)
    {
        // NOP
    }
}

// Dummy handler used for all other interrupt handlers that we do not care at the moment
void __wait(void)
{
    while(1)
    {
        // NOP
    }
}

// This is the boot entry point
// The MCU entry point is linked to this method via the linker script
// This method is called as soon as the MCU board is reset
void IRQ_RESET_Handler(void)
{
    // Copy flash sys data to memory
    FlashToRAM(&__data_flash, &__data_begin, &__data_end);
    ZeroMemory(&__bss_start__, &__bss_end__);

    // Before we configure onboard PLL clock to maximum speed 
    // we need an interim source of clock in a meantime - we choose internal oscilator (HSI)
    CLKResetClocks();
    CLKInitSysClock(CLK_SRC_HSI);

    // Once onboard HSI clock is ready we can begin setting up PLL clock & internal system tick event
    CLKInitSysClock(CLK_SRC_PLL);
    SYSConfigureTick(g_SysClkFreq / 1000000UL); // Systick will trigger every micro second

    // Enter application scope
    main();
}

/// This is the sys tick interrupt handler
/// The frequency of sys tick interrupt is configured via ConfigureSysTick()
/// Lets us measure the time since boot
void IRQ_SYSTICK_Handler(void)
{
    g_SysClkTk++;
}

void IRQ_HARDFAULT_Handler(void)
{
    __err();
}

void IRQ_MEMFAULT_Handler(void)
{
    __err();
}

void IRQ_USAGEFAULT_Handler(void)
{
    __err();
}

void IRQ_BUSFAULT_Handler(void)
{
    __err();
}

/// Copies data from FlashROM source to RAM destination
void FlashToRAM(unsigned long *src, unsigned long *dest, unsigned long *end)
{
    for(unsigned long *addr = dest; addr < end;)
    {
        *(addr++) = *(src++);
    }
}

/// Zeros out given memory block
void ZeroMemory(unsigned long *from, unsigned long *to)
{
    for(unsigned long *addr = from; addr < to; addr++)
    {
        *(addr) = 0;
    }
}

void _exit(int satus)
{
    while(1); // hang on endless loop
}

/// This is our interrupt vector map, it is part of ARM architecture requirement
/// Via this table we can bind handler method pointers to certain ARM events
/// For now we only care about the reset event and stack pointer and ignore the rest via a dummy def handler
/// This definition is tightly coupled with the linker script
__attribute__ ((section(".vec_table")))
void (* const interrupt_vector_table[])(void) =
{
    (void *) &__stack_end,      // Stack pointer
    IRQ_RESET_Handler,          // Reset interrupt handler
    __wait,                     // NMI interrupt handler
    IRQ_HARDFAULT_Handler,      // Hard fault interrupt handler
    IRQ_MEMFAULT_Handler,       // Memory fault interrupt handler
    IRQ_BUSFAULT_Handler,       // Bus fault interrupt handler
    IRQ_USAGEFAULT_Handler,     // Usage fault interrupt handler
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    __wait,                     // SV Call interrupt handler
    __wait,                     // Debugging hadnler
    0,                          // -- Reserved by chipset
    __wait,                     // Pend SV interrupt handler
    IRQ_SYSTICK_Handler,        // Sys tick interrupt handler
    __wait,                     // WWDG - Window Watchdog interrupt
    __wait,                     // PVD - PVD through EXTI line 16 detection interrupt
    __wait,                     // TAMPER_STAMP
    __wait,                     // RTC_WKUP
    __wait,                     // FLASH
    __wait,                     // RCC
    __wait,                     // EXTI0
    __wait,                     // EXTI1
    __wait,                     // EXTI2_TS
    __wait,                     // EXTI3
    __wait,                     // EXTI4
    __wait,                     // DMA1_Channel1
    __wait,                     // DMA1_Channel2
    __wait,                     // DMA1_Channel3
    __wait,                     // DMA1_Channel4
    __wait,                     // DMA1_Channel5
    __wait,                     // DMA1_Channel6
    __wait,                     // DMA1_Channel7
    __wait,                     // ADC1_2
    __wait,                     // CAN_TX
    __wait,                     // CAN_RX0
    __wait,                     // CAN_RX1
    __wait,                     // CAN_SCE
    __wait,                     // EXTI9_5
    __wait,                     // TIM1_BRK/TIM15
    __wait,                     // TIM1_UP/TIM16
    __wait,                     // TIM1_TRG_COM/TIM17
    __wait,                     // TIM1_CC
    __wait,                     // TIM2
    __wait,                     // TIM3
    0,                          // -- Reserved by chipset
    __wait,                     // I2C1_EV
    __wait,                     // I2C1_ER
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    __wait,                     // SPI1
    0,                          // -- Reserved by chipset
    IRQ_USART1_Handler,         // USART1
    IRQ_USART2_Handler,         // USART2
    IRQ_USART3_Handler,         // USART3
    __wait,                     // EXTI15_10
    __wait,                     // RTC_Alarm
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0,                          // -- Reserved by chipset
    0                           // -- Reserved by chipset
};