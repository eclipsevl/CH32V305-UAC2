#pragma once
#include <cstdint>

struct SetupRequest;
class UsbHardware;

struct UsbdClass {
    virtual void Init(UsbHardware& device) = 0;
    virtual void Reset(UsbHardware& device) = 0;
    virtual void ClassRequest(SetupRequest const& request, bool& allow_operation) = 0;
    virtual void SetInterfaceAlter(uint8_t alter) = 0;
    virtual void EndpointInComplete(uint8_t ep_num) = 0;
    virtual void EndpointOutComplete(uint8_t ep_num) = 0;
};
