#pragma once

#include "Types.h"

namespace advatek {

// Target lookup is shared by GPIO dispatch, the HTTP API and validation. The
// legacy "primary" identifier remains valid after schema migration so existing
// installations do not need every action rewritten during an upgrade.
inline int8_t pixLiteIndexById(const AppConfig &config, const char *id) {
  if (config.pixliteCount == 0) return -1;
  if (!id || !id[0] || strcmp(id, "primary") == 0) {
    return config.primaryPixliteIndex < config.pixliteCount
               ? static_cast<int8_t>(config.primaryPixliteIndex)
               : 0;
  }
  for (uint8_t i = 0; i < config.pixliteCount && i < MAX_PIXLITES; ++i) {
    if (config.pixlites[i].enabled && strcmp(config.pixlites[i].id, id) == 0) {
      return static_cast<int8_t>(i);
    }
  }
  return -1;
}

inline int8_t pixLiteIndexByMac(const AppConfig &config, const char *mac) {
  if (!mac || !mac[0]) return -1;
  for (uint8_t i = 0; i < config.pixliteCount && i < MAX_PIXLITES; ++i) {
    if (strcasecmp(config.pixlites[i].macAddress, mac) == 0) {
      return static_cast<int8_t>(i);
    }
  }
  return -1;
}

inline int8_t pixLiteIndexByHost(const AppConfig &config, const char *host) {
  if (!host || !host[0]) return -1;
  for (uint8_t i = 0; i < config.pixliteCount && i < MAX_PIXLITES; ++i) {
    if (strcasecmp(config.pixlites[i].host, host) == 0) {
      return static_cast<int8_t>(i);
    }
  }
  return -1;
}

}  // namespace advatek
