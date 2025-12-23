#include "ch32v30x_misc.h"
#include "debug.h"
#include "tick.h"
#include "usbd.h"
#include "codec.h"

uint32_t GetDmaSamplesHave();

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    Tick_Init();
    Usbd_Init();
    Codec_Init();

    Usbd_Connect();
    uint32_t tick = Tick_GetTick();
    for (;;) {
        Codec_Handler();

        uint32_t now = Tick_GetTick();
        if (now - tick > 1000) {
            printf("fb:%lu, have:%lu\n\r", Codec_GetFeedbackFs(), GetDmaSamplesHave());
            tick = now;
        }
    }

    return 0;
}
