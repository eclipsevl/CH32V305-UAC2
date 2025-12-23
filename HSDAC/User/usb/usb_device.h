#pragma once

#include "usb_setup_request.h"
#include "usb_endpoint.h"

#define USB_EP0_MAX_PACKAGE_SIZE 64

struct UsbDevice {
    __attribute__((aligned(4)))
    uint8_t usb_ep0_buffer[USB_EP0_MAX_PACKAGE_SIZE];
    struct UsbEndpoint ep0;

    struct UsbSetupRequest setup_request;

    uint8_t using_configuration;
    uint8_t address;
    struct {
        uint8_t self_power : 1;
        uint8_t remote_wakeup : 1;
        uint8_t test_flag : 1;
    } device_status;
};

extern struct UsbDevice usb_device;
void UsbDevice_Init();
