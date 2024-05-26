#include "i2c.h"
#include "sys.h"

I2C_TypeDef* I2CReg()
{
    return I2C1;
}

void I2CEnable(const I2CSettings* const settings)
{
    // Get harware register set for I2C bus
    I2C_TypeDef* reg = I2CReg();

    // First we need to configure the IO pins to the I2C spec
    GPIODesc SCL_PIN; // Clock pin
    GPIODesc SDA_PIN; // Data pin
    GPIOEnable(settings->m_sclPort, settings->m_sclPin, GPIO_MODE_ALT, &SCL_PIN);
    GPIOEnable(settings->m_sdaPort, settings->m_sdaPin, GPIO_MODE_ALT, &SDA_PIN);

    GPIOSetOutputSpeed(&SCL_PIN, GPIO_SPEED_HIGH);
    GPIOSetOutputSpeed(&SDA_PIN, GPIO_SPEED_HIGH);
    GPIOSetOutputType(&SCL_PIN, GPIO_OUT_TYPE_OPENDRAIN);
    GPIOSetOutputType(&SDA_PIN, GPIO_OUT_TYPE_OPENDRAIN);
    GPIOSetAltFn(&SCL_PIN, GPIO_FN4);   // Alternative pin function for controlling I2C
    GPIOSetAltFn(&SDA_PIN, GPIO_FN4);   // Alternative pin function for controlling I2C

    // DATASHEET: Before enabling the I2C peripheral, configure and enable its clock through the clock
    // controller, and initialize its control registers.
    __SET(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
    __CLEAR(reg->CR1, I2C_CR1_PE);

    // DATASHEET: Before enabling the I2C peripheral by setting the PE bit of the I2C_CR1 register, the user
    // must configure the analog and/or digital noise filters, as required.


    // DATASHEET: To ensure correct data hold and setup times, the corresponding timings must be configured
    // through the PRESC[3:0], SCLDEL[3:0], and SDADEL[3:0] bitfields of the I2C_TIMINGR register.
    const uint32_t freq = 100000U; // 100Hz - Target I2C clock speed
    const uint32_t pclk = g_SysClkFreq;
    const uint32_t SCLDEL = 0U; // SCL delay - Not sure if we need any delay
    const uint32_t SDADEL = 0U; // SDA delay - Not sure if we need any delay
    uint32_t timing = pclk / (freq * (SCLDEL + SDADEL + 1));

    reg->TIMINGR = 0x0;
    __SET(reg->TIMINGR, timing << I2C_TIMINGR_PRESC_Pos);
    __SET(reg->TIMINGR, SCLDEL << I2C_TIMINGR_SDADEL_Pos);
    __SET(reg->TIMINGR, SDADEL << I2C_TIMINGR_SCLDEL_Pos);

    // DATASHEET: Before enabling the peripheral, the I2C master clock must be configured, by setting the
    // SCLH and SCLL bits in the I2C_TIMINGR register.
    const float period = 1.0f / (float)freq;
    const uint32_t scll = (uint32_t)(pclk * (period / 2)) - 1;
    const uint32_t sclh = (uint32_t)(pclk * (period / 2)) + 1;
    __SET(reg->TIMINGR, scll << I2C_TIMINGR_SCLL_Pos);
    __SET(reg->TIMINGR, sclh << I2C_TIMINGR_SCLH_Pos);

    // Disable clock streaching
    __CLEAR(reg->CR1, I2C_CR1_NOSTRETCH);

    // TODO: Do we need to hint I2C speed mode ?

    // Enable I2C peripheral
    __SET(reg->CR1, I2C_CR1_PE);
}

void i2CDisable(const I2CSettings* const settings)
{

}

void I2CWriteSingle(const I2CContext* const context, const uint8_t data)
{
    // Get harware register set for I2C bus
    I2C_TypeDef* reg = I2CReg();

    // Wait if I2C interface is busy
    while(reg->ISR & I2C_ISR_BUSY);

    reg->CR2 |= (context->m_slaveAddr << I2C_CR2_SADD_Pos); // Set slave address
    __CLEAR(reg->CR2, I2C_CR2_RD_WRN); // - Write mode
    __CLEAR(reg->CR2, I2C_CR2_ADD10); // 8 bit address mode
    reg->CR2 |= (1U << I2C_CR2_NBYTES_Pos); // Set number of bytes to write (1 byte)
    
    reg->CR2 |= I2C_CR2_START;            // Generate start condition

    while (!(reg->ISR & I2C_ISR_TXIS)); // Wait until TX buffer is empty
    reg->TXDR = data;
    while (!(reg->ISR & I2C_ISR_TC));
    reg->CR2 |= I2C_CR2_STOP;
}

void I2CWrite(const I2CContext* const context, const uint8_t* data, const uint32_t len)
{
    // Get harware register set for I2C bus
    I2C_TypeDef* reg = I2CReg();

    // Wait if I2C interface is busy
    while(reg->ISR & I2C_ISR_BUSY);

    reg->CR2 |= (context->m_slaveAddr << I2C_CR2_SADD_Pos); // Set slave address
    __CLEAR(reg->CR2, I2C_CR2_RD_WRN); // - Write mode
    __CLEAR(reg->CR2, I2C_CR2_ADD10); // 8 bit address mode
    reg->CR2 |= (1U << I2C_CR2_NBYTES_Pos); // Set number of bytes to write (1 byte)
    
    reg->CR2 |= I2C_CR2_START;            // Generate start condition

    // Go through each data byte and send it down the I2C pipe
    for(uint32_t i=0; i < len; i++)
    {
        while (!(reg->ISR & I2C_ISR_TXIS)); // Wait until TX buffer is empty
        reg->TXDR = data[i];
        while (!(reg->ISR & I2C_ISR_TC));  // Wait for data transmission to finish
    }
    
    reg->CR2 |= I2C_CR2_STOP;
}

void I2CDirectWriteBegin(const I2CContext* const context, const uint8_t size)
{
    // Get harware register set for I2C bus
    I2C_TypeDef* reg = I2CReg();

    // Wait if I2C interface is busy
    while(I2CGetBusOpen());

    // Set slave address and wait until acknowledged
    __CLEAR(reg->CR2, I2C_CR2_RD_WRN);                         // - Write mode
    __CLEAR(reg->CR2, I2C_CR2_ADD10);                          // 8 bit address mode

    // Set number of bytes per write request (bytes)
    reg->CR2 |= (size << I2C_CR2_NBYTES_Pos);

    // Set target slave address & generate start command
    reg->CR2 |= (context->m_slaveAddr << I2C_CR2_SADD_Pos);
    reg->CR2 |= I2C_CR2_START;
}

void I2CDirectWriteEnd(const I2CContext* const context)
{
    I2CReg()->CR2 |= I2C_CR2_STOP;
}


void I2CDirectWriteByte(const I2CContext* const context, const uint8_t data)
{
    //while (!I2CGetWriteBufferReady());        // Wait until TX buffer is empty
    I2CReg()->TXDR = data;                      // Sata data bits
    while (!I2CGetWriteComplete());             // Wait for data transmission to finish
}

bool I2CGetBusOpen()
{
    // Datasheet: This flag indicates that a communication is in progress on the bus. 
    // It is set by hardware when a START condition is detected, and cleared by 
    // hardware when a STOP condition is detected, or when PE = 0.
    return (bool)(I2CReg()->ISR & I2C_ISR_BUSY);
}

bool I2CGetWriteBufferReady()
{
    // Datasheet: This bit is set by hardware when the I2C_TXDR register is empty and the
    // data to be transmitted must be written in the I2C_TXDR register. It is cleared when
    // the next data to be sent is written in the I2C_TXDR register. 
    // This bit can be written to 1 by software only when NOSTRETCH = 1
    // To generate a TXIS event (interrupt if TXIE = 1 or DMA request if TXDMAEN = 1).
    return (bool)(I2CReg()->ISR & I2C_ISR_TXIS);
}

bool I2CGetWriteComplete()
{
    // Datasheet: This flag is set by hardware when RELOAD = 0, AUTOEND = 0 and NBYTES data
    // have been transferred. It is cleared by software when START bit or STOP bit is set.
    return (bool)(I2CReg()->ISR & I2C_ISR_TC);
}