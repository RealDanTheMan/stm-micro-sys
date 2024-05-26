#ifndef CLOCK_H
#define CLOCK_H

#include "platform.h"

#define CLK_SRC_HSI (0UL)
#define CLK_SRC_HSE (1UL)
#define CLK_SRC_PLL (2UL)

extern uint32_t g_SysClkFreq;
extern uint32_t g_SysClkTk;

extern void CLKResetClocks(void);
extern void CLKSetFlash(const uint32_t waitStates);
extern void CLKSetPrescalers(const uint32_t ahb, const uint32_t apb1, const uint32_t apb2);
extern void CLKSetSysClockHSI(void);
extern void CLKSetSysClockHSE(void);
extern void CLKSetSysClockPLL(void);
extern void CLKInitSysClock(const uint32_t clkSrc);

extern void CLKEnable();

#endif