#ifndef PLATFORM_H
#define PLATFORM_H

#include "base.h"
#include "platformdefs.h"

#if PLATFORMID == PLAT_UNKNOWN
#error Unknown platform identifier!
#endif

#if PLATFORMID == PLAT_NUCLEO32F401RE // STM32 F401RE microcontroller specs
    // Platfrom defines provided by the manufacturer
    #include "stm32f4xx.h"
    #include "stm32f401xe.h"

    #define MAX_HSI_CLK_FREQ        16000000UL
    #define MAX_HSE_CLK_FREQ        16000000UL
    #define MAX_CLK_FREQ            84000000UL
    #define MAX_AHB_CLK_FREQ        MAX_CLK_FREQ
    #define MAX_APB1_CLK_FREQ       MAX_AHB_CLK_FREQ / 2UL
    #define MAX_APB2_CLK_FREQ       MAX_AHB_CLK_FREQ
    #define MAX_FLASH_CLK_FREQ      24000000UL

    // PLL Coeficients - This platform gives us granual control over clock frequency
    // NOTE: These should be calculated at runtime depending on platform clock specs
    // Output clock formula ((InputFreq / PLL_M) * PLL_N) / PLL_P
    #define PLL_M  MAX_HSI_CLK_FREQ / 1000000UL
    #define PLL_N  336U
    #define PLL_P  1U //(DIV 4)
    #define PLL_Q  8U

    // Bus speed prescalers
    // AHB - main system bus | APB1 low speed peripheral bus | APB2 high speed peripheral bus
    #define AHB_SCALE 0U
    #define APB1_SCALE 2U
    #define APB2_SCALE 0U
#endif

#if PLATFORMID == PLAT_NUCLEO32F303K8 // STM32 F303K8 microcontroller specs
    // Platfrom defines provided by the manufacturer
    #include "stm32f3xx.h"
    #include "stm32f303x8.h"

    //TODO: Explain why our max clock frequencies differ from the one in the data sheet!

    #define MAX_HSI_CLK_FREQ        8000000UL
    #define MAX_HSE_CLK_FREQ        8000000UL
    #define MAX_CLK_FREQ            64000000UL
    #define MAX_AHB_CLK_FREQ        MAX_CLK_FREQ
    #define MAX_APB1_CLK_FREQ       MAX_AHB_CLK_FREQ / 2UL
    #define MAX_APB2_CLK_FREQ       MAX_AHB_CLK_FREQ
    #define MAX_FLASH_CLK_FREQ      24000000UL

    // PLL Coeficients - Simple PLL frequency scaling
    //#define PLL_MUL MAX_CLK_FREQ / MAX_HSI_CLK_FREQ

    // Bus speed prescalers
    // AHB - main system bus | APB1 low speed peripheral bus | APB2 high speed peripheral bus
    #define AHB_SCALE 1U
    #define APB1_SCALE 2U
    #define APB2_SCALE 2U
#endif

#endif