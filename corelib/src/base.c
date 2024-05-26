#include "base.h"

uint8_t ReverseByte(const uint8_t value)
{
    uint8_t val = value;
    val = ((val >> 1) & 0x55) | ((val << 1) & 0xAA);
    // Swap consecutive pairs
    val = ((val >> 2) & 0x33) | ((val << 2) & 0xCC);
    // Swap nibbles (4-bit groups)
    val = ((val >> 4) & 0x0F) | ((val << 4) & 0xF0);
    return val;
}