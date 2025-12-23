#pragma once
#include <cstdint>
#include <cstddef>

template<class Type>
struct BufferSpan {
    Type* buffer_{};
    size_t len_{};

    Type* data() const {
        return buffer_;
    }

    bool empty() const {
        return len_ == 0;
    }

    size_t size() const {
        return len_;
    }

    BufferSpan subspan(size_t offset, size_t size) {
        return BufferSpan{
            buffer_ + offset,
            size
        };
    }
};

struct SimpleSerializer {
    template<class T>
    void Append(T const& value) noexcept {
        memcpy(buffer, &value, sizeof(T));
        buffer += sizeof(T);
        num_write += sizeof(T);
    }

    uint8_t* buffer;
    size_t num_write;
};

struct UsbDescriptor {
    BufferSpan<const uint8_t> GetDevice() const;
    BufferSpan<const uint8_t> GetFullSpeedConfiguration() const;
    BufferSpan<const uint8_t> GetHighSpeedConfiguration() const;
    BufferSpan<const uint8_t> GetOtherSpeedConfiguration() const;
    BufferSpan<const uint8_t> GetString(uint8_t index) const;
    BufferSpan<const uint8_t> GetHid(uint8_t index) const;
};

struct SetupRequest {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;

    bool IsDirectionIn() const noexcept {
        return (bmRequestType & 0x80) != 0;
    }

    enum CommandType {
        kStandard = 0,
        kClass,
        kVendor,
    };
    CommandType GetCommandType() const noexcept {
        return static_cast<CommandType>((bmRequestType >> 5) & 0x3);
    }

    enum CommandReciver {
        kDevice = 0,
        kInterface,
        kEndpoint,
        kOther
    };
    CommandReciver GetCommandReciver() const noexcept {
        return static_cast<CommandReciver>(bmRequestType & 0x1f);
    }

    enum StandardRequest {
        // device
        kGetDeviceStatus = 0x80'00 | 0,
        kClearDeviceFeature = 0x00'00 | 1,
        kSetDeviceFeature = 0x00'00 | 3,
        kSetAddress = 0x00'00 | 5,
        kGetDescriptor = 0x80'00 | 6,
        kSetDescriptor = 0x00'00 | 7,
        kGetConfiguration = 0x80'00 | 8,
        kSetConfiguration = 0x00'00 | 9,
        // interface
        kGetInterfaceStatus = 0x81'00 | 0,
        kClearInterfaceFeature = 0x01'00 | 1,
        kSetInterfaceFeature = 0x01'00 | 3,
        kGetInterfaceAlter = 0x81'00 | 10,
        kSetInterfaceAlter = 0x01'00 | 11,
        kGetInterfaceDescriptor = 0x81'00 | 6,
        // endpoint
        kGetEndpointStatus = 0x82'00 | 0,
        kClearEndpointFeature = 0x02'00 | 1,
        kSetEndpointFeature = 0x02'00 | 3,
        kSynchFrame = 0x82'00 | 12
    };
    StandardRequest GetStandardRequest() const noexcept {
        return static_cast<StandardRequest>((bmRequestType << 8) | bRequest);
    }
};

struct UsbStatus {
    uint8_t using_configuration = 0;
    struct {
        uint8_t self_power : 1;
        uint8_t remote_wakeup : 1;
        uint8_t test_flag : 1;
    } device_status;
};
