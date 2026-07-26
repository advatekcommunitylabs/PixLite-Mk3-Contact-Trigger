#pragma once

#include <ETH.h>
#include <SPI.h>

#include "BoardProfile.h"

namespace advatek {

// All supported Waveshare targets use Arduino-ESP32's W5500 SPI adapter. The
// board profile supplies the wiring, keeping reset and initialization policy
// shared instead of duplicating it in every target.
inline bool beginW5500SpiEthernet(const BoardProfile &profile) {
  const SpiEthernetPins &pins = profile.ethernet;
  pinMode(pins.reset, OUTPUT);
  digitalWrite(pins.reset, LOW);
  delay(50);
  digitalWrite(pins.reset, HIGH);
  delay(200);
  SPI.begin(pins.sclk, pins.miso, pins.mosi);
  return ETH.begin(
      ETH_PHY_W5500,
      pins.phyAddress,
      pins.chipSelect,
      pins.interrupt,
      pins.reset,
      SPI);
}

}  // namespace advatek
