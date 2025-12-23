#pragma once
#include <array>
#include "helper.hpp"
#include "usb_class.hpp"
#include "usb_hardware.hpp"

class UsbDevice {
public:
    UsbHardware hardware;
    UsbStatus status;
    SetupRequest setup_request;

    static constexpr size_t kNumInterface = 5;
    std::array<UsbdClass*, kNumInterface> interface_class_dispatch{};
    std::array<uint8_t, kNumInterface> interface_alter{};

    static constexpr size_t kEp0BufferSize = 64;
    std::array<uint8_t, kEp0BufferSize> ep0_buffer{};

    template<class ClassDispatch>
    void AddClass(ClassDispatch& core) {
        for (auto interface_no : ClassDispatch::kUsedInterface) {
            interface_class_dispatch[interface_no] = &core;
        }
        core.Init(hardware);
    }

    void Init() {
        hardware.hal.Init();
    }

    void Connect() {
        hardware.hal.Connect();
    }

    // void EndpointSend(uint8_t ep_num, uint8_t* buffer, size_t count) {
    //     ep_num &= 0xf;
    //     EndpointContext& ep = in_endpoints[ep_num];
    //     ep.transfer_buffer = buffer;
    //     ep.transfer_remain = count;
    //     ep.transfer_count = std::min(ep.max_packet_size, count);
    //     if (ep_num != 0) {
    //         if (ep.attribute.type == EndpointType::Bulk) {
    //             ep.attribute.zero_package = (count % ep.max_packet_size == 0);
    //         }
    //         hardware.Tx(ep_num, ep.transfer_buffer, ep.transfer_count);
    //         hardware.TxAck(ep_num, true);
    //     }
    // }

    // void EndpointRecive(uint8_t ep_num, uint8_t* buffer, size_t count) {
    //     ep_num &= 0xf;
    //     EndpointContext& ep = out_endpoints[ep_num];
    //     ep.transfer_buffer = buffer;
    //     ep.transfer_remain = count;
    //     ep.transfer_count = std::min(ep.max_packet_size, count);
    //     if (ep_num != 0) {
    //         hardware.Rx(ep_num, ep.transfer_buffer, ep.transfer_count);
    //         hardware.RxAck(ep_num, true);
    //     }
    // }

    // ----------------------------------------
    // common
    // ----------------------------------------
    void Setup() noexcept {
        bool allow_operation = false;
        switch (setup_request.GetCommandType()) {
            case SetupRequest::kStandard:
                SetupStandard(allow_operation);
                break;
            case SetupRequest::kClass: {
                // qwqfixme: some callback
                uint8_t interface_no = setup_request.wIndex & 0xff;
                allow_operation = interface_no < kNumInterface;
                if (allow_operation) {
                    interface_class_dispatch[interface_no]->ClassRequest(setup_request, allow_operation);
                }
                break;
            }
            case SetupRequest::kVendor:
                // qwqfixme: some callback
                break;
        }

        if (!allow_operation) {
            hardware.hal.Stall(0);
            hardware.hal.Stall(0x80);
        }
        else {
            if (setup_request.IsDirectionIn()) {
                EndpointContext& ep0 = hardware.in_endpoints[0];
                ep0.transfer_remain = std::min<size_t>(ep0.transfer_remain, setup_request.wLength);
                ep0.transfer_count = std::min(ep0.transfer_remain, ep0.max_packet_size);
                hardware.hal.Tx(0, ep0.transfer_buffer, ep0.transfer_count);
                hardware.hal.TxAck(0, true);
            }
            else {
                EndpointContext& ep0 = hardware.out_endpoints[0];
                ep0.transfer_remain = std::min<size_t>(ep0.transfer_remain, setup_request.wLength);
                ep0.transfer_count = std::min(ep0.transfer_remain, ep0.max_packet_size);
                if (ep0.transfer_remain == 0) {
                    hardware.hal.Tx(0, nullptr, 0);
                    hardware.hal.TxAck(0, true);
                }
                else {
                    hardware.hal.Rx(0, ep0.transfer_buffer, ep0.transfer_count);
                    hardware.hal.RxAck(0, true);
                }
            }
        }
    }

    void Reset() noexcept {
        status.using_configuration = 0;
        status.device_status.self_power = 0;
        status.device_status.remote_wakeup = 0;
        status.device_status.test_flag = 0;
        hardware.hal.SetAddress(0);
    }

    void Suspend() noexcept {

    }

    void In(uint8_t ep_num) noexcept {
        EndpointContext& ep = hardware.in_endpoints[ep_num];
        ep.transfer_remain -= ep.transfer_count;
        ep.transfer_buffer += ep.transfer_count;

        if (ep.transfer_remain == 0) {
            // transfer complete
            if (ep_num == 0) {
                hardware.hal.Rx(0, nullptr, 0);
                hardware.hal.RxAck(0, true);
            }
            else {
                if (ep.attribute.zero_package) {
                    ep.attribute.zero_package = 0;
                    hardware.hal.Tx(ep_num, nullptr, 0);
                    hardware.hal.TxAckToggle(ep_num);
                }
                else {
                    ep.dispatch->EndpointInComplete(ep_num);
                }
            }
        }
        else {
            // continue
            ep.transfer_count = std::min(ep.transfer_remain, ep.max_packet_size);
            hardware.hal.Tx(ep_num, ep.transfer_buffer, ep.transfer_count);
            hardware.hal.TxAckToggle(ep_num);
        }

        if (status.device_status.test_flag) {
            status.device_status.test_flag = 0;
            hardware.hal.Test(static_cast<UsbdTest>(setup_request.wIndex));
        }
    }

    void Out(uint8_t ep_num, size_t num_recive) noexcept {
        EndpointContext& ep = hardware.out_endpoints[ep_num];
        ep.transfer_remain -= num_recive;
        ep.transfer_buffer += num_recive;

        if (ep.transfer_remain == 0) {
            // complete
            if (ep_num == 0) {
                hardware.hal.Tx(0, nullptr, 0);
                hardware.hal.TxAck(0, true);
            }
            else {
                if (ep.attribute.zero_package) {
                    ep.attribute.zero_package = 0;
                    hardware.hal.Rx(ep_num, nullptr, 0);
                    hardware.hal.RxAckToggle(ep_num);
                }
                else {
                    ep.dispatch->EndpointOutComplete(ep_num);
                }
            }
        }
        else {
            // continue
            ep.transfer_count = std::min(ep.transfer_remain, ep.max_packet_size);
            hardware.hal.Rx(ep_num, ep.transfer_buffer, ep.transfer_count);
            hardware.hal.RxAckToggle(ep_num);
        }
    }

    // ----------------------------------------
    // handler
    // ----------------------------------------
    void SetupStandard(bool& allow_operation) noexcept {
        switch (setup_request.GetStandardRequest()) {
            case SetupRequest::kSetAddress: {
                allow_operation = true;

                hardware.hal.SetAddress(setup_request.wValue);
                break;
            }
            case SetupRequest::kGetDescriptor: {
                SetupGetDescriptor(allow_operation);
                break;
            }
            case SetupRequest::kSetConfiguration: {
                allow_operation = true;

                status.using_configuration = setup_request.wValue & 0xff;
                break;
            }
            case SetupRequest::kGetConfiguration: {
                allow_operation = true;

                SimpleSerializer s{ep0_buffer.data(), 0};
                s.Append(status.using_configuration);
                hardware.EndpointSend(0, ep0_buffer.data(), s.num_write);
                break;
            }
            case SetupRequest::kSetInterfaceAlter: {
                // qwqfixme: maybe need some callback to class
                uint8_t interface_no = setup_request.wIndex & 0xff;
                uint8_t alter = setup_request.wValue & 0xff;
                allow_operation = interface_no < kNumInterface;

                if (allow_operation) {
                    interface_alter[interface_no] = alter;
                    interface_class_dispatch[interface_no]->SetInterfaceAlter(alter);
                }
                break;
            }
            case SetupRequest::kGetInterfaceAlter: {
                // qwqfixme: maybe need some callback to class
                uint8_t interface_no = setup_request.wIndex & 0xff;
                allow_operation = interface_no < kNumInterface;

                if (allow_operation) {
                    SimpleSerializer s{ep0_buffer.data(), 0};
                    s.Append<uint8_t>(interface_alter[interface_no]);
                    hardware.EndpointSend(0, ep0_buffer.data(), s.num_write);
                }
                break;
            }
            case SetupRequest::kClearEndpointFeature: {
                allow_operation = true;

                hardware.hal.ClearStall(setup_request.wIndex & 0xff);
                break;
            }
            case SetupRequest::kSetEndpointFeature: {
                allow_operation = true;

                hardware.hal.Stall(setup_request.wIndex & 0xff);
                break;
            }
            case SetupRequest::kGetEndpointStatus: {
                allow_operation = true;

                SimpleSerializer s{ep0_buffer.data(), 0};
                uint8_t ep_address = setup_request.wIndex & 0xff;
                uint8_t ep_num = ep_address & 0xf;
                bool stalled = false;
                if (ep_address & 0x80) {
                    stalled = hardware.in_endpoints[ep_num].attribute.stall;
                }
                else {
                    stalled = hardware.out_endpoints[ep_num].attribute.stall;
                }

                if (stalled) {
                    s.Append<uint16_t>(0x0001);
                }
                else {
                    s.Append<uint16_t>(0x0000);
                }
                hardware.EndpointSend(0, ep0_buffer.data(), s.num_write);
                break;
            }
            case SetupRequest::kSetDeviceFeature: {
                switch (setup_request.wValue) {
                    case 1:
                        // qwqfixme: some callback
                        status.device_status.remote_wakeup = 1;
                        allow_operation = true;
                        break;
                    case 2:
                        status.device_status.test_flag = 1;
                        allow_operation = true;
                        break;
                }
                break;
            }
            case SetupRequest::kClearDeviceFeature: {
                if (setup_request.wValue == 1) {
                    // qwqfixme: some callback
                    status.device_status.remote_wakeup = 0;
                    allow_operation = true;
                }
                break;
            }
            case SetupRequest::kGetDeviceStatus: {
                allow_operation = true;

                SimpleSerializer s{ep0_buffer.data(), 0};
                uint16_t t = 0;
                if (status.device_status.self_power) {
                    t |= 0x0001;
                }
                if (status.device_status.remote_wakeup) {
                    t |= 0x0002;
                }
                s.Append(t);
                hardware.EndpointSend(0, ep0_buffer.data(), s.num_write);
                break;
            }
            case SetupRequest::kGetInterfaceStatus: {
                allow_operation = true;

                SimpleSerializer s{ep0_buffer.data(), 0};
                s.Append<uint16_t>(0);
                hardware.EndpointSend(0, ep0_buffer.data(), s.num_write);
                break;
            }
            case SetupRequest::kSetDescriptor:
                break;
            case SetupRequest::kGetInterfaceDescriptor:
                // qwqfixme: get hid descriptor from interface here
                break;
            // not handled
            case SetupRequest::kSetInterfaceFeature:
                break;
            case SetupRequest::kClearInterfaceFeature:
                break;
            case SetupRequest::kSynchFrame:
                break;
        }
    }

    void SetupGetDescriptor(bool& allow_operation) noexcept {
        uint8_t type = setup_request.wValue >> 8;
        uint8_t index = setup_request.wValue & 0xff;
        uint8_t lang_id = setup_request.wIndex >> 8;

        BufferSpan<const uint8_t> desc;
        switch (type) {
            // device
            case 1:
                desc = descriptors.GetDevice();
                break;
            // configuration
            case 2:
                desc = descriptors.GetFullSpeedConfiguration();
                if (hardware.CheckSpeedAndIfHighSpeed()) {
                    desc = descriptors.GetHighSpeedConfiguration();
                }
                break;
            // string
            case 3:
                desc = descriptors.GetString(lang_id);
                break;
            // hid
            case 0x22:
                desc = descriptors.GetHid(index);
                break;
            // device qualifier
            case 6:
                break;
            // bos, usb2.0 does not support
            case 0xf:
                break;
            // other speed
            case 7:
                break;
        }

        allow_operation = !desc.empty();
        if (allow_operation) {
            hardware.EndpointSend(0, const_cast<uint8_t*>(desc.data()), desc.size());
        }
    }
};
