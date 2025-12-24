#pragma once

// i2s dma缓冲区大小，延迟大概是1/3 ~ 1/2
#define I2S_DMA_BUFFER_SIZE 256
// 如果64帧内有这个数量的微针没有发送数据，将会进入静音阶段阻止突发高负载下的pop
#define UAC2_SLIENCE_DETECT_FRAMES 40
// 进入静音阶段后保持静音的时间
#define UAC2_SLIENCE_HOLD_TIME 500

// 采样率见 usb/usb_desc.cpp kUac2SampleRateTable变量
// 位数见   usb/usb_desc.cpp kConfigHs流接口
