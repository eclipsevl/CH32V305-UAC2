#ifndef _TICK_H
#define _TICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void     Tick_Init(void);
uint32_t Tick_GetTick(void);
void     Tick_DelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // !_TICK_H
