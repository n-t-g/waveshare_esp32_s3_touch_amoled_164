#ifndef FT3168_H
#define FT3168_H
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif 

void Touch_Init(void);

uint8_t getTouch(uint16_t *x,uint16_t *y);

uint8_t getTouchRaw(uint8_t *status, uint16_t *x, uint16_t *y);

void Touch_DebugDump(void);

#ifdef __cplusplus
}
#endif
#endif