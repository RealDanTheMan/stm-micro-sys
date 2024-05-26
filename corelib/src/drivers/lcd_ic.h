#ifndef LCD_IC_H
#define LCD_IC_H

#include "../gpio.h"
#include "../i2c.h"

// Note: LCD Model HITACHI HD44780U
// Note: LCD I2C Expander Model:  PCF8574* by NXP.

// The PCF8574* Remote 8-bit I/O expander for I2C-bus with interrupt is used to 
// control the HD44780 using only the highest 4 bits of its data bus (D4, D5, D6, D7),
// and uses the other 4 IO pins for EN, RS, R/W as well as controlling the status of 
// the backlight LED (which can be hardware disabled on some backpacks via means of a jumper).
// 
// Minimal Example:
//
// LCDIContext* lcd = LCD_CreateContext();
// LCD_BindContext(lcd);
// LCD_Init(GPIO_PORTB, GPIO_PIN6, GPIO_PORTB, GPIO_PIN7);// 
// LCD_SendTextString("- LCD IS READY -", 16, 0U);
// LCD_SetActiveLine(LCD_LINE_SECOND);
// LCD_SendTextString(" * ---------- *", 15, 0U);
//

// Default I2C address for the LCD I2C expander module
#define LCD_I2C_ADDR                        (0x4E)


// I2C Expander Uses 8 bit register
// First (lower) 4 of the register bits are control bits:
#define LCD_CTRL_BITS                   (0b00001111)
#define LCD_CTRL_BIT_RS_Pos             (0U)                            // Register select control bit
#define LCD_CTRL_BIT_RS_Msk             (1U << LCD_CTRL_BIT_RS_Pos)     // 0b00000001
#define LCD_CTRL_BIT_RW_Pos             (1U)                            // Register read|write control bit
#define LCD_CTRL_BIT_RW_Msk             (1U << LCD_CTRL_BIT_RW_Pos)     // 0b00000010
#define LCD_CTRL_BIT_EN_Pos             (2U)                            // Register enable control bit
#define LCD_CTRL_BIT_EN_Msk             (1U << LCD_CTRL_BIT_EN_Pos)     // 0b00000100
#define LCD_CTRL_BIT_BL_Pos             (3U)                            // Register backlight control bit
#define LCD_CTRL_BIT_BL_Msk             (1U << LCD_CTRL_BIT_BL_Pos)     // 0b00001000

// Second (higher) 4 of the register bits are data bits:
#define LCD_DATA_BITS                   (0b11110000)
#define LCD_DATA_BIT_D4_Pos             (4U)                            // Register data Bit 1
#define LCD_DATA_BIT_D4_Msk             (1U << LCD_DATA_BIT_D4_Pos)     // 0b00010000
#define LCD_DATA_BIT_D5_Pos             (5U)                            // Register data Bit 2
#define LCD_DATA_BIT_D5_Msk             (1U << LCD_DATA_BIT_D5_Pos)     // 0b00100000
#define LCD_DATA_BIT_D6_Pos             (6U)                            // Register data Bit 3
#define LCD_DATA_BIT_D6_Msk             (1U << LCD_DATA_BIT_D6_Pos)     // 0b01000000
#define LCD_DATA_BIT_D7_Pos             (7U)                            // Register data Bit 4
#define LCD_DATA_BIT_D7_Msk             (1U << LCD_DATA_BIT_D7_Pos)     // 0b10000000

#define LCD_DISPLAY_MODE_OFF            (0b00001000)
#define LCD_DISPLAY_MODE_ON             (0b00001100)
#define LCD_DISPLAY_MODE_CURSOR         (0b00001010)
#define LCD_DISPLAY_MODE_BLINK          (0b00001001)

#define LCD_LINE_FIRST                  (0b10000000)
#define LCD_LINE_SECOND                 (0b11000000)


typedef struct
{
    GPIODesc m_scl;
    GPIODesc m_sca;
    I2CContext m_i2c;
} LCDIContext;

extern LCDIContext* LCD_CreateContext();
extern void LCD_DeleteContext(LCDIContext* ctx);
extern void LCD_BindContext(LCDIContext* ctx);
extern LCDIContext* LCD_GetActiveContext();

extern void LCD_Init(const uint32_t sclPort, const uint32_t sclPin,
                     const uint32_t sdaPort, const uint32_t sdaPin);

extern void LCD_ToggleBacklight(const bool toggle);
extern void LCD_ClearScreen();
extern void LCD_ResetCursor();
extern void LCD_SetDisplayMode(const uint8_t mode);
extern void LCD_SetActiveLine(const uint8_t line);

extern void LCD_SendCommand(uint8_t value);
extern void LCD_SendText(uint8_t value);
extern void LCD_SendTextString(const char* text, const uint8_t delayms);
extern void LCD_WriteExpander(uint8_t _data);
extern void LCD_WriteInitSequence();

#endif