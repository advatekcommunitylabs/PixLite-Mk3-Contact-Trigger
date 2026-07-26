#pragma once

#include "../core/Types.h"
#include "../core/Util.h"
#include "MemoryResources.h"

namespace advatek {

class Diagnostics {
 public:
  explicit Diagnostics(MemoryResources &memory) : memory_(memory) {}

  void begin() {
    portENTER_CRITICAL(&lock_);
    entries_ = memory_.logs ? memory_.logs : fallback_;
    capacity_ = memory_.logs ? DIAGNOSTIC_CAPACITY : FALLBACK_CAPACITY;
    next_ = 0;
    count_ = 0;
    portEXIT_CRITICAL(&lock_);
  }

  void add(LogLevel level, const char *message) {
    const uint32_t timestamp = millis();
    portENTER_CRITICAL(&lock_);
    LogEntry &entry = entries_[next_];
    entry.timestampMs = timestamp;
    entry.level = level;
    copyText(entry.message, sizeof(entry.message), message);
    next_ = (next_ + 1U) % capacity_;
    if (count_ < capacity_) ++count_;
    portEXIT_CRITICAL(&lock_);
    // Serial is a diagnostic mirror only. Never place credentials or other
    // secrets in log messages because this output is intentionally plaintext.
    Serial.printf(
        "[%10lu] %-7s %s\r\n",
        static_cast<unsigned long>(timestamp),
        levelName(level),
        message ? message : "");
  }

  void addf(LogLevel level, const char *format, ...) {
    char message[160];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    add(level, message);
  }

  String json() {
    String result = "[";
    portENTER_CRITICAL(&lock_);
    for (uint8_t i = 0; i < count_; ++i) {
      const uint8_t index = (next_ + capacity_ - count_ + i) % capacity_;
      if (i) result += ',';
      result += "{\"timeMs\":" + String(entries_[index].timestampMs) +
                ",\"level\":\"" + levelName(entries_[index].level) +
                "\",\"message\":\"" + jsonEscape(entries_[index].message) + "\"}";
    }
    portEXIT_CRITICAL(&lock_);
    result += ']';
    return result;
  }

  uint8_t count() const { return count_; }

  bool snapshot(uint8_t chronologicalIndex, LogEntry &destination) {
    portENTER_CRITICAL(&lock_);
    if (chronologicalIndex >= count_) {
      portEXIT_CRITICAL(&lock_);
      return false;
    }
    const uint8_t index =
        (next_ + capacity_ - count_ + chronologicalIndex) % capacity_;
    destination = entries_[index];
    portEXIT_CRITICAL(&lock_);
    return true;
  }

  static const char *levelName(LogLevel level) {
    if (level == LogLevel::Warning) return "warning";
    if (level == LogLevel::Error) return "error";
    return "info";
  }

 private:
  static constexpr uint8_t FALLBACK_CAPACITY = 8;

  MemoryResources &memory_;
  portMUX_TYPE lock_ = portMUX_INITIALIZER_UNLOCKED;
  LogEntry fallback_[FALLBACK_CAPACITY]{};
  LogEntry *entries_ = fallback_;
  uint8_t capacity_ = FALLBACK_CAPACITY;
  uint8_t next_ = 0;
  uint8_t count_ = 0;
};

}  // namespace advatek
