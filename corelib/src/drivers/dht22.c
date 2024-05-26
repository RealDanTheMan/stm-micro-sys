#include "dht22.h"

bool DHT22StatusOK(const uint8_t status)
{
    return status == DHT22_STATUS_OK;
}

bool DHT22StatusFAIL(const uint8_t status)
{
    return status != DHT22_STATUS_OK;
}

uint8_t DHT22Init(const uint32_t port, const uint32_t pin, DHT22Context* ctx, const bool prewarm)
{
    // Datasheet: Sensor chip has to first clear unstable stage before we can use it.
    // (Optional) You can skip this if the chip has been supplied voltage for at least
    // a second before initializing.
    if(prewarm)
    {
        SYSWaitMs(1000UL);
    }

    memset(ctx, 0, sizeof(DHT22Context));
    GPIOEnable(port, pin, GPIO_MODE_WRITE, &ctx->m_gpio);
    GPIOWrite(&ctx->m_gpio, GPIO_VAL_LOW);
    SYSWaitUs(10000UL); // Datasheet: wait at least 18 milliseconds
    GPIOSetMode(&ctx->m_gpio, GPIO_MODE_READ);

    uint32_t pinread = 1UL;
    SYSWaitUs(40UL);
    GPIORead(&ctx->m_gpio, &pinread);
    if(pinread == 0)
    {
        SYSWaitUs(80UL);
        GPIORead(&ctx->m_gpio, &pinread);
        if(pinread == 1)
        {
            // Datasheet: At this point the sensor indicates that its ready to work
            // But we have to wait for it to go low on the data pin before can continue.
            while(pinread)
            {
                GPIORead(&ctx->m_gpio, &pinread);
            }

            // Once sensor releases the pin / goes low we then set the data pin high to 
            // indicate idle state.
            GPIOSetMode(&ctx->m_gpio, GPIO_MODE_WRITE);
            GPIOWrite(&ctx->m_gpio, GPIO_VAL_HIGH);
            SYSWaitUs(40UL);

            return DHT22_STATUS_OK;
        }
    }

    return DHT22_STATUS_BADREQ;
}

uint8_t DHT22DataStreamBegin(const DHT22Context* ctx)
{
    // Note: The moment function returns OK sensor chip will start sending data!
    // Datasheet: Data request handshake is as follows:
    // 1.) MCU sends HIGH for at least 10ms and waits 40us
    // 2.) SENSOR sends LOW and waits for 80us
    // 3.) SENSOR sends HIGH and waits for 80us (request acknowledged!)
    // 4.) Sensor sends LOW  (data transmission begins!)

    GPIOWrite(&ctx->m_gpio, GPIO_VAL_LOW);
    SYSWaitUs(18000UL);
    GPIOSetMode(&ctx->m_gpio, GPIO_MODE_READ);
    SYSWaitUs(40UL);

    uint32_t pinread = GPIO_VAL_HIGH;

    GPIORead(&ctx->m_gpio, &pinread);
    if(pinread != GPIO_VAL_LOW)
    {
        return DHT22_STATUS_BADREQ;
    }

    SYSWaitUs(85UL);
    GPIORead(&ctx->m_gpio, &pinread);
    if(pinread != GPIO_VAL_HIGH)
    {
        return DHT22_STATUS_BADREQ;
    }

    // Wait for the pin to go LOW to indicate beginning of data transmission
    while(pinread == GPIO_VAL_HIGH) { GPIORead(&ctx->m_gpio, &pinread); }
    return DHT22_STATUS_OK;
}

uint8_t DHT22ReadDataPacket(const DHT22Context* ctx, DHT22DataBuffer* buff)
{
    // DHT22 sensor data packet consists of 40 bits
    // 16 bit RH (relative humidity)
    // 16 bit T (temperature)
    // 8 bit checksum to validate the readings
    // Note: DHT22 sends data in MSb format

    // Make sure we start with a data pin set to LOW indicating transmission start
    uint32_t pinread = GPIO_VAL_HIGH;
    GPIORead(&ctx->m_gpio, &pinread);
    if(pinread == GPIO_VAL_HIGH)
    {
        // If the pin doesnt read LOW at this point the sensor is not sending data!
        return DHT22_STATUS_BADREAD;
    }

    uint8_t bytes[5] = {0};

    // DHT22 send all the data in reverse
    for(int i=0; i<40; i++)
    {
        const uint8_t bitIdx = (i % 8);
        const uint8_t byteIdx = i / 8;
        
        // Wait for the sensor to set data pin HIGH & mark the time
        while(pinread == GPIO_VAL_LOW) { GPIORead(&ctx->m_gpio, &pinread); }
        uint32_t time0 = SYSGetTimeUs();

        // Wait for the sensor to set data pin LOW & mark the time again
        while(pinread == GPIO_VAL_HIGH) { GPIORead(&ctx->m_gpio, &pinread); }
        uint32_t time1 = SYSGetTimeUs();

        const uint32_t td = time1 - time0;
        if(td <= 30UL)
        {
            // Sensor sent bit 0
            bytes[byteIdx] |= (0U << bitIdx);
            continue;
        }

        if(td >= 60UL)
        {
            // Sensor sent bit 1
            bytes[byteIdx] |= (1U << bitIdx);
            continue;
        }

        return DHT22_STATUS_BADDATA;
    }

    // Reverse bit order due to transmission using MSb
    buff->m_hHum = ReverseByte(bytes[0]);                // High humidity bits
    buff->m_lHum = ReverseByte(bytes[1]);                // Low gumidity bits
    buff->m_hTemp = ReverseByte(bytes[2]);               // High temperature bits
    buff->m_lTemp = ReverseByte(bytes[3]);               // Low temperature bits
    buff->m_check = ReverseByte(bytes[4]);               // Checksum bits

    return DHT22_STATUS_OK;
}


uint8_t DHT22GetReading(DHT22Context* ctx, float* outHumidity, float* outTemperature)
{
    *outHumidity = 0U;
    *outTemperature = 0U;

    DHT22DataBuffer sensorData;
    memset(&sensorData, 0, 5);

    uint8_t status = DHT22DataStreamBegin(ctx);
    if(DHT22StatusFAIL(status))
    {
        DHT22DataStreamEnd(ctx);
        return status;
    }

    status = DHT22ReadDataPacket(ctx, &sensorData);
    if(DHT22StatusFAIL(status))
    {
        DHT22DataStreamEnd(ctx);
        return status;
    }

    // TODO: Validate data integrity via checksum!

    // Convert to decimal values with
    const uint16_t humidity = (sensorData.m_hHum << 8) | sensorData.m_lHum;
    const uint16_t temperature = (sensorData.m_hTemp << 8) | sensorData.m_lTemp;

    // Sensor is using one decimal precision when sending data
    *outHumidity = ((float)humidity) * 0.1f;
    *outTemperature = ((float)temperature) * 0.1f;
    SYSWaitMs(5UL);
    DHT22DataStreamEnd(ctx);

    return DHT22_STATUS_OK;
}

uint8_t DHT22DataStreamEnd(const DHT22Context* ctx)
{
    // Set GPIO states back to default state
    GPIOSetMode(&ctx->m_gpio, GPIO_MODE_WRITE);
    GPIOWrite(&ctx->m_gpio, GPIO_VAL_HIGH);
    SYSWaitMs(16UL);

    return DHT22_STATUS_OK;
}