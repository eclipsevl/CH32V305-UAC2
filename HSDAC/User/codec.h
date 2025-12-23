#pragma once

#include <stdint.h>

void Codec_Init();
void Codec_Handler();

void Codec_Start();
void Codec_Stop();

uint32_t Codec_GetSampleRate();
void     Codec_SetSampleRate(uint32_t sample_rate);

void    Codec_Mute(uint8_t channel, uint8_t mute);
uint8_t Codec_IsMute(uint8_t channel);
void    Codec_SetVolume(uint8_t channel, int16_t volume);
int16_t Codec_GetVolume(uint8_t channel);

void     Codec_WriteBuffer(uint8_t const* buffer, uint32_t bytes);
uint32_t Codec_GetFeedbackFs();
