#pragma once

#include <stdio.h>

#include "Types.h"
#include "PixLiteTargets.h"
#include "../boards/BoardProfile.h"

namespace advatek {

struct ValidationResult {
  bool valid;
  char message[160];
};

inline void validationMessage(
    ValidationResult &result,
    const char *message) {
  result.valid = false;
  snprintf(result.message, sizeof(result.message), "%s", message ? message : "");
}

inline bool hostnameCharacterIsAlphanumeric(char value) {
  return (value >= 'a' && value <= 'z') ||
         (value >= 'A' && value <= 'Z') ||
         (value >= '0' && value <= '9');
}

inline bool validMdnsHostname(const char *hostname) {
  if (!hostname) return false;
  const size_t length = strlen(hostname);
  if (length == 0 || length > 31) return false;
  if (!hostnameCharacterIsAlphanumeric(hostname[0]) ||
      !hostnameCharacterIsAlphanumeric(hostname[length - 1])) {
    return false;
  }
  for (size_t i = 0; i < length; ++i) {
    if (!hostnameCharacterIsAlphanumeric(hostname[i]) && hostname[i] != '-') {
      return false;
    }
  }
  return true;
}

inline ValidationResult validateConfig(const AppConfig &config, const BoardProfile &board) {
  ValidationResult result{true, ""};
  if (strcmp(config.hardware.boardId, board.id) != 0) {
    validationMessage(
        result,
        "Configuration belongs to another board; remap hardware before applying.");
    return result;
  }
  if (!validMdnsHostname(config.network.hostname)) {
    validationMessage(
        result,
        "Hostname must be 1-31 letters, numbers or hyphens, and cannot start or end with a hyphen.");
    return result;
  }
  if (config.pixliteCount > MAX_PIXLITES) {
    validationMessage(result, "No more than 16 PixLites can be configured.");
    return result;
  }
  for (uint8_t i = 0; i < config.pixliteCount; ++i) {
    const PixLiteConfig &pixlite = config.pixlites[i];
    if (!pixlite.enabled || !pixlite.id[0] || !pixlite.host[0]) {
      result.valid = false;
      snprintf(result.message, sizeof(result.message), "PixLite %u is incomplete.", i + 1);
      return result;
    }
    for (uint8_t other = i + 1; other < config.pixliteCount; ++other) {
      if (strcmp(pixlite.id, config.pixlites[other].id) == 0) {
        validationMessage(result, "PixLite target IDs must be unique.");
        return result;
      }
    }
  }
  for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
    const InputConfig &input = config.inputs[i];
    if (!input.enabled) continue;
    const uint8_t gpio = config.hardware.inputGpios[i];
    if (gpio == PIN_UNASSIGNED) {
      result.valid = false;
      snprintf(result.message, sizeof(result.message), "Input %u needs a GPIO assignment.", i + 1);
      return result;
    }
    if (!pinAllowed(board, gpio)) {
      result.valid = false;
      snprintf(result.message, sizeof(result.message), "Input %u uses GPIO %u, which is not permitted by this board profile.", i + 1, gpio);
      return result;
    }
    if (input.debounceMs < 10 || input.debounceMs > 2000) {
      result.valid = false;
      snprintf(result.message, sizeof(result.message), "Input %u debounce must be between 10 and 2000 ms.", i + 1);
      return result;
    }
    const ActionConfig *actions[] = {&input.onAction, &input.offAction};
    for (const ActionConfig *action : actions) {
      if (action->kind != ActionKind::None &&
          pixLiteIndexById(config, action->targetId) < 0) {
        result.valid = false;
        snprintf(
            result.message,
            sizeof(result.message),
            "Input %u action refers to a PixLite that is not configured.",
            i + 1);
        return result;
      }
    }
    for (uint8_t other = i + 1; other < MAX_INPUTS; ++other) {
      if (config.inputs[other].enabled && config.hardware.inputGpios[other] == gpio) {
        result.valid = false;
        snprintf(result.message, sizeof(result.message), "Inputs %u and %u share GPIO %u.", i + 1, other + 1, gpio);
        return result;
      }
    }
  }
  return result;
}

inline bool configurationNeedsRemap(const AppConfig &config, const BoardProfile &board) {
  if (strcmp(config.hardware.boardId, board.id) != 0) return true;
  for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
    if (!config.inputs[i].enabled) continue;
    const uint8_t gpio = config.hardware.inputGpios[i];
    if (gpio == PIN_UNASSIGNED || !pinAllowed(board, gpio)) return true;
  }
  return false;
}

}  // namespace advatek
