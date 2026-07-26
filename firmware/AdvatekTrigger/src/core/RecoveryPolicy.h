#pragma once

#include <stdint.h>

namespace advatek {

// Holding BOOT moves through three visible, release-to-confirm zones. Keeping
// the button held for 20 seconds cancels the pending destructive operation.
enum class RecoveryIntent : uint8_t {
  None = 0,
  ClearAuthentication = 1,
  FactoryReset = 2,
  Cancelled = 3,
};

inline RecoveryIntent recoveryIntentForDuration(uint32_t heldMs) {
  if (heldMs >= 20000) return RecoveryIntent::Cancelled;
  if (heldMs >= 15000) return RecoveryIntent::FactoryReset;
  if (heldMs >= 5000) return RecoveryIntent::ClearAuthentication;
  return RecoveryIntent::None;
}

// A DHCP server must never be introduced onto an already-linked venue LAN.
// Direct recovery is armed with the cable out, then one computer is connected.
inline bool directEthernetRecoveryAllowed(bool ethernetLinkUp) {
  return !ethernetLinkUp;
}

}  // namespace advatek
