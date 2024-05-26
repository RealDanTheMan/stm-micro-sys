#ifndef I2C_H
#define I2C_H

#include "gpio.h"

typedef struct 
{
    uint32_t m_sclPort;
    uint32_t m_sclPin;
    uint32_t m_sdaPort;
    uint32_t m_sdaPin;
} I2CSettings;

typedef struct 
{
    uint8_t m_masterAddr;
    uint8_t m_slaveAddr;
} I2CContext;

extern I2C_TypeDef * I2CReg();
extern void I2CEnable(const I2CSettings* const settings);
extern void I2CDisable(const I2CSettings* const settings);
extern void I2CWriteSingle(const I2CContext* const context, const uint8_t data);
extern void I2CWrite(const I2CContext* const context, const uint8_t* data, const uint32_t len);

extern void I2CDirectWriteBegin(const I2CContext* const context, const uint8_t size);
extern void I2CDirectWriteEnd(const I2CContext* const context);
extern void I2CDirectWriteByte(const I2CContext* const context, const uint8_t data);

extern bool I2CGetBusOpen();
extern bool I2CGetWriteBufferReady();
extern bool I2CGetWriteComplete();

#endif