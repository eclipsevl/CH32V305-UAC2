#pragma once
#include <cstdint>

enum class EndpointType : uint8_t {
    Control,
    Interrupt,
    Bulk,
    Iso
};

struct UsbdClass;
struct EndpointContext {
    UsbdClass* dispatch;
    uint32_t transfer_remain;
    uint32_t transfer_count;
    uint32_t max_packet_size;
    uint8_t* transfer_buffer;
    struct {
        uint8_t stall : 1;
        uint8_t zero_package : 1;
        EndpointType type : 2;
    } attribute;
};
