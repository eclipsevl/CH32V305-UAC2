#include "codec_i2c.h"

#include "ch32v30x_i2c.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_gpio.h"

#include "pt.h"
#include "tick.h"

// typedef
struct CodecI2cState {
    struct {
        uint8_t write;
        uint8_t busy;
    } bits;
    uint8_t address;
    uint8_t reg;
    uint8_t value;

    uint32_t start_tick;
    uint32_t timeout_ticks;

    struct pt coroutine;
};

// variable
static struct CodecI2cState state_ = {
    .bits.busy = 0,
    .bits.write = 0,
    .start_tick = 0,
    .coroutine = pt_init()
};

// static declare
static void CodecI2c_TickWrite() {
    pt_begin(&state_.coroutine);

    pt_wait(&state_.coroutine, I2C_GetFlagStatus (I2C2, I2C_FLAG_BUSY) == RESET);

    I2C_GenerateSTART(I2C2, ENABLE);
    pt_wait(&state_.coroutine, I2C_CheckEvent (I2C2, I2C_EVENT_MASTER_MODE_SELECT) == READY);

    I2C_Send7bitAddress(I2C2, state_.address, I2C_Direction_Transmitter);
    pt_wait(&state_.coroutine, I2C_CheckEvent (I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == READY);
    pt_wait(&state_.coroutine, I2C_GetFlagStatus (I2C2, I2C_FLAG_TXE) == SET);

    I2C_SendData(I2C2, state_.reg);
    pt_wait(&state_.coroutine, I2C_GetFlagStatus (I2C2, I2C_FLAG_TXE) == SET);

    I2C_SendData(I2C2, state_.value);
    pt_wait(&state_.coroutine, I2C_GetFlagStatus (I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == SET);

    I2C_GenerateSTOP (I2C2, ENABLE);

    pt_end(&state_.coroutine);

    state_.bits.busy = 0;
}

static void CodecI2c_TickRead() {
    pt_begin(&state_.coroutine);

    pt_wait(&state_.coroutine, I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == RESET);

    I2C_GenerateSTART(I2C2, ENABLE);
    pt_wait(&state_.coroutine, I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) == READY);

    I2C_Send7bitAddress(I2C2, state_.address, I2C_Direction_Transmitter);
    pt_wait(&state_.coroutine, I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == READY);
    pt_wait(&state_.coroutine, I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) == SET);

    I2C_SendData(I2C2, state_.reg);
    pt_wait(&state_.coroutine, I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == READY);

    I2C_GenerateSTART(I2C2, ENABLE);
    pt_wait(&state_.coroutine, I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) == READY);

    I2C_Send7bitAddress(I2C2, state_.address, I2C_Direction_Receiver);
    pt_wait(&state_.coroutine, I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == READY);

    pt_wait(&state_.coroutine, I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == SET);
    state_.value = I2C_ReceiveData(I2C2);

    I2C_GenerateSTOP(I2C2, ENABLE);

    pt_end(&state_.coroutine);

    state_.bits.busy = 0;
}

// public implement
void CodecI2c_Init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    I2C_DeInit(I2C2);

    GPIO_InitTypeDef init;
    init.GPIO_Mode = GPIO_Mode_AF_OD;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    init.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOB, &init);

    I2C_InitTypeDef i2c_init = {
        .I2C_ClockSpeed = 400000,
        .I2C_Mode = I2C_Mode_I2C,
        .I2C_DutyCycle = I2C_DutyCycle_2,
        .I2C_Ack = I2C_Ack_Enable,
        .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
    };
    I2C_Init(I2C2, &i2c_init);
    I2C_Cmd(I2C2, ENABLE);
}

void CodecI2c_Deinit() {
    I2C_DeInit(I2C2);
}

void CodecI2c_Write(
    uint8_t address,
    uint8_t reg,
    uint8_t value,
    uint32_t timeout_ticks
) {
    state_.address = address;
    state_.reg = reg;
    state_.value = value;
    state_.bits.write = 1;
    state_.bits.busy = 1;
    state_.timeout_ticks = timeout_ticks;
    state_.start_tick = Tick_GetTick();
    state_.coroutine.label = 0;
    state_.coroutine.status = 0;
}

void CodecI2c_Read(
    uint8_t address,
    uint8_t reg,
    uint32_t timeout_ticks
) {
    state_.address = address;
    state_.reg = reg;
    state_.bits.write = 0;
    state_.bits.busy = 1;
    state_.timeout_ticks = timeout_ticks;
    state_.start_tick = Tick_GetTick();
    state_.coroutine.label = 0;
    state_.coroutine.status = 0;
}

bool CodecI2c_IsBusy() {
    return state_.bits.busy;
}

enum CodecI2cError CodecI2c_Tick() {
    if (state_.bits.busy == 0) {
        return kCodecI2cError_Finish;
    }

    uint32_t now = Tick_GetTick();
    if (now - state_.start_tick > state_.timeout_ticks) {
        state_.bits.busy = 0;
        I2C_GenerateSTOP(I2C2, ENABLE);
        return kCodecI2cError_Timeout;
    }

    if (state_.bits.write == 1) {
        CodecI2c_TickWrite();
    }
    else {
        CodecI2c_TickRead();
    }
    return state_.bits.busy == 0 ? kCodecI2cError_Finish : kCodecI2cError_Busy;
}

#define QUIT_IF_TIMEOUT()\
    if (Tick_GetTick() - start_tick > timeout_ticks) {\
        I2C_GenerateSTOP (I2C2, ENABLE);\
        return kCodecI2cError_Timeout;\
    }

enum CodecI2cError CodecI2c_PollWrite(
    uint8_t address,
    uint8_t reg,
    uint8_t value,
    uint32_t timeout_ticks
) {
    uint32_t start_tick = Tick_GetTick();
    while (I2C_GetFlagStatus (I2C2, I2C_FLAG_BUSY) == SET) {
        QUIT_IF_TIMEOUT();
    }

    I2C_GenerateSTART(I2C2, ENABLE);
    while (I2C_CheckEvent (I2C2, I2C_EVENT_MASTER_MODE_SELECT) == NoREADY) {
        QUIT_IF_TIMEOUT();
    }

    I2C_Send7bitAddress(I2C2, address, I2C_Direction_Transmitter);
    while (I2C_CheckEvent (I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == NoREADY) {
        QUIT_IF_TIMEOUT();
    }
    while (I2C_GetFlagStatus (I2C2, I2C_FLAG_TXE) == RESET) {
        QUIT_IF_TIMEOUT();
    }

    I2C_SendData(I2C2, reg);
    while (I2C_GetFlagStatus (I2C2, I2C_FLAG_TXE) == RESET) {
        QUIT_IF_TIMEOUT();
    }

    I2C_SendData(I2C2, value);
    while (I2C_GetFlagStatus (I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET) {
        QUIT_IF_TIMEOUT();
    }

    I2C_GenerateSTOP (I2C2, ENABLE);
    return kCodecI2cError_Finish;
}

enum CodecI2cError CodecI2c_PollRead(
    uint8_t address,
    uint8_t reg,
    uint8_t* value,
    uint32_t timeout_ticks
) {
    uint32_t start_tick = Tick_GetTick();
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET) {
        QUIT_IF_TIMEOUT();
    }

    I2C_GenerateSTART(I2C2, ENABLE);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) == NoREADY) {
        QUIT_IF_TIMEOUT();
    }

    I2C_Send7bitAddress(I2C2, address, I2C_Direction_Transmitter);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == NoREADY) {
        QUIT_IF_TIMEOUT();
    }
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) == RESET) {
        QUIT_IF_TIMEOUT();
    }

    I2C_SendData(I2C2, reg);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == NoREADY) {
        QUIT_IF_TIMEOUT();
    }

    I2C_GenerateSTART(I2C2, ENABLE);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) == NoREADY) {
        QUIT_IF_TIMEOUT();
    }

    I2C_Send7bitAddress(I2C2, address, I2C_Direction_Receiver);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == NoREADY) {
        QUIT_IF_TIMEOUT();
    }

    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET) {
        QUIT_IF_TIMEOUT();
    }
    *value = I2C_ReceiveData(I2C2);

    I2C_GenerateSTOP(I2C2, ENABLE);
    return kCodecI2cError_Finish;
}

uint8_t CodecI2c_GetReg() {
    return state_.reg;
}

uint8_t CodecI2c_GetWriteValue() {
    return state_.value;
}
