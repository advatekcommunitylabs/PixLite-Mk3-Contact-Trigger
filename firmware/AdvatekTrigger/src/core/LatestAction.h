#pragma once

#include "ActionPolicy.h"

namespace advatek {

// Synchronization is supplied by the platform wrapper. Keeping replacement
// and expiry policy here makes it host-testable and portable.
class LatestActionSlot {
 public:
  void publish(uint8_t inputIndex, bool activeEdge, const ActionConfig &action, uint32_t now) {
    pending_ = {true, inputIndex, activeEdge, now, action};
  }

  bool take(PendingAction &result, uint32_t now) {
    if (!pending_.valid) return false;
    result = pending_;
    pending_.valid = false;
    if (!actionStillFresh(result.createdAt, now)) {
      result.valid = false;
      return false;
    }
    return true;
  }

  void clear() { pending_.valid = false; }

 private:
  PendingAction pending_{};
};

}  // namespace advatek
