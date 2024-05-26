#include "clock.h"

uint32_t g_SysClkFreq = 0UL;
uint32_t g_SysClkTk = 0UL;

void CLKResetClocks(void)
{
    // Reset clock configuration
    RCC->CFGR = 0UL;

    #if PLATFORMID == PLAT_NUCLEO32F401RE
        RCC->PLLCFGR = 0x24003010;
    #endif

    __CLEAR(RCC->CR, RCC_CR_HSION);
    __CLEAR(RCC->CR, RCC_CR_HSEON);
    __CLEAR(RCC->CR, RCC_CR_CSSON);
    __CLEAR(RCC->CR, RCC_CR_PLLON);

    // Reset flash configuration
    CLKSetFlash(0UL);
}

void CLKSetFlash(const uint32_t waitStates)
{
    // TODO: ASSERT -- waitStates shuld use max presicion of 4 bits | waitStates < 16
    FLASH->CR = 0UL;
    // Enable prefetch
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        __SET(FLASH->ACR, FLASH_ACR_PRFTEN);
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        __SET(FLASH->ACR, FLASH_ACR_PRFTBE);
    #endif

    // Enable flash instructiona and data cache
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        __SET(FLASH->ACR, FLASH_ACR_ICEN);
        __SET(FLASH->ACR, FLASH_ACR_DCEN);
    #endif
 
    // Clear then set number of wait states aka latency
    __CLEAR(FLASH->ACR, FLASH_ACR_LATENCY_Pos);
    __CLEAR(FLASH->ACR, FLASH_ACR_LATENCY_Pos + 1UL);
    __CLEAR(FLASH->ACR, FLASH_ACR_LATENCY_Pos + 2UL);
    __CLEAR(FLASH->ACR, FLASH_ACR_LATENCY_Pos + 3UL);

    FLASH->ACR |= waitStates << 0;
}

void CLKSetPrescalers(const uint32_t ahb, const uint32_t apb1, const uint32_t apb2)
{
    // TODO: ASSERT -- param ahb shuld be power of 2 & max 512
    // TODO: ASSERT -- param apb1 shuld be power of 2 & max 16
    // TODO: ASSERT -- param apb2 shuld be power of 2 & max 16

    // Clear then set main clock prescaler
    __CLEAR (RCC->CFGR, RCC_CFGR_HPRE_0);
    __CLEAR (RCC->CFGR, RCC_CFGR_HPRE_1);
    __CLEAR (RCC->CFGR, RCC_CFGR_HPRE_2);
    __CLEAR (RCC->CFGR, RCC_CFGR_HPRE_3);
    switch(ahb)
    {
        case 1 : RCC->CFGR |= RCC_CFGR_HPRE_DIV1 << RCC_CFGR_HPRE_Pos; break;
        case 2 : RCC->CFGR |= RCC_CFGR_HPRE_DIV2 << RCC_CFGR_HPRE_Pos; break;
        case 4 : RCC->CFGR |= RCC_CFGR_HPRE_DIV4 << RCC_CFGR_HPRE_Pos; break;
        case 8 : RCC->CFGR |= RCC_CFGR_HPRE_DIV8 << RCC_CFGR_HPRE_Pos; break;
        case 16 : RCC->CFGR |= RCC_CFGR_HPRE_DIV16 << RCC_CFGR_HPRE_Pos; break;
        case 64 : RCC->CFGR |= RCC_CFGR_HPRE_DIV64 << RCC_CFGR_HPRE_Pos; break;
        case 128 : RCC->CFGR |= RCC_CFGR_HPRE_DIV128 << RCC_CFGR_HPRE_Pos; break;
        case 256 : RCC->CFGR |= RCC_CFGR_HPRE_DIV256 << RCC_CFGR_HPRE_Pos; break;
        case 512 : RCC->CFGR |= RCC_CFGR_HPRE_DIV512 << RCC_CFGR_HPRE_Pos; break;
    }

    // Clear then set peripheral bus 1 clock prescaler
    __CLEAR (RCC->CFGR, RCC_CFGR_PPRE1_0);
    __CLEAR (RCC->CFGR, RCC_CFGR_PPRE1_1);
    __CLEAR (RCC->CFGR, RCC_CFGR_PPRE1_2);
    switch(apb1)
    {
        case 1 : RCC->CFGR |= RCC_CFGR_PPRE1_DIV1 << RCC_CFGR_PPRE1_Pos; break;
        case 2 : RCC->CFGR |= RCC_CFGR_PPRE1_DIV2 << RCC_CFGR_PPRE1_Pos; break;
        case 4 : RCC->CFGR |= RCC_CFGR_PPRE1_DIV4 << RCC_CFGR_PPRE1_Pos; break;
        case 8 : RCC->CFGR |= RCC_CFGR_PPRE1_DIV8 << RCC_CFGR_PPRE1_Pos; break;
        case 16 : RCC->CFGR |= RCC_CFGR_PPRE1_DIV16 << RCC_CFGR_PPRE1_Pos; break;
    }

    // Clear then set peripheral bus 2 clock prescaler
    __CLEAR (RCC->CFGR, RCC_CFGR_PPRE2_0);
    __CLEAR (RCC->CFGR, RCC_CFGR_PPRE2_1);
    __CLEAR (RCC->CFGR, RCC_CFGR_PPRE2_2);
    switch(apb2)
    {
        case 1 : RCC->CFGR |= RCC_CFGR_PPRE2_DIV1 << RCC_CFGR_PPRE2_Pos; break;
        case 2 : RCC->CFGR |= RCC_CFGR_PPRE2_DIV2 << RCC_CFGR_PPRE2_Pos; break;
        case 4 : RCC->CFGR |= RCC_CFGR_PPRE2_DIV4 << RCC_CFGR_PPRE2_Pos; break;
        case 8 : RCC->CFGR |= RCC_CFGR_PPRE2_DIV8 << RCC_CFGR_PPRE2_Pos; break;
        case 16 : RCC->CFGR |= RCC_CFGR_PPRE2_DIV16 << RCC_CFGR_PPRE2_Pos; break;
    }
}

void CLKSetSysClockHSI(void)
{
    __SET(RCC->CR, RCC_CR_HSION);
    
    while(__READ(RCC->CR, RCC_CR_HSIRDY) == 0UL)
    {
        // Make sure the clock is ready
        // Wait if not
    }

    // Assign HSI clock as a main sys clock
    // Make sure the sys clock is ready
    __SET(RCC->CFGR, RCC_CFGR_SW_HSI);
    while(__READ(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
    {
        // Wait
    }

    // Update flash latency before increasing clock frequency
    // Flash natively run at 24mhz 
    CLKSetFlash(0UL);

    /// Setup sys clock and peripheral clocks pre scalers
    CLKSetPrescalers(0, 0, 0);

    // Update global sys clock frequency
    g_SysClkFreq = MAX_HSI_CLK_FREQ;
}

void CLKSetSysClockHSE(void)
{
}

void CLKSetSysClockPLL(void)
{
    const uint32_t flashLatency = MAX_CLK_FREQ / MAX_FLASH_CLK_FREQ;
    const uint32_t ppre1 = MAX_CLK_FREQ / MAX_APB1_CLK_FREQ;
    const uint32_t ppre2 = MAX_CLK_FREQ / MAX_APB2_CLK_FREQ;
    
    // Configure PLL clock system
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        // Some platforms give us more granual control over PLL frequency
        RCC->PLLCFGR =  0x24003010; // - Reset PLL register to default value
        __MODIFY(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_Msk, PLL_M << RCC_PLLCFGR_PLLM_Pos);
        __MODIFY(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, PLL_N << RCC_PLLCFGR_PLLN_Pos);
        __MODIFY(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk, PLL_P << RCC_PLLCFGR_PLLP_Pos);
        __MODIFY(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_Msk, PLL_Q << RCC_PLLCFGR_PLLQ_Pos);
    #else
        // Simple PLL scaling

        // Clear current PLL configuration
        __CLEAR(RCC->CFGR, RCC_CFGR_PLLSRC);
        __CLEAR(RCC->CFGR, RCC_CFGR_PLLMUL);

        // On this platform we can only powerup main PLL to 64Mhz
        __SET(RCC->CFGR, RCC_CFGR_PLLSRC_HSI_DIV2);
        __SET(RCC->CFGR, RCC_CFGR_PLLMUL16);
    #endif

    // Configure peripheral clocks & flash latency (They are dependant on the main clock source)
    CLKSetPrescalers(0, ppre1, ppre2);
    CLKSetFlash(flashLatency);

    // Enable PLL clock on and wait for lock
    __SET(RCC->CR, RCC_CR_PLLON);
    while(__READ(RCC->CR, RCC_CR_PLLRDY) == 0UL)
    {
        // NOP
    } 

    // Set the PLL as the main sys clock source & wait to make sure its ready
    RCC->CFGR |= RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos;
    while(__READ(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
        // NOP
    }

    // Update global sys clock frequency
    g_SysClkFreq = MAX_CLK_FREQ;
}

void CLKInitSysClock(const uint32_t clkSrc)
{
    switch(clkSrc)
    {
        case CLK_SRC_HSI: CLKSetSysClockHSI(); break;
        case CLK_SRC_HSE: CLKSetSysClockHSE(); break;
        case CLK_SRC_PLL: CLKSetSysClockPLL(); break;
    }
}