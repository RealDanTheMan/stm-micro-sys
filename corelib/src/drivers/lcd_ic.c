#include "lcd_ic.h"
#include "../../corelib/src/sys.h"

LCDIContext* gLCDActiveCtx = NULL;
bool gLCDBacklightToggle = true;

LCDIContext* LCD_CreateContext()
{
    LCDIContext* ctx = (LCDIContext*)malloc(sizeof(LCDIContext));
    if(ctx != NULL)
    {
        memset(ctx, 0, sizeof(LCDIContext));
    }

    return ctx;
}

void LCD_DeleteContext(LCDIContext* ctx)
{
    if(ctx != NULL)
    {
        // If given context handle is currently bounded then unbind before deletion
        if(LCD_GetActiveContext() == ctx)
        {
            LCD_BindContext(NULL);
        }

        free(ctx);
    }
}

void LCD_BindContext(LCDIContext* ctx)
{
    gLCDActiveCtx = ctx;
}

LCDIContext* LCD_GetActiveContext()
{
    return gLCDActiveCtx;
}

void LCD_Init(const uint32_t sclPort, const uint32_t sclPin,
              const uint32_t sdaPort, const uint32_t sdaPin)
{
    // Initialise I2C Bus for communicating with LCD module
    I2CSettings i2cSettings;
    i2cSettings.m_sclPort = sclPort;
    i2cSettings.m_sclPin = sclPin;
    i2cSettings.m_sdaPort = sdaPort;
    i2cSettings.m_sdaPin = sdaPin;
    I2CEnable(&i2cSettings);

    LCD_GetActiveContext()->m_i2c.m_masterAddr = 0x0U;
    LCD_GetActiveContext()->m_i2c.m_slaveAddr = LCD_I2C_ADDR;
    
    LCD_ToggleBacklight(false);
    LCD_WriteInitSequence();
    LCD_SetDisplayMode(LCD_DISPLAY_MODE_ON);
    LCD_ClearScreen();
    LCD_ResetCursor();
    SYSWaitMs(500UL);
    LCD_ToggleBacklight(true);
}

void LCD_ToggleBacklight(const bool toggle)
{
    if(gLCDBacklightToggle != toggle)
    {
        gLCDBacklightToggle = toggle;
        LCD_SendCommand(0x0);       // Pulse none command to update backlight value
    }
}

void LCD_ClearScreen()
{
    LCD_SendCommand(0b00000001);
    SYSWaitMs(2UL); // Datasheet: Wait at least 1.52ms
}

void LCD_ResetCursor()
{
    LCD_SendCommand(0b00000010);
    SYSWaitMs(2UL); // Datasheet: Wait at least 1.52ms
}

void LCD_SetDisplayMode(const uint8_t mode)
{
    uint8_t val = 0U;
    LCD_SendCommand(val | mode);
}

void LCD_SetActiveLine(const uint8_t line)
{
    LCD_SendCommand(line);
}

void LCD_SendCommand(uint8_t value)
{
    // Register bit layout:  D7|D6|D5|D4|LED|EN|RW|RS
    uint8_t highBits = value & LCD_DATA_BITS;
    uint8_t lowBits = (value << 4) & LCD_DATA_BITS;

    LCD_WriteExpander(highBits);
    LCD_WriteExpander(lowBits);
}

void LCD_SendText(uint8_t value)
{
    // Register bit layout:  D7|D6|D5|D4|LED|EN|RW|RS
    uint8_t highBits = value & LCD_DATA_BITS;
    uint8_t lowBits = (value << 4) & LCD_DATA_BITS;

    LCD_WriteExpander(highBits | LCD_CTRL_BIT_RS_Msk);
    LCD_WriteExpander(lowBits | LCD_CTRL_BIT_RS_Msk);
}

void LCD_SendTextString(const char* text, const uint8_t delayms)
{
    for(int i=0; i < (strlen(text)); i++)
    {
        if(delayms != 0U)
        {
            SYSWaitMs(delayms);
        }

        LCD_SendText(text[i]);
    }
}

void LCD_WriteExpander(uint8_t _data)
{
    if(LCD_GetActiveContext())
    {
        const uint8_t val = gLCDBacklightToggle ? (_data | LCD_CTRL_BIT_BL_Msk) : _data;
        I2CContext* i2c = &LCD_GetActiveContext()->m_i2c;

        I2CDirectWriteBegin(i2c, 1U);
        I2CDirectWriteByte(i2c, val);
        I2CDirectWriteEnd(i2c);
        //SYSWaitMs(2UL);

        // Pulse enable to let LCD panel know to read data
        I2CDirectWriteBegin(i2c, 1U);
        I2CDirectWriteByte(i2c, val | LCD_CTRL_BIT_EN_Msk);
        I2CDirectWriteEnd(i2c);
        //SYSWaitMs(2UL);

        I2CDirectWriteBegin(i2c, 1U);
        I2CDirectWriteByte(i2c, val);
        I2CDirectWriteEnd(i2c);
        //SYSWaitMs(2UL);
    }
}

void LCD_WriteInitSequence()
{
    // Initialisation sequence as specified in datasheet page 45|46
    SYSWaitMs(20UL); // We have to wait at least 15ms after powerup before issuesing commands

    LCD_WriteExpander(0b00110000);
    SYSWaitMs(5UL); // Datasheet: Wait atleast 4.1ms
    LCD_WriteExpander(0b00110000);
    SYSWaitMs(5UL); // Datasheet: Wait atleast 4.1ms
    LCD_WriteExpander(0b00110000);
    SYSWaitMs(5UL); // Datasheet: Wait atleast 4.1ms

    // This is the only place where we can specify LCD function set
    // After the end of this method we cannot change addres mode | Line numbers or font size
    //LCD_WriteExpander(0b00100000);
    LCD_WriteExpander(0b00101100); // We are default to 4bit address | 2 lines | large font
    SYSWaitMs(5UL);
}