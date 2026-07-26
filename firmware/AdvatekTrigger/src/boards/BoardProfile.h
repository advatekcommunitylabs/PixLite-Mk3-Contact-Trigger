#pragma once

#include <Arduino.h>

namespace advatek {

// BoardProfile is deliberately data-only so portable validation and migration
// tests can include it without pulling in a particular Ethernet driver.
enum class EthernetKind : uint8_t {
  None = 0,
  W5500ArduinoSpi = 1,
  Rmii = 2,
};

enum class InputHardwareKind : uint8_t {
  DirectGpioToGround = 0,
  IsolatedDigitalInput = 1,
};

struct SpiEthernetPins {
  int8_t miso;
  int8_t mosi;
  int8_t sclk;
  int8_t chipSelect;
  int8_t reset;
  int8_t interrupt;
  int8_t phyAddress;
};

struct RtcCapability {
  bool available;
  int8_t scl;
  int8_t sda;
  uint8_t address;
};

struct PinReservation {
  uint8_t pin;
  const char *reason;
};

struct BoardProfile {
  // Stable identifiers are persisted in configuration and release metadata.
  const char *id;
  const char *displayName;
  const char *profileVersion;

  // The platform layer invokes this callback; the concrete board header owns
  // any ETH, SPI, or RMII dependencies needed by its implementation.
  EthernetKind ethernetKind;
  SpiEthernetPins ethernet;
  InputHardwareKind inputHardwareKind;
  const char *inputTerminalPrefix;
  const uint8_t *allowedInputPins;
  uint8_t allowedInputPinCount;
  const uint8_t *cameraSharedPins;
  uint8_t cameraSharedPinCount;
  const PinReservation *reservedPins;
  uint8_t reservedPinCount;
  uint8_t maximumInputs;
  int8_t recoveryButtonPin;
  int8_t statusLedPin;
  bool requiresPsram;
  uint8_t minimumFlashMb;
  uint8_t minimumPsramMb;
  RtcCapability rtc;
  bool (*beginEthernet)(const BoardProfile &profile);
};

inline bool pinAllowed(const BoardProfile &profile, uint8_t pin) {
  for (uint8_t i = 0; i < profile.allowedInputPinCount; ++i) {
    if (profile.allowedInputPins[i] == pin) return true;
  }
  return false;
}

inline bool pinCameraShared(const BoardProfile &profile, uint8_t pin) {
  for (uint8_t i = 0; i < profile.cameraSharedPinCount; ++i) {
    if (profile.cameraSharedPins[i] == pin) return true;
  }
  return false;
}

}  // namespace advatek
