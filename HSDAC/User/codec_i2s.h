#pragma once

#include <stdint.h>

void CodecI2s_Init(void);
void CodecI2s_DeInit(void);

void CodecI2s_Start(void);
void CodecI2s_Stop(void);
void CodecI2s_WriteUACBuffer(const uint8_t* ptr, uint32_t len);
void CodecI2s_SetSampleRate(uint32_t sample_rate);

int32_t CodecI2s_GetCurrentSizeDiffFromCenter();
