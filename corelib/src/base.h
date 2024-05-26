
#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> 

#define __REG volatile

#define __SET(reg, bit)     ((reg) |= (bit))
#define __CLEAR(reg, bit)   ((reg) &= ~(bit))
#define __READ(reg, bit)    ((reg) & (bit))
#define __MODIFY(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

extern uint8_t ReverseByte(const uint8_t value);

#endif