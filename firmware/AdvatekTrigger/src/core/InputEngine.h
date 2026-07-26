#pragma once

#include "Types.h"

namespace advatek {

struct InputEvent {
  bool occurred;
  bool active;
};

class InputEngine {
 public:
  static void initialize(InputRuntime &runtime, bool active, uint32_t now) {
    runtime.initialized = true;
    runtime.lastRawActive = active;
    runtime.stableActive = active;
    runtime.rawChangedAt = now;
    runtime.rampActive = false;
    runtime.rampDueAt = 0;
    runtime.rampPercent = 100;
  }

  static InputEvent update(
      InputRuntime &runtime,
      bool rawActive,
      uint16_t debounceMs,
      uint32_t now) {
    if (!runtime.initialized) {
      initialize(runtime, rawActive, now);
      return {false, rawActive};
    }
    if (rawActive != runtime.lastRawActive) {
      runtime.lastRawActive = rawActive;
      runtime.rawChangedAt = now;
    }
    if (rawActive != runtime.stableActive &&
        static_cast<uint32_t>(now - runtime.rawChangedAt) >= debounceMs) {
      runtime.stableActive = rawActive;
      return {true, rawActive};
    }
    return {false, runtime.stableActive};
  }
};

}  // namespace advatek
