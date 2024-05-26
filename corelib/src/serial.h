#ifndef SERIAL_H
#define SERIAL_H

#include "uart.h"
#include "ccbuff.h"

typedef struct 
{
    uint8_t m_rxPort;
    uint8_t m_rxPin;
    uint8_t m_txPort;
    uint8_t m_txPin;
    uint8_t m_channel;
} SERIALSettings;

typedef struct 
{
    uint8_t m_channel;
} SERIALCom;

CCBBuff* SERIALGetBufferRx();
void SERIALRXEHandler(const uint8_t channel);

extern void SERIALInit(const SERIALSettings* settings, SERIALCom* outHandle);
extern void SERIALBroadcast(const SERIALCom* com, const char* msg);
extern bool SERIALRead(const SERIALCom* com, char* msg, const uint32_t maxSize);

#endif