#pragma once

#include <stddef.h>

#include "Types.h"

namespace advatek {

inline char mediaOrderFoldAscii(char value) {
  return value >= 'A' && value <= 'Z'
      ? static_cast<char>(value + ('a' - 'A'))
      : value;
}

// Compare file names in the same case-insensitive, human-friendly order used
// by the UI. Number runs are compared by value, so Scene 2 precedes Scene 10.
// The comparison avoids locale tables and heap allocation on the ESP32.
inline int8_t compareMediaNames(const char *left, const char *right) {
  while (*left && *right) {
    if (*left >= '0' && *left <= '9' && *right >= '0' && *right <= '9') {
      const char *leftRun = left;
      const char *rightRun = right;
      while (*leftRun == '0') ++leftRun;
      while (*rightRun == '0') ++rightRun;

      const char *leftEnd = leftRun;
      const char *rightEnd = rightRun;
      while (*leftEnd >= '0' && *leftEnd <= '9') ++leftEnd;
      while (*rightEnd >= '0' && *rightEnd <= '9') ++rightEnd;

      const ptrdiff_t leftDigits = leftEnd - leftRun;
      const ptrdiff_t rightDigits = rightEnd - rightRun;
      if (leftDigits != rightDigits) return leftDigits < rightDigits ? -1 : 1;
      for (ptrdiff_t i = 0; i < leftDigits; ++i) {
        if (leftRun[i] != rightRun[i]) return leftRun[i] < rightRun[i] ? -1 : 1;
      }

      while (*left >= '0' && *left <= '9') ++left;
      while (*right >= '0' && *right <= '9') ++right;
      continue;
    }

    const char a = mediaOrderFoldAscii(*left++);
    const char b = mediaOrderFoldAscii(*right++);
    if (a != b) return a < b ? -1 : 1;
  }
  if (*left == *right) return 0;
  return *left ? 1 : -1;
}

// The file catalogue is bounded to 128 entries in PSRAM. Insertion sort is
// compact, deterministic, and requires only one MediaFile of working storage.
inline void sortMediaFiles(MediaFile *files, uint8_t count) {
  if (!files) return;
  for (uint8_t i = 1; i < count; ++i) {
    const MediaFile item = files[i];
    uint8_t position = i;
    while (position &&
           compareMediaNames(item.name, files[position - 1].name) < 0) {
      files[position] = files[position - 1];
      --position;
    }
    files[position] = item;
  }
}

}  // namespace advatek
