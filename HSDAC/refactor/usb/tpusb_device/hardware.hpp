#pragma once
#include "endpoint.hpp"

enum class UsbdTest {
    Test_J = 0x0100,
    Test_K = 0x0200,
    Test_SE0_NAK = 0x0300,
    Test_Packet = 0x0400
};

struct Hardware {
    void Init();
    void Connect();
    void DisConnect();
    void SetAddress(uint8_t address);
    void Test(UsbdTest test);

    void OpenEndpoint(
        uint8_t address,
        EndpointType ep_type,
        uint16_t max_packet_size
    );
    void CloseEndpoint(uint8_t address);

    [[nodiscard]]
    bool CheckSpeedAndIfHighSpeed();

    void Stall(uint8_t ep_address);
    void ClearStall(uint8_t ep_address);

    void TxAck(uint8_t ep_num, bool data1);
    void TxAckToggle(uint8_t ep_num);
    void TxNak(uint8_t ep_num);
    void Tx(uint8_t ep_num, uint8_t const* address, uint32_t length);

    void RxAck(uint8_t ep_num, bool data1);
    void RxAckToggle(uint8_t ep_num);
    void RxNak(uint8_t ep_num);
    void Rx(uint8_t ep_num, uint8_t* address, uint32_t length);
};
