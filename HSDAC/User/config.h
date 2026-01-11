#pragma once

// i2s dma缓冲区大小，延迟大概是1/3 ~ 2/3
#define I2S_DMA_BUFFER_SIZE 256
// 如果有这么多的可写空间会尝试填充0到缓冲区中间
#define I2S_DMA_FILL_ZERO_THRESHOULD (I2S_DMA_BUFFER_SIZE * 5 / 6)
// 处理缓冲区每个包的采样数量
#define UAC_MAX_PACKAGE_SIZE (192000 / 8000)
// 处理缓冲区的包数量
#define NUM_AUDIO_BLOCK 16

// 采样率见 usb/usb_desc.cpp kUac2SampleRateTable变量 codec_i2s.c采样率处理相关
// 位数见   usb/usb_desc.cpp kConfigHs流接口
