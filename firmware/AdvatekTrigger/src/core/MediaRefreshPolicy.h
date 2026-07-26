#pragma once

namespace advatek {

// A successful status poll after an observed outage is the earliest reliable
// signal that a saved PixLite is reachable again. Refresh its media catalogue
// immediately so scene selectors and scene-step actions recover without the
// operator having to remove and re-add the controller.
inline bool mediaRefreshRequiredAfterPoll(
    bool previousPollOnline,
    bool currentPollSucceeded) {
  return !previousPollOnline && currentPollSucceeded;
}

}  // namespace advatek
