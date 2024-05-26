#include "serial.h"

static CCBBuff gSERIALBufferRx = {0, 0, 0, NULL};

#if PLATFORMID == PLAT_NUCLEO32F401RE
    void IRQ_USART1_Handler() {SERIALRXEHandler(USART_CHANNEL1); }
    void IRQ_USART2_Handler() {SERIALRXEHandler(USART_CHANNEL2); }
    void IRQ_USART3_Handler() {SERIALRXEHandler(USART_CHANNEL3); }
    void IRQ_USART4_Handler() {SERIALRXEHandler(USART_CHANNEL4); }
    void IRQ_USART5_Handler() {SERIALRXEHandler(USART_CHANNEL5); }
    void IRQ_USART6_Handler() {SERIALRXEHandler(USART_CHANNEL6); }

#elif PLATFORMID == PLAT_NUCLEO32F303K8
    void IRQ_USART1_Handler() {SERIALRXEHandler(USART_CHANNEL1); }
    void IRQ_USART2_Handler() {SERIALRXEHandler(USART_CHANNEL2); }
    void IRQ_USART3_Handler() {SERIALRXEHandler(USART_CHANNEL3); }

#endif

CCBBuff* SERIALGetBufferRx()
{
    return &gSERIALBufferRx;
}

void SERIALRXEHandler(const uint8_t channel)
{
    char data = '\0';
    if(USARTReadSingle(channel, &data))
    {
        CCBBuff* buffer = SERIALGetBufferRx();
        CCBWrite(buffer, data);
    }
}

void SERIALInit(const SERIALSettings* settings, SERIALCom* outHandle)
{
    GPIODesc rx;
    GPIODesc tx;
    GPIOEnable(settings->m_rxPort, settings->m_rxPin, GPIO_MODE_ALT, &rx);
    GPIOEnable(settings->m_txPort, settings->m_txPin, GPIO_MODE_ALT, &tx);

    CCBBuff* buffer = SERIALGetBufferRx();
    CCBInit(buffer, 256);

    USARTInit(settings->m_channel, &rx, &tx);
    USARTEnableIRQ(settings->m_channel);

    if(outHandle)
    {
        outHandle->m_channel = settings->m_channel;
    }
}

void SERIALBroadcast(const SERIALCom* com, const char* msg)
{
    if(!com)
    {
        return;
    }

    const uint32_t size = strlen(msg);
    USARTWrite(com->m_channel, msg, size);
}

bool SERIALRead(const SERIALCom* com, char* msg, const uint32_t maxSize)
{
    CCBBuff* buffer = SERIALGetBufferRx();
    
    // Abort if buffer is uninitialised
    if(CCBEmpty(buffer))
    {
        return false;
    }

    // Abort if nothing new to read
    if(CCBReadEnd(buffer))
    {
        return false;
    }

    memset(msg, 0, maxSize);
    uint32_t idx = 0;
    while(idx < maxSize && !CCBReadEnd(buffer))
    {
        char data = '\0';
        CCBRead(buffer, &data);
        msg[idx] = data;
        idx++;
    }

    return true;
}