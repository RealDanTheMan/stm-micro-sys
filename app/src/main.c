#include "../../corelib/src/drivers/lcd_ic.h"
#include "../../corelib/src/drivers/dht22.h"
#include "../../corelib/src/gpio.h"
#include "../../corelib/src/serial.h"
#include "../../corelib/src/i2c.h"
#include "../../corelib/src/sys.h"

#include <stdio.h>

int main()
{
    // Configure onboard LED IO
    GPIODesc LEDPIN;
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        GPIOEnable(GPIO_PORTA, GPIO_PIN5, GPIO_MODE_WRITE, &LEDPIN);
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        GPIOEnable(GPIO_PORTB, GPIO_PIN3, GPIO_MODE_WRITE, &LEDPIN);
    #endif

    // USART receiver and transmiter pins
    //uint32_t USART_CHANNEL = USART_CHANNEL2;

    // Configure serial communication
    SERIALCom com;
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        SERIALSettings ss = 
        {
            GPIO_PORTC, 
            GPIO_PIN8, 
            GPIO_PORTC, 
            GPIO_PIN6, 
            USART_CHANNEL6
        };
        SERIALnit(*ss, &com);
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        SERIALSettings ss = 
        {
            GPIO_PORTA, 
            GPIO_PIN3, 
            GPIO_PORTA, 
            GPIO_PIN2, 
            USART_CHANNEL2
        }; 
        SERIALInit(&ss, &com);
    #endif
    
    SERIALBroadcast(&com, "Serial com enabled!\n");

    LCDIContext* lcd = LCD_CreateContext();
    LCD_BindContext(lcd);
    LCD_Init(GPIO_PORTB, GPIO_PIN6, GPIO_PORTB, GPIO_PIN7);
    
    LCD_SendTextString("- LCD IS READY -", 0U);
    LCD_SetActiveLine(LCD_LINE_SECOND);
    LCD_SendTextString(" * ---------- *", 0U);

    // Initialize temperature sensor (DHT22)
    DHT22Context sensor;
    if(DHT22Init(GPIO_PORTB, GPIO_PIN4, &sensor, true) == DHT22_STATUS_OK)
    {
        LCD_ClearScreen();
        LCD_ResetCursor();
        LCD_SendTextString("Sensor: ON", 0U);
        SYSWaitMs(3000UL);
    }

    while(1)
    {
        //GPIOWrite(&LEDPIN, GPIO_VAL_HIGH);
        //SYSWaitMs(15UL);
        //GPIOWrite(&LEDPIN, GPIO_VAL_LOW);
        //SYSWaitMs(150UL);
        //GPIOWrite(&LEDPIN, GPIO_VAL_HIGH);
        //SYSWaitMs(15UL);
        //GPIOWrite(&LEDPIN, GPIO_VAL_LOW);
        //SYSWaitMs(3000UL);

        //const uint32_t size=16;
        //char rd[size] = {};
        //memset(rd, '\0', size);
        //if(SERIALRead(&com, rd, size))
        //{
        //    LCD_ClearScreen();
        //    LCD_ResetCursor();
        //    LCD_SendTextString(rd, 0U);
        //}

        SYSWaitMs(3000UL);
        GPIOWrite(&LEDPIN, GPIO_VAL_HIGH);
        ProcessSensorData(&sensor, &com);
        SYSWaitMs(1000UL);
        GPIOWrite(&LEDPIN, GPIO_VAL_LOW);

        //SERIALBroadcast(&com, "Listening...\n");
    }
    
    return 0;
}

void ProcessSensorData(DHT22Context* sensor, SERIALCom* com)
{
    float humidity = 0;
    float temperature = 0;
    uint8_t sensorstat = DHT22GetReading(sensor, &humidity, &temperature);
    if(DHT22StatusOK(sensorstat))
    {
        LCD_ClearScreen();
        LCD_ResetCursor();

        char txt[32];
        //sprintf(txt, "Humidity: %.1f%%\n", humidity);
        sprintf(txt, "Humidity: %d%%", (int)humidity);
        LCD_SendTextString(txt, 0U);
        SERIALBroadcast(com, txt);
        //sprintf(txt, "Temperature: %.1fC\n", temperature);
        sprintf(txt, "Temperature: %dC", (int)temperature);
        LCD_SetActiveLine(LCD_LINE_SECOND);
        LCD_SendTextString(txt, 0U);
        SERIALBroadcast(com, txt);
    }
    else
    {
        LCD_ClearScreen();
        LCD_ResetCursor();
        switch(sensorstat)
        {
            case DHT22_STATUS_BADREQ : LCD_SendTextString("Error: BADREQ", 0U); break;
            case DHT22_STATUS_BADREAD : LCD_SendTextString("Error: BADREAD", 0U); break;
            case DHT22_STATUS_BADDATA : LCD_SendTextString("Error: BADDATA", 0U); break;
            case DHT22_STATUS_BADCHECKSUM : LCD_SendTextString("Error: BADCHECK", 0U); break;
        }
    }
}