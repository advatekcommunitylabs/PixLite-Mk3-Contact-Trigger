#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace advatek {

inline constexpr char ADAR_MAGIC[] = "DiscProt";
inline constexpr uint16_t ADAR_DISCOVERY_REQUEST = 0x1201;
inline constexpr uint16_t ADAR_DISCOVERY_RESPONSE = 0x2102;
inline constexpr uint16_t ADAR_VERSION_1_1 = 0x0101;
inline constexpr uint16_t ADAR_PORT = 49151;
inline constexpr char ADAR_DEVICE_MULTICAST[] = "239.255.251.1";
inline constexpr char ADAR_CLIENT_MULTICAST[] = "239.255.251.2";
inline constexpr uint32_t PIXLITE_MK3_FAMILY = 0x50697833UL;  // ASCII "Pix3"
inline constexpr size_t ADAR_DISCOVERY_BASE_SIZE = 34;

inline void writeBe16(uint8_t *out, uint16_t value) {
  out[0] = static_cast<uint8_t>(value >> 8U);
  out[1] = static_cast<uint8_t>(value);
}

inline void writeBe32(uint8_t *out, uint32_t value) {
  out[0] = static_cast<uint8_t>(value >> 24U);
  out[1] = static_cast<uint8_t>(value >> 16U);
  out[2] = static_cast<uint8_t>(value >> 8U);
  out[3] = static_cast<uint8_t>(value);
}

inline uint16_t readBe16(const uint8_t *in) {
  return static_cast<uint16_t>(in[0] << 8U) | in[1];
}

inline bool parseMac12(const char *text, uint8_t out[6]) {
  if (!text || strlen(text) != 12) return false;
  for (uint8_t i = 0; i < 6; ++i) {
    char pair[3] = {text[i * 2], text[i * 2 + 1], '\0'};
    char *end = nullptr;
    const long value = strtol(pair, &end, 16);
    if (end != pair + 2 || value < 0 || value > 255) return false;
    out[i] = static_cast<uint8_t>(value);
  }
  return true;
}

inline size_t buildAdarDiscoveryRequest(
    uint8_t *out,
    size_t capacity,
    const char excluded[][13],
    uint8_t excludedCount,
    const uint8_t rangeStart[6] = nullptr,
    const uint8_t rangeEnd[6] = nullptr) {
  if (!out || excludedCount > 50) return 0;
  const size_t required = ADAR_DISCOVERY_BASE_SIZE + static_cast<size_t>(excludedCount) * 6U;
  if (capacity < required) return 0;
  memcpy(out, ADAR_MAGIC, 8);
  writeBe16(out + 8, ADAR_DISCOVERY_REQUEST);
  writeBe16(out + 10, ADAR_VERSION_1_1);
  writeBe32(out + 12, 0xFFFFFFFFUL);
  writeBe32(out + 16, 0xFFFFFFFFUL);
  memset(out + 20, 0, 6);
  memset(out + 26, 0xFF, 6);
  if (rangeStart) memcpy(out + 20, rangeStart, 6);
  if (rangeEnd) memcpy(out + 26, rangeEnd, 6);
  writeBe16(out + 32, excludedCount);
  for (uint8_t i = 0; i < excludedCount; ++i) {
    if (!parseMac12(excluded[i], out + 34 + i * 6)) return 0;
  }
  return required;
}

inline bool validAdarResponseHeader(const uint8_t *data, size_t length) {
  return data && length > 12 && memcmp(data, ADAR_MAGIC, 8) == 0 &&
         readBe16(data + 8) == ADAR_DISCOVERY_RESPONSE &&
         readBe16(data + 10) == ADAR_VERSION_1_1 && data[12] == '{';
}

}  // namespace advatek
