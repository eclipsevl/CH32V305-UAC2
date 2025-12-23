#include "usbd.h"

#include "ch32v30x_rcc.h"
#include "usb/usb_hardware.h"
#include "debug.h"
#include "ch32v30x_misc.h"
#include "usb/usb_device.h"

void Usbd_Init() {
    RCC_USBCLK48MConfig (RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig (RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig (RCC_USBPLL_Div2);
    RCC_USBHSPLLCKREFCLKConfig (RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd (ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_USBHS, ENABLE);

    UsbDevice_Init();
}

void Usbd_Connect() {
    USBHSD->CONTROL |= USBHS_UC_DEV_PU_EN;

    NVIC_InitTypeDef nvic = {
        .NVIC_IRQChannel = USBHS_IRQn,
        .NVIC_IRQChannelCmd = ENABLE,
        .NVIC_IRQChannelPreemptionPriority = 0,
        .NVIC_IRQChannelSubPriority = 0
    };
    NVIC_Init(&nvic);
}

void Usbd_DisConnect() {
    USBHSD->CONTROL = USBHS_UC_CLR_ALL | USBHS_UC_RESET_SIE;
    Delay_Us (10);
    USBHSD->CONTROL &= ~USBHS_UC_RESET_SIE;
    NVIC_DisableIRQ (USBHS_IRQn);
}
