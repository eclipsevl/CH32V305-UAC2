#pragma once
#include "hardware.hpp"
#include "endpoint.hpp"

struct UsbHardware {
    void OpenEndpoint(uint8_t address, EndpointType type, uint16_t max_packet_size, UsbdClass& usb_class);
    void CloseEndpoint(uint8_t address);
    void EndpointSend(uint8_t address, uint8_t const* buffer, uint32_t count);
    void EndpointRecive(uint8_t address, uint8_t const* buffer, uint32_t count);

    EndpointContext in_endpoints[16];
    EndpointContext out_endpoints[16];
    Hardware hal;
};
