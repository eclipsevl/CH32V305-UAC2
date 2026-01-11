#include "audio_block.h"

#include <stdatomic.h>

#include "codec_i2s.h"

static struct AudioBlock audio_blocks_[NUM_AUDIO_BLOCK];
static uint32_t rpos_;
static uint32_t wpos_;
static uint32_t size_;

static struct AudioBlock temp_trash_block_;

// ----------------------------------------
// implement
// ----------------------------------------
static void ProcessBlock(struct StereoSample* ptr, uint32_t len) {
    (void)ptr;
    (void)len;
}

// ----------------------------------------
// public
// ----------------------------------------
struct AudioBlock* AudioBlock_GetPrepareForUac() {
    if (atomic_load(&size_) == NUM_AUDIO_BLOCK) {
        return &temp_trash_block_;
    }
    else {
        struct AudioBlock* ret = &audio_blocks_[wpos_];
        return ret;
    }
}

void AudioBlock_UacRecivied(struct AudioBlock* block) {
    if (block == &temp_trash_block_) return;

    block->size /= sizeof(struct StereoSample);
    wpos_ = (wpos_ + 1) & (NUM_AUDIO_BLOCK - 1);
    atomic_fetch_add(&size_, 1);
}

void AudioBlock_Handler() {
    uint32_t num_consume = 0;
    uint32_t num_block = atomic_load(&size_);
    uint32_t free_space = CodecI2s_GetFreeSpace();
    while (free_space != 0 && num_block != 0) {
        struct AudioBlock* curr = &audio_blocks_[rpos_];
        struct StereoSample* src = curr->buffer + curr->rpos;
        uint32_t total_write = curr->size - curr->rpos;
        uint32_t can_write = total_write > free_space ? free_space : total_write;
        ProcessBlock(src, can_write);
        CodecI2s_WriteUACBufferNocheck(src, can_write);
        curr->rpos += can_write;
        free_space -= can_write;
        if (curr->rpos == curr->size) {
            curr->rpos = 0;
            curr->size = 0;
            ++num_consume;
            --num_block;
            rpos_ = (rpos_ + 1) & (NUM_AUDIO_BLOCK - 1);
        }
    }
    CodecI2s_FillZeroIfTooSmallData();
    atomic_fetch_sub(&size_, num_consume);
}
