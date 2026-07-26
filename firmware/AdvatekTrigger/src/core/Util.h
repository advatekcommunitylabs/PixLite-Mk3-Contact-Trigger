#pragma once

#include <Arduino.h>

namespace advatek {

inline void copyText(char *destination, size_t capacity, const char *source) {
  if (!destination || capacity == 0) return;
  if (!source) source = "";
  strlcpy(destination, source, capacity);
}

inline void copyText(char *destination, size_t capacity, const String &source) {
  copyText(destination, capacity, source.c_str());
}

inline bool dueAt(uint32_t now, uint32_t deadline) {
  return static_cast<int32_t>(now - deadline) >= 0;
}

inline uint8_t percentToFactor(uint8_t percent) {
  if (percent > 100) percent = 100;
  return static_cast<uint8_t>((static_cast<uint16_t>(percent) * 255U + 50U) / 100U);
}

inline uint8_t factorToPercent(uint8_t factor) {
  return static_cast<uint8_t>((static_cast<uint16_t>(factor) * 100U + 127U) / 255U);
}

inline String jsonEscape(const char *value) {
  String out;
  if (!value) return out;
  out.reserve(strlen(value) + 8);
  for (const char *p = value; *p; ++p) {
    switch (*p) {
      case '"': out += F("\\\""); break;
      case '\\': out += F("\\\\"); break;
      case '\b': out += F("\\b"); break;
      case '\f': out += F("\\f"); break;
      case '\n': out += F("\\n"); break;
      case '\r': out += F("\\r"); break;
      case '\t': out += F("\\t"); break;
      default:
        if (static_cast<uint8_t>(*p) >= 0x20) out += *p;
        break;
    }
  }
  return out;
}

inline bool parseIPv4(const char *text, IPAddress &address) {
  return text && address.fromString(text);
}

inline uint32_t crc32Bytes(const uint8_t *data, size_t length) {
  uint32_t crc = 0xFFFFFFFFUL;
  for (size_t i = 0; i < length; ++i) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; ++bit) {
      crc = (crc >> 1U) ^ (0xEDB88320UL & (0U - (crc & 1U)));
    }
  }
  return ~crc;
}

}  // namespace advatek
