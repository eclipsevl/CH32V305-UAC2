#ifndef _CODEC_I2C_H
#define _CODEC_I2C_H

#include <stdint.h>
#include <stdbool.h>

enum CodecI2cError {
    kCodecI2cError_Timeout,
    kCodecI2cError_Finish,
    kCodecI2cError_Busy
};

void CodecI2c_Init();
void CodecI2c_Deinit();

bool CodecI2c_IsBusy();

void CodecI2c_Write(
    uint8_t address,
    uint8_t reg,
    uint8_t value,
    uint32_t timeout_ticks
);

void CodecI2c_Read(
    uint8_t address,
    uint8_t reg,
    uint32_t timeout_ticks
);

enum CodecI2cError CodecI2c_Tick();

uint8_t CodecI2c_GetReg();
uint8_t CodecI2c_GetWriteValue();

enum CodecI2cError CodecI2c_PollWrite(
    uint8_t address,
    uint8_t reg,
    uint8_t value,
    uint32_t timeout_ticks
);

enum CodecI2cError CodecI2c_PollRead(
    uint8_t address,
    uint8_t reg,
    uint8_t* value,
    uint32_t timeout_ticks
);

/*

if (!CodecI2c_IsBusy()) {
    if (write) {
        CodecI2c_Write(...);
    }
    else (
        CodecI2c_Read(...);
    )
    goto _tick;
}
else {
_tick:
    auto result = CodecI2c_Tick();
    if (result == Timeout) {
        // do something
    }
    else if (result == Finish) {
        // do something
    }
}

*/

#endif
