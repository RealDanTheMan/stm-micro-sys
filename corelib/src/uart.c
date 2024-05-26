#include "uart.h"
#include <math.h>

USART_TypeDef * USARTReg(const uint32_t channel)
{
    switch(channel)
    {
        case USART_CHANNEL1 : return USART1;
        case USART_CHANNEL2 : return USART2;
        case USART_CHANNEL3 : return USART3;
        case USART_CHANNEL4 : return USART4;
        case USART_CHANNEL5 : return USART5;
        case USART_CHANNEL6 : return USART6;
        case USART_CHANNEL7 : return USART7;
        case USART_CHANNEL8 : return USART8;
    }

    return NULL;
}

void USARTInitGPIO(const uint32_t channel, const GPIODesc *gpio)
{
    GPIOSetOutputSpeed(gpio, GPIO_SPEED_HIGH);
    GPIOSetOutputType(gpio, GPIO_OUT_TYPE_OPENDRAIN);

    // NOTE: Depending on the platform different alternative pin mode might be indicating USART mode
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        switch (channel)
        {
            // On F401RE alt function 7 specifies the USART on any pins that supports it for AHB1 & AHB2
            // Consult with data sheets to confirm which pins support USART functionality
            case USART_CHANNEL1: GPIOSetAltFn(gpio, GPIO_FN7); return;
            case USART_CHANNEL2: GPIOSetAltFn(gpio, GPIO_FN7); return;
            case USART_CHANNEL6: GPIOSetAltFn(gpio, GPIO_FN8); return;
        }
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        // On F303K8 alt function 7 specifies the USART on any pins that supports it
        // Consult with data sheets to confirm which pins support USART functionality
        switch (channel)
        {
            case USART_CHANNEL1: GPIOSetAltFn(gpio, GPIO_FN7); return;
            case USART_CHANNEL2: GPIOSetAltFn(gpio, GPIO_FN7); return;
            case USART_CHANNEL3: GPIOSetAltFn(gpio, GPIO_FN7); return;
        }
    #endif
}

void USARTEnableClock(const uint32_t channel)
{
    // Depending on the platform usart controller might use different clock bus for different channels
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        switch (channel)
        {
            case USART_CHANNEL1: __SET(RCC->APB2ENR, RCC_APB2ENR_USART1EN); return;
            case USART_CHANNEL2: __SET(RCC->APB1ENR, RCC_APB1ENR_USART2EN); return;
            case USART_CHANNEL6: __SET(RCC->APB2ENR, RCC_APB2ENR_USART6EN); return;
        }
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        switch (channel)
        {
            case USART_CHANNEL1: __SET(RCC->APB2ENR, RCC_APB2ENR_USART1EN); return;
            case USART_CHANNEL2: __SET(RCC->APB1ENR, RCC_APB1ENR_USART2EN); return;
            case USART_CHANNEL3: __SET(RCC->APB1ENR, RCC_APB1ENR_USART3EN); return;
        }
    #endif
}

void USARTClkDisable(const uint32_t channel)
{
    // Depending on the platform usart controller might use different clock bus for different channels
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        switch (channel)
        {
            case USART_CHANNEL1: __CLEAR(RCC->APB2ENR, RCC_APB2ENR_USART1EN); return;
            case USART_CHANNEL2: __CLEAR(RCC->APB1ENR, RCC_APB1ENR_USART2EN); return;
            case USART_CHANNEL6: __CLEAR(RCC->APB2ENR, RCC_APB2ENR_USART6EN); return;
        }
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        switch (channel)
        {
            case USART_CHANNEL1: __CLEAR(RCC->APB2ENR, RCC_APB2ENR_USART1EN); return;
            case USART_CHANNEL2: __CLEAR(RCC->APB1ENR, RCC_APB2ENR_USART1EN); return;
            case USART_CHANNEL3: __CLEAR(RCC->APB1ENR, RCC_APB1ENR_USART3EN); return;
        }
    #endif
}

void USARTEnableIRQ(const uint32_t channel)
{
    USART_TypeDef *reg = USARTReg(channel);
    __SET(reg->CR1, USART_CR1_RXNEIE);

    #if PLATFORMID == PLAT_NUCLEO32F401RE
        switch (channel)
        {
            case USART_CHANNEL1: NVIC_EnableIRQ(USART1_IRQn); break;
            case USART_CHANNEL2: NVIC_EnableIRQ(USART2_IRQn); break;
            case USART_CHANNEL3: NVIC_EnableIRQ(USART3_IRQn); break;
            case USART_CHANNEL4: NVIC_EnableIRQ(USART4_IRQn); break;
            case USART_CHANNEL5: NVIC_EnableIRQ(USART5_IRQn); break;
            case USART_CHANNEL6: NVIC_EnableIRQ(USART6_IRQn); break;
        }
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        switch (channel)
        {
            case USART_CHANNEL1: NVIC_EnableIRQ(USART1_IRQn); break;
            case USART_CHANNEL2: NVIC_EnableIRQ(USART2_IRQn); break;
            case USART_CHANNEL3: NVIC_EnableIRQ(USART3_IRQn); break;
        }
    #endif
}

void USARTSetRate(const uint32_t channel, const uint32_t baud)
{
    // USART Speed is dependant on peripheral bus clock speed
    // Different platforms have different bus speeds & different platform uses different buses for each USAR channels

    USART_TypeDef *reg = USARTReg(channel);
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        double rate = (float)(g_SysClkFreq) / (float)baud;
        double mantissa = rate;
        double frac = modf(rate, &mantissa);

        __MODIFY(reg->BRR, USART_BRR_DIV_Fraction, (uint32_t)frac << USART_BRR_DIV_Fraction_Pos);
        __MODIFY(reg->BRR, USART_BRR_DIV_Mantissa, (uint32_t)mantissa << USART_BRR_DIV_Mantissa_Pos);
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        reg->BRR = g_SysClkFreq / baud;
    #endif
}

void USARTInit(const uint32_t channel, const GPIODesc *rx, const GPIODesc *tx)
{
    // Set GPIO usage to USART mode
    // We can use any IO pin to transmit serial inforamtion via IO multiplexing
    USARTInitGPIO(channel, rx);
    USARTInitGPIO(channel, tx);

    // Enable usart clock
    USARTEnableClock(channel);

    // Select the desired baud rate using the USART_BRR register.
    USARTSetRate(channel, 9600UL);

    // One Stop bit 
    USART_TypeDef *reg = USARTReg(channel);
    __CLEAR(reg->CR2, USART_CR2_STOP_0);
    __CLEAR(reg->CR2, USART_CR2_STOP_1);

    // Word length is 8 bit
    __CLEAR(reg->CR1, USART_CR1_M0);
    __CLEAR(reg->CR1, USART_CR1_M1);

    // Keep at reset when using async comms
    // __CLEAR(reg->CR2, USART_CR2_CPHA);
    // __CLEAR(reg->CR2, USART_CR2_LBCL);

    // Enable usart controllers
    __SET(reg->CR1, USART_CR1_RE);
    __SET(reg->CR1, USART_CR1_TE);
    __SET(reg->CR1, USART_CR1_UE);
}

void USARTWrite(const uint32_t channel, const char* msg, const uint32_t len)
{
    USART_TypeDef *reg = USARTReg(channel);
    for(uint32_t i=0; i < len; i++)
    {
        #if PLATFORMID == PLAT_NUCLEO32F401RE
            reg->DR = msg[i];
            while(!(reg->SR & (1 << 6)));
        #elif PLATFORMID == PLAT_NUCLEO32F303K8
            reg->TDR = msg[i];
            while (!(reg->ISR & USART_ISR_TC)) {}
        #endif
    }
}

void USARTWriteSingle(const uint32_t channel, char msg)
{
    USART_TypeDef *reg = USARTReg(channel);
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        reg->DR = msg;
        while (!(reg->SR & USART_ISR_TC)) {}
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        reg->TDR = msg;
        while (!(reg->ISR & USART_ISR_TC)) {}
    #endif
}

bool USARTReadSingle(const uint32_t channel, char* msg)
{
    if(msg == NULL)
    {
        return false;
    }

    // Read the data
    if(USARTGetReadReady(channel))
    {
        USART_TypeDef *reg = USARTReg(channel);
        *msg = (char)reg->RDR;

        return true;
    }

    return false;
}

bool USARTRead(const uint32_t channel, char* msg, const uint32_t maxSize)
{
    bool readok = false;
    if(msg)
    {
        uint32_t count = 0;
        char val = '?';
        while(count < maxSize && USARTReadSingle(channel, &val))
        {
            msg[count] = val;
            count++;

            while(USARTGetBusy(channel)){}

            if(!readok)
            {
                readok = true;
            }
        }

        if(readok)
        {
            USARTWrite(channel, msg, maxSize);
        }
    }

    return readok;
}

bool USARTGetReadReady(const uint32_t channel)
{
    USART_TypeDef *reg = USARTReg(channel);
    return reg->ISR & USART_ISR_RXNE;
}

bool USARTGetBusy(const uint32_t channel)
{
    USART_TypeDef *reg = USARTReg(channel);
    return reg->ISR & USART_ISR_BUSY;
}