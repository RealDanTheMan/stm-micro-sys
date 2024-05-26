#ifndef DHT22_H
#define DHT22_H

#include "../sys.h"
#include "../gpio.h"

#define DHT22_STATUS_OK                 (0U)
#define DHT22_STATUS_BADREQ             (1U)
#define DHT22_STATUS_BADREAD            (2U)
#define DHT22_STATUS_BADDATA            (3U)
#define DHT22_STATUS_BADCHECKSUM        (4U)

typedef struct 
{
    GPIODesc m_gpio;

} DHT22Context;

typedef struct 
{
    uint8_t m_hHum;
    uint8_t m_lHum;
    uint8_t m_hTemp;
    uint8_t m_lTemp;
    uint8_t m_check;

} DHT22DataBuffer;

extern bool DHT22StatusOK(const uint8_t status);
extern bool DHT22StatusFAIL(const uint8_t status);
extern uint8_t DHT22Init(const uint32_t port, const uint32_t pin, DHT22Context* ctx, const bool prewarm);
extern uint8_t DHT22GetReading(DHT22Context* ctx, float* outHumidity, float* outTemperature);

uint8_t DHT22DataStreamBegin(const DHT22Context* ctx);
uint8_t DHT22DataStreamEnd(const DHT22Context* ctx);
uint8_t DHT22ReadDataPacket(const DHT22Context* ctx, DHT22DataBuffer* buff);

#endif