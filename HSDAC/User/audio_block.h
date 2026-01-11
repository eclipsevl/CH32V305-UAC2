#pragma once

#include <stdint.h>
#include "config.h"

struct StereoSample {
    int32_t left;
    int32_t right;
};

struct AudioBlock {
    struct StereoSample buffer[UAC_MAX_PACKAGE_SIZE];
    uint32_t rpos;
    uint32_t size;
};

struct AudioBlock* AudioBlock_GetPrepareForUac();
void AudioBlock_UacRecivied(struct AudioBlock* block);
void AudioBlock_Handler();
