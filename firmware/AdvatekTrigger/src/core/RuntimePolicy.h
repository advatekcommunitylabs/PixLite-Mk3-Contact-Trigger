#pragma once

#include <stddef.h>
#include <stdint.h>

namespace advatek {

enum class MemoryMode : uint8_t { Normal = 0, DegradedRecovery = 1 };

inline MemoryMode selectMemoryMode(bool psramPresent, bool allAllocationsSucceeded) {
  return psramPresent && allAllocationsSucceeded
             ? MemoryMode::Normal
             : MemoryMode::DegradedRecovery;
}

inline bool payloadWithinLimit(size_t length, size_t limit) {
  return length <= limit;
}

struct BoundedRetryState {
  uint8_t maximumAttempts;
  uint8_t attempts;
  bool succeeded;

  bool canAttempt() const { return !succeeded && attempts < maximumAttempts; }

  void record(bool success) {
    if (!canAttempt()) return;
    ++attempts;
    succeeded = success;
  }
};

}  // namespace advatek
