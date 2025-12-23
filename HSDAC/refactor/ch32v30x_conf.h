/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_conf.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Library configuration file.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V30x_CONF_H
#define __CH32V30x_CONF_H

#include "ch32v30x_adc.h"
#include "ch32v30x_bkp.h"
#include "ch32v30x_can.h"
#include "ch32v30x_crc.h"
#include "ch32v30x_dac.h"
#include "ch32v30x_dbgmcu.h"
#include "ch32v30x_dma.h"
#include "ch32v30x_exti.h"
#include "ch32v30x_flash.h"
#include "ch32v30x_fsmc.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_i2c.h"
#include "ch32v30x_iwdg.h"
#include "ch32v30x_pwr.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_rtc.h"
#include "ch32v30x_sdio.h"
#include "ch32v30x_spi.h"
#include "ch32v30x_tim.h"
#include "ch32v30x_usart.h"
#include "ch32v30x_wwdg.h"
#include "ch32v30x_misc.h"

#define WCH_FAST_INTERRUPT __attribute__((interrupt("WCH-Interrupt-fast")))

#endif /* __CH32V30x_CONF_H */


	
	
	
// #include "usb/ch32v30x_usbhs_device.h"
// #include "debug.h"
// #include "codec.h"
// #include <math.h>
// #include "tick.h"
// #include "hid_queue.h"
// #include "codec_i2c.h"

// static void ES9018K_Init(void) {
//     Codec_Init();
//     GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
//     Delay_Ms(100);
//     GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
//     Delay_Ms(100);

//     CodecI2c_Init();
//     CodecI2c_Write(0x90, 0, 0xf1, 1000);
    

//     CodecI2c_Write(0x90, 0, 0xf0, 1000);
//     CodecI2c_Write(0x90, 1, 0b10000000, 1000);
// }

// extern uint32_t max_uac_len_ever;
// extern uint32_t min_uac_len_ever;
// extern uint32_t mesured_dma_sample_rate_;
// extern uint32_t mesured_usb_sample_rate_;
// extern float report_fs_;
// extern uint8_t vol_[];

// int main(void) {
//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//     SystemCoreClockUpdate();
//     Delay_Init();
//     Tick_Init();

//     ES9018K_Init();
//     USBHS_RCC_Init();
//     USBHS_Device_Init(ENABLE);

//     uint32_t t = Tick_GetTick();
//     uint32_t t2 = Tick_GetTick();
//     for (;;) {
//         uint32_t ct = Tick_GetTick();
//         if ((ct - t) > 1000) {
//             t = ct;

//             if (USBHS_DevEnumStatus) {
//                 printf("[fs]usb: %uHz, dma: %uHz, fb:%uHz\n\r", mesured_usb_sample_rate_, mesured_dma_sample_rate_, (uint32_t)report_fs_);
//                 printf("[uac]len: %u, min: %u, max:%u\n\r\n\r", Codec_GetUACBufferLen(), min_uac_len_ever, max_uac_len_ever);
//             }
//         }

//         if ((ct - t2) > 10) {
//             t2 = ct;
//             if (min_uac_len_ever < 256)
//                 ++min_uac_len_ever;
//             if (max_uac_len_ever > 0)
//                 --max_uac_len_ever;
//         }

//         Codec_CheckVolumeEvent();

//         uint32_t len;
//         HID_Queue_Read(&g_hid_queue, &len);
//         for (uint32_t i = 0; i < len; ++i) {
//             struct HID_Event* e = HID_Queue_GetItem(&g_hid_queue, i);
//             if (USBHS_DevEnumStatus) {
//                 printf("[hid]type: %d, reg: %d, val: %d\n\r", (int)e->type, e->reg, e->val);
//             }

//             if (e->type == 0) {
//                 // dac param setting
//                 Codec_PollWrite(e->reg, e->val);
//             }
//             else if (e->type == 1 && e->reg == 0x12 && e->val == 0x34) {
//                 // firmware update
//                 RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );
//                 BKP_TamperPinCmd( DISABLE );
//                 PWR_BackupAccessCmd( ENABLE );
//                 BKP_ClearFlag();

//                 BKP_WriteBackupRegister(BKP_DR1, 0x1234);
//                 BKP_WriteBackupRegister(BKP_DR2, 0x5678);
//                 Codec_DeInit();
//                 USBHS_Device_Init(DISABLE);
//                 NVIC_SystemReset();
//             }
//             else if (e->type == 2) {
//                 // latency setting
//                 uint32_t pos = e->reg << 8 | e->val;
//                 Codec_SetLatencyPos(pos);
//             }
//         }
//         HID_Queue_Finish(&g_hid_queue, len);
//     }
// }
