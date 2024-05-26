#include "sys.h"

/// Configures the system tick interupt event to be fired every given ticks
/// Make sure the system clock is setup before calling this
void SYSConfigureTick(uint32_t interval)
{
    // Disable SysTick during setup
    SysTick->CTRL = 0;

    // Set reload value
    SysTick->LOAD = interval - 1;

    // Set SysTick interrupt priority (optional)
    NVIC_SetPriority(SysTick_IRQn, 0);

    // Clear current value
    SysTick->VAL = 0;

    // Enable SysTick with processor clock source and enable interrupts (optional)
    __SET(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);
    __SET(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
    __SET(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

// Counts number of systicks
void SYSWait(const uint32_t tks)
{
    uint32_t st = SYSGetTimeTk();
    while(SYSGetTimeTk() - st < tks)
    {
        // wait
    }
}

// Counts number of microseconds
void SYSWaitUs(const uint32_t us)
{
    // By default system is configured to tick every microsecond
    // So for now SYSWaitMsUs & SYSWaitMs are synonymous
    SYSWait(us);
}

// Counts number of milliseconds
void SYSWaitMs(const uint32_t ms)
{
    SYSWaitUs(ms * 1000UL);
}


// Get number of ticks since system start
extern uint32_t SYSGetTimeTk()
{
    return g_SysClkTk;
}

// Get number of microseconds since system start
extern uint32_t SYSGetTimeUs()
{
    // By default system is configured to tick every microsecond
    // So for now SYSGetTimeUs & SYSGetTimeTk are synonymous

    return SYSGetTimeTk();
}

// Get number of milliseconds since system start
extern uint32_t SYSGetTimeMs()
{
    return SYSGetTimeUs() / 1000UL;
}