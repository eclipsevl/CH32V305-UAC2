#pragma once
#include <stdint.h>
#include <stdbool.h>

struct UsbSetupRequest {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

enum UsbRequestType {
    kUsbRequestType_Standard = 0,
    kUsbRequestType_Class,
    kUsbRequestType_Vendor,
};

enum UsbRequestReciver {
    kUsbRequestReciver_Device = 0,
    kUsbRequestReciver_Interface,
    kUsbRequestReciver_Endpoint,
    kUsbRequestReciver_Other
};

enum UsbStandardRequest {
    // device
    kUsbStandardRequest_GetDeviceStatus = 0x8000 | 0,
    kUsbStandardRequest_ClearDeviceFeature = 0x0000 | 1,
    kUsbStandardRequest_SetDeviceFeature = 0x0000 | 3,
    kUsbStandardRequest_SetAddress = 0x0000 | 5,
    kUsbStandardRequest_GetDescriptor = 0x8000 | 6,
    kUsbStandardRequest_SetDescriptor = 0x0000 | 7,
    kUsbStandardRequest_GetConfiguration = 0x8000 | 8,
    kUsbStandardRequest_SetConfiguration = 0x0000 | 9,
    // interface
    kUsbStandardRequest_GetInterfaceStatus = 0x8100 | 0,
    kUsbStandardRequest_ClearInterfaceFeature = 0x0100 | 1,
    kUsbStandardRequest_SetInterfaceFeature = 0x0100 | 3,
    kUsbStandardRequest_GetInterfaceAlter = 0x8100 | 10,
    kUsbStandardRequest_SetInterfaceAlter = 0x0100 | 11,
    kUsbStandardRequest_GetInterfaceDescriptor = 0x8100 | 6,
    // endpoint
    kUsbStandardRequest_GetEndpointStatus = 0x8200 | 0,
    kUsbStandardRequest_ClearEndpointFeature = 0x0200 | 1,
    kUsbStandardRequest_SetEndpointFeature = 0x0200 | 3,
    kUsbStandardRequest_SynchFrame = 0x8200 | 12
};

static inline bool UsbSetupRequest_IsDirectionIn(struct UsbSetupRequest const* req) {
    return (req->bmRequestType & 0x80) != 0;
}

static inline enum UsbRequestType UsbSetupRequest_GetRequestType(struct UsbSetupRequest const* req) {
    return (enum UsbRequestType)((req->bmRequestType >> 5) & 0x3);
}

static inline enum UsbRequestReciver UsbSetupRequest_GetRequestReciver(struct UsbSetupRequest const* req) {
    return (enum UsbRequestReciver)(req->bmRequestType & 0x1f);
}

static inline enum UsbStandardRequest UsbSetupRequest_GetStandardRequest(struct UsbSetupRequest const* req) {
    return (enum UsbStandardRequest)((req->bmRequestType << 8) | req->bRequest);
}
