#pragma once

#include "BoardProfile.h"
#include "W5500SpiBoard.h"

namespace advatek {

// The terminal numbers follow the enclosure labels. Waveshare's isolated
// input stage presents an active-low signal to the ESP32, matching the shared
// contact engine's pull-up and polarity handling.
inline constexpr uint8_t WAVESHARE_8DI_INPUT_PINS[] = {
    4, 5, 6, 7, 8, 9, 10, 11};
inline constexpr PinReservation WAVESHARE_8DI_RESERVED_PINS[] = {
    {0, "BOOT and credential recovery"},
    {4, "Isolated digital input DI1"},
    {5, "Isolated digital input DI2"},
    {6, "Isolated digital input DI3"},
    {7, "Isolated digital input DI4"},
    {8, "Isolated digital input DI5"},
    {9, "Isolated digital input DI6"},
    {10, "Isolated digital input DI7"},
    {11, "Isolated digital input DI8"},
    {12, "W5500 interrupt"},
    {13, "W5500 MOSI"},
    {14, "W5500 MISO"},
    {15, "W5500 clock"},
    {16, "W5500 chip select"},
    {17, "Isolated RS485 transmit"},
    {18, "Isolated RS485 receive"},
    {19, "USB D-"},
    {20, "USB D+"},
    {35, "Internally occupied by octal PSRAM"},
    {36, "Internally occupied by octal PSRAM"},
    {37, "Internally occupied by octal PSRAM"},
    {38, "Onboard RGB LED"},
    {39, "W5500 reset"},
    {41, "RTC and relay-expander I2C clock"},
    {42, "RTC and relay-expander I2C data"},
    {43, "UART transmit"},
    {44, "UART receive"},
    {45, "TF card MISO"},
    {46, "Onboard buzzer and ESP32-S3 strapping pin"},
    {47, "TF card MOSI"},
    {48, "TF card clock"},
};

inline const BoardProfile WAVESHARE_ESP32_S3_ETH_8DI_8RO = {
    "waveshare-esp32-s3-eth-8di-8ro",
    "Waveshare ESP32-S3-ETH-8DI-8RO",
    "1.0.0",
    EthernetKind::W5500ArduinoSpi,
    {14, 13, 15, 16, 39, 12, 1},
    InputHardwareKind::IsolatedDigitalInput,
    "DI",
    WAVESHARE_8DI_INPUT_PINS,
    static_cast<uint8_t>(
        sizeof(WAVESHARE_8DI_INPUT_PINS) /
        sizeof(WAVESHARE_8DI_INPUT_PINS[0])),
    nullptr,
    0,
    WAVESHARE_8DI_RESERVED_PINS,
    static_cast<uint8_t>(
        sizeof(WAVESHARE_8DI_RESERVED_PINS) /
        sizeof(WAVESHARE_8DI_RESERVED_PINS[0])),
    8,
    0,
    38,
    true,
    16,
    8,
    {true, 41, 42, 0x51},
    beginW5500SpiEthernet,
};

}  // namespace advatek
