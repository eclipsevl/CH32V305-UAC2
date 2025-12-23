#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t const* UsbDesc_Device(uint16_t* len);
uint8_t const* UsbDesc_ConfigFs(uint16_t* len);
uint8_t const* UsbDesc_ConfigHs(uint16_t* len);
uint8_t const* UsbDesc_String(uint8_t idx, uint16_t* len);
uint8_t const* UsbDesc_Hid(uint16_t idx, uint16_t* len);
uint8_t const* UsbDesc_Qualifier(uint16_t* len);
uint8_t const* UsbDesc_Uac2SampleRate(uint16_t* len);

#ifdef __cplusplus
}
#endif
