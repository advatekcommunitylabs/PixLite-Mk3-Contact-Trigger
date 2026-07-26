#pragma once

#include "../core/LatestAction.h"

namespace advatek {

// The GPIO loop is deliberately isolated from HTTP. This one-element mailbox
// makes the most recent physical event authoritative and prevents a slow
// PixLite request from building a stale queue.
class ActionMailbox {
 public:
  ActionMailbox() : lock_(portMUX_INITIALIZER_UNLOCKED) {}

  void publish(uint8_t inputIndex, bool activeEdge, const ActionConfig &action, uint32_t now) {
    portENTER_CRITICAL(&lock_);
    slot_.publish(inputIndex, activeEdge, action, now);
    portEXIT_CRITICAL(&lock_);
  }

  bool take(PendingAction &result, uint32_t now) {
    portENTER_CRITICAL(&lock_);
    const bool available = slot_.take(result, now);
    portEXIT_CRITICAL(&lock_);
    return available;
  }

  void clear() {
    portENTER_CRITICAL(&lock_);
    slot_.clear();
    portEXIT_CRITICAL(&lock_);
  }

 private:
  portMUX_TYPE lock_;
  LatestActionSlot slot_;
};

}  // namespace advatek
