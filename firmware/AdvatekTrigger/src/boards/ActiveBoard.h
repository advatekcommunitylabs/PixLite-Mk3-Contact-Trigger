#pragma once

#include "WaveshareEsp32S3Eth.h"
#include "WaveshareEsp32S3Eth8Di8Ro.h"

namespace advatek {

// Generated Arduino sketches define exactly one target macro. The canonical
// maintainer build defaults to the original ESP32-S3-ETH for compatibility.
inline const BoardProfile &activeBoardProfile() {
#if defined(ADVATEK_BOARD_WAVESHARE_ESP32_S3_ETH_8DI_8RO)
  return WAVESHARE_ESP32_S3_ETH_8DI_8RO;
#else
  return WAVESHARE_ESP32_S3_ETH;
#endif
}

}  // namespace advatek
