#pragma once

#include <string.h>

namespace advatek {

// PixLite Mk3 statusRead reports programmed priority as the strings "Hi" and
// "Lo". Keep this conversion outside the JSON traversal so it remains easy to
// exercise in host tests with real-controller fixtures.
inline bool programPriorityIsHigh(const char *value) {
  return value && strcasecmp(value, "Hi") == 0;
}

}  // namespace advatek
