#pragma once

#include <mbedtls/sha256.h>

#include "../core/Types.h"
#include "../core/Util.h"

namespace advatek {

inline void sha256Hex(const uint8_t *data, size_t length, char output[65]) {
  uint8_t digest[32];
  mbedtls_sha256(data, length, digest, 0);
  for (uint8_t i = 0; i < 32; ++i) snprintf(output + i * 2, 3, "%02x", digest[i]);
  output[64] = '\0';
}

inline String sha256Base64Url(const char *value) {
  static constexpr char alphabet[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  uint8_t digest[32];
  mbedtls_sha256(
      reinterpret_cast<const uint8_t *>(value ? value : ""),
      value ? strlen(value) : 0,
      digest,
      0);
  String encoded;
  encoded.reserve(43);
  uint32_t buffer = 0;
  uint8_t bits = 0;
  for (uint8_t byte : digest) {
    buffer = (buffer << 8U) | byte;
    bits += 8;
    while (bits >= 6) {
      bits -= 6;
      encoded += alphabet[(buffer >> bits) & 0x3F];
    }
  }
  if (bits) encoded += alphabet[(buffer << (6 - bits)) & 0x3F];
  return encoded;
}

inline void setWebPassword(SecurityConfig &security, const char *password) {
  if (!password || !*password) {
    memset(&security, 0, sizeof(security));
    return;
  }
  uint32_t randomA = esp_random();
  uint32_t randomB = esp_random();
  snprintf(
      security.webPasswordSalt,
      sizeof(security.webPasswordSalt),
      "%08lx%08lx",
      static_cast<unsigned long>(randomA),
      static_cast<unsigned long>(randomB));
  String salted = String(security.webPasswordSalt) + password;
  sha256Hex(
      reinterpret_cast<const uint8_t *>(salted.c_str()),
      salted.length(),
      security.webPasswordHash);
}

inline bool verifyWebPassword(const SecurityConfig &security, const char *password) {
  if (!security.webPasswordHash[0]) return true;
  String salted = String(security.webPasswordSalt) + (password ? password : "");
  char hash[65];
  sha256Hex(reinterpret_cast<const uint8_t *>(salted.c_str()), salted.length(), hash);
  uint8_t difference = 0;
  for (uint8_t i = 0; i < 64; ++i) difference |= hash[i] ^ security.webPasswordHash[i];
  return difference == 0;
}

}  // namespace advatek
