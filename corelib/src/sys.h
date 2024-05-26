#ifndef SYS_H
#define SYS_H

#include "clock.h"

extern void SYSConfigureTick(uint32_t interval);
extern void SYSWait(const uint32_t tks);
extern void SYSWaitUs(const uint32_t us);
extern void SYSWaitMs(const uint32_t ms);

extern uint32_t SYSGetTimeTk();
extern uint32_t SYSGetTimeUs();
extern uint32_t SYSGetTimeMs();


#endif