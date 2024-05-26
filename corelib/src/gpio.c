#include "gpio.h"

void GPIOEnablePortBus(const uint32_t port)
{
    // Different platforms have GPIO ports on different clock bus
    #if PLATFORMID == PLAT_NUCLEO32F401RE
        switch(port)
        {
            case GPIO_PORTA: RCC->AHB1ENR |= RCC_AHBENR_GPIOAEN; break;
            case GPIO_PORTB: RCC->AHB1ENR |= RCC_AHBENR_GPIOBEN; break;
            case GPIO_PORTC: RCC->AHB1ENR |= RCC_AHBENR_GPIOCEN; break;
            case GPIO_PORTD: RCC->AHB1ENR |= RCC_AHBENR_GPIODEN; break;
            case GPIO_PORTE: RCC->AHB1ENR |= RCC_AHBENR_GPIOEEN; break;
            case GPIO_PORTH: RCC->AHB1ENR |= RCC_AHBENR_GPIOFHN; break;
        }
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        switch(port)
        {
            case GPIO_PORTA: RCC->AHBENR |= RCC_AHBENR_GPIOAEN; break;
            case GPIO_PORTB: RCC->AHBENR |= RCC_AHBENR_GPIOBEN; break;
            case GPIO_PORTC: RCC->AHBENR |= RCC_AHBENR_GPIOCEN; break;
            case GPIO_PORTD: RCC->AHBENR |= RCC_AHBENR_GPIODEN; break;
            case GPIO_PORTF: RCC->AHBENR |= RCC_AHBENR_GPIOFEN; break;
        }
    #endif
}

GPIO_TypeDef* GPIOGetReg(const uint32_t port)
{
    // Different platforms have different GPIO port configuration
    switch(port)
    {
        case GPIO_PORTA : return GPIOA;
        case GPIO_PORTB : return GPIOB;
        case GPIO_PORTC : return GPIOC;
        case GPIO_PORTD : return GPIOD;
    }


    #if PLATFORMID == PLAT_NUCLEO32F401RE
        switch(port)
        {
            case GPIO_PORTE: return GPIOE;
            case GPIO_PORTH: return GPIOH;
        }
    #elif PLATFORMID == PLAT_NUCLEO32F303K8
        switch(port)
        {
            case GPIO_PORTF: return GPIOF;
        }
    #endif

    return NULL;
}

void GPIOEnable(const uint32_t port, const uint32_t pin, const uint32_t mode, GPIODesc *outGPIO)
{
    // Enable clock bus for GPIO port that contains the pin
    GPIOEnablePortBus(port);

    // Fetch hardware register for GPIO port of given pin
    GPIO_TypeDef* reg = GPIOGetReg(port);

    // Clear pin mode first to ensure we default to READ - (Pin offset example - stm32f303x8.h Line:7473)
    // GPIO default state after MCU reset is READ, but some pins are exceptions (Exmaple: PB3 on F303K8 which defaults to alt)
    // Check target MCU data sheets for clarity if we re getting undefined behaviour!
    const uint32_t PIN_OFFSET = pin * 2U;
    __CLEAR(reg->MODER, 0x3UL << PIN_OFFSET);

    // GPIO MODER register values contains 2 bits for each pin
    // Total combination of 4 modes:
    // 0x0: READ, 0x1: WRITE, 0x2: ALT, 0x3: ANALOG
    switch(mode)
    {
        case GPIO_MODE_WRITE: __SET(reg->MODER, 0x1UL << PIN_OFFSET); break;    // OUTPUT MODE
        case GPIO_MODE_ALT: __SET(reg->MODER, 0x2UL << PIN_OFFSET); break;      // ALT FUNCTION MODE
        case GPIO_MODE_ANALOG: __SET(reg->MODER, 0x3UL << PIN_OFFSET); break;   // ANALOG MODE
    }

    // Return GPIO description
    outGPIO->m_port = port;
    outGPIO->m_mode = mode;
    outGPIO->m_pin = pin;
}

void GPIOSetMode(const GPIODesc* gpio, const uint32_t mode)
{
    // Fetch hardware register for GPIO port of given pin
    GPIO_TypeDef* reg = GPIOGetReg(gpio->m_port);

    // Clear pin mode first to ensure we default to READ - (Pin offset example - stm32f303x8.h Line:7473)
    // GPIO default state after MCU reset is READ, but some pins are exceptions (Exmaple: PB3 on F303K8 which defaults to alt)
    // Check target MCU data sheets for clarity if we re getting undefined behaviour!
    const uint32_t PIN_OFFSET = gpio->m_pin * 2U;
    __CLEAR(reg->MODER, 0x3UL << PIN_OFFSET);

    switch(mode)
    {
        case GPIO_MODE_WRITE: __SET(reg->MODER, 0x1UL << PIN_OFFSET); break;    // OUTPUT MODE
        case GPIO_MODE_ALT: __SET(reg->MODER, 0x2UL << PIN_OFFSET); break;      // ALT FUNCTION MODE
        case GPIO_MODE_ANALOG: __SET(reg->MODER, 0x3UL << PIN_OFFSET); break;   // ANALOG MODE
    }
}

void GPIOSetAltFn(const GPIODesc *gpio, const uint32_t fn)
{
    // Fetch hardware register for GPIO port of given pin
    GPIO_TypeDef* reg = GPIOGetReg(gpio->m_port);

    // Specify alternative function
    // AFR register specifies 4 bits per bit for a grant total of 16 different alt function per pin
    // Different pins support different alternative function on different MCU platforms - consult data sheets
    const uint32_t PIN_OFFSET = 4U;
    if(gpio->m_pin < 8)
    {
        // Use AFR low register for the first 8 pins
        //__CLEAR(reg->AFR[0], 0x15UL << PIN_OFFSET);
        reg->AFR[0] |= (fn << (PIN_OFFSET * gpio->m_pin));
    }
    else 
    {
        // Use AFR high register for the last 8 pins
        //__CLEAR(reg->AFR[1], 0x15UL << (PIN_OFFSET * (gpio->m_pin - 8)));
        reg->AFR[1] |= (fn << (PIN_OFFSET * (gpio->m_pin - 8)));
    }
}

void GPIOSetOutputType(const GPIODesc *gpio, const uint32_t type)
{
    // Fetch hardware register for GPIO port of given pin
    GPIO_TypeDef* reg = GPIOGetReg(gpio->m_port);

    // Push-Pull is the default state of GPIO pin output.
    // Some MCU boards might be overriding certain pin output types on hardware reset to support certain
    // board functionaluty such as hardware debugging or comms with arduino MCU's (check datasheets).
    switch(type)
    {
        case GPIO_OUT_TYPE_PUSHPULL: __CLEAR(reg->OTYPER, gpio->m_pin); break;
        case GPIO_OUT_TYPE_OPENDRAIN: __SET(reg->OTYPER, gpio->m_pin); break;
    }
}

void GPIOSetOutputSpeed(const GPIODesc *gpio, const uint32_t speed)
{
    // Fetch hardware register for GPIO port of given pin
    GPIO_TypeDef* reg = GPIOGetReg(gpio->m_port);

    // OSPEEDR register uses 2 bits for each pin for a combination of 3 speed modes:
    // xx0 - Low speed
    // x01 - Medium speed
    // x11 - High speed

    const uint32_t PIN_OFFSET = 2U;

    // Reset seed bits
    __CLEAR(reg->OSPEEDR, (gpio->m_pin * PIN_OFFSET));
    __CLEAR(reg->OSPEEDR, (gpio->m_pin * PIN_OFFSET) + 1U);

    // Set new speed
    __SET(reg->OSPEEDR, speed << (gpio->m_pin * PIN_OFFSET));
}

void GPIODisable(const GPIODesc *gpio)
{
    // TODO: Disabling GPIO bus clock here might not be a good idea
}

void GPIOWrite(const GPIODesc *gpio, const uint32_t val)
{
    // Fetch hardware register for GPIO port of given pin
    GPIO_TypeDef* reg = GPIOGetReg(gpio->m_port);

    switch(val)
    {
        case GPIO_VAL_LOW: __SET(reg->BSRR, 1 << (16U + gpio->m_pin)); break;
        case GPIO_VAL_HIGH: __SET(reg->BSRR, 1 << gpio->m_pin); break;
    }
}

void GPIORead(const GPIODesc *gpio, uint32_t *outVal)
{
    GPIO_TypeDef* reg = GPIOGetReg(gpio->m_port);
    *outVal = (uint32_t) (reg->IDR >> gpio->m_pin ) & 1U;
}

void GPIOToggle(const GPIODesc *gpio)
{
    GPIO_TypeDef* reg = GPIOGetReg(gpio->m_port);
    const uint32_t val = (reg->ODR >> gpio->m_pin ) & 1U;
    
    switch(val)
    {
        case GPIO_VAL_LOW:
        reg->BSRR |= 1 << gpio->m_pin; break;
        
        case GPIO_VAL_HIGH:
        reg->BSRR |= 1 << (16U + gpio->m_pin); break;
    }
}