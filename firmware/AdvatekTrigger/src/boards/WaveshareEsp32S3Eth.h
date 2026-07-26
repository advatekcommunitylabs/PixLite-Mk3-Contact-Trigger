#pragma once

#include "BoardProfile.h"
#include "W5500SpiBoard.h"

namespace advatek {

// These eight contacts remain reachable on the outside headers with the
// Waveshare PoE daughterboard fitted.
inline constexpr uint8_t WAVESHARE_INPUT_PINS[] = {1, 2, 15, 16, 18, 38, 39, 40};
inline constexpr uint8_t WAVESHARE_CAMERA_SHARED_PINS[] = {1, 2, 15, 18, 38, 39, 40};
inline constexpr PinReservation WAVESHARE_RESERVED_PINS[] = {
    {0, "BOOT and credential recovery"},
    {3, "ESP32-S3 strapping pin"},
    {4, "TF card chip select"},
    {5, "TF card MISO"},
    {6, "TF card MOSI"},
    {7, "TF card clock"},
    {9, "W5500 reset"},
    {10, "W5500 interrupt"},
    {11, "W5500 MOSI"},
    {12, "W5500 MISO"},
    {13, "W5500 clock"},
    {14, "W5500 chip select"},
    {17, "Obstructed by the fitted PoE daughterboard"},
    {19, "USB D-"},
    {20, "USB D+"},
    {21, "Onboard RGB LED"},
    {33, "Internally occupied by octal PSRAM"},
    {34, "Internally occupied by octal PSRAM"},
    {35, "Internally occupied by octal PSRAM"},
    {36, "Internally occupied by octal PSRAM"},
    {37, "Internally occupied by octal PSRAM"},
    {43, "UART transmit"},
    {44, "UART receive"},
    {45, "ESP32-S3 strapping pin"},
    {46, "ESP32-S3 strapping pin"},
    {47, "Obstructed by the fitted PoE daughterboard"},
};

inline const BoardProfile WAVESHARE_ESP32_S3_ETH = {
    "waveshare-esp32-s3-eth",
    "Waveshare ESP32-S3-ETH",
    "1.2.0",
    EthernetKind::W5500ArduinoSpi,
    {12, 11, 13, 14, 9, 10, 1},
    InputHardwareKind::DirectGpioToGround,
    "",
    WAVESHARE_INPUT_PINS,
    static_cast<uint8_t>(sizeof(WAVESHARE_INPUT_PINS) / sizeof(WAVESHARE_INPUT_PINS[0])),
    WAVESHARE_CAMERA_SHARED_PINS,
    static_cast<uint8_t>(
        sizeof(WAVESHARE_CAMERA_SHARED_PINS) / sizeof(WAVESHARE_CAMERA_SHARED_PINS[0])),
    WAVESHARE_RESERVED_PINS,
    static_cast<uint8_t>(sizeof(WAVESHARE_RESERVED_PINS) / sizeof(WAVESHARE_RESERVED_PINS[0])),
    8,
    0,
    21,
    true,
    16,
    8,
    {false, -1, -1, 0},
    beginW5500SpiEthernet,
};

}  // namespace advatek
