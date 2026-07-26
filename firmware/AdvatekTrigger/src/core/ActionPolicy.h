#pragma once

#include "Types.h"

namespace advatek {

inline bool actionStillFresh(uint32_t createdAt, uint32_t now) {
  return static_cast<uint32_t>(now - createdAt) <= ACTION_TTL_MS;
}

inline uint8_t nextRampPercent(uint8_t current, uint8_t step, bool darker) {
  const int next = static_cast<int>(current) + (darker ? -step : step);
  if (next < 0) return 0;
  if (next > 100) return 100;
  return static_cast<uint8_t>(next);
}

}  // namespace advatek
