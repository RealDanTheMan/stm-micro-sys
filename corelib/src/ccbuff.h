#ifndef CCBUFF_H
#define CCBUFF_H

#include "base.h"

typedef struct 
{
    uint32_t m_size;
    uint32_t m_ridx;
    uint32_t m_widx;
    uint8_t* m_data;
} CCBBuff;

extern bool CCBEmpty(const CCBBuff* buffer);
extern bool CCBReadEnd(const CCBBuff* buffer);
extern void CCBInit(CCBBuff* buffer, const uint32_t size);
extern void CCBFree(CCBBuff* buffer);
extern void CCBRead(CCBBuff* buffer, char* outData);
extern void CCBWrite(CCBBuff* buffer, const char data);

#endif