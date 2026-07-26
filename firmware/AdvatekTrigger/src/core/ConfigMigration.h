#pragma once

#include <stdio.h>

#include "Types.h"

namespace advatek {

inline void migrationCopyText(char *destination, size_t capacity, const char *source) {
  if (!destination || !capacity) return;
  if (!source) source = "";
  strncpy(destination, source, capacity - 1);
  destination[capacity - 1] = '\0';
}

// Frozen PixLite layout used by schemas v1-v3. Schema v4 replaces this single
// controller record with up to MAX_PIXLITES named, action-addressable records.
struct PixLiteConfigV3 {
  char macAddress[13];
  char host[64];
  uint16_t port;
  char apiVersion[8];
  char username[12];
  char password[65];
};

// Frozen action/input layout used by schemas v1-v4. Schema v5 adds an RGB
// value for Test Color, so legacy records must no longer refer to the current
// ActionConfig type or their byte layout would change with future features.
struct ActionConfigV4 {
  ActionKind kind;
  PlaybackRepeat repeat;
  OutputSelection outputSelection;
  int8_t auxOutput;
  uint8_t intensityPercent;
  uint8_t intensityStepPercent;
  uint16_t holdDelayMs;
  uint16_t repeatIntervalMs;
  uint16_t fadeMs;
  char targetId[16];
  char mediaName[64];
};

struct InputConfigV4 {
  bool enabled;
  bool normallyClosed;
  InputMode mode;
  uint16_t debounceMs;
  char name[24];
  ActionConfigV4 onAction;
  ActionConfigV4 offAction;
};

// Frozen schema-v1 structures. Keep these byte-for-byte stable: installations
// may retain a v1 NVS slot for years before being upgraded.
struct InputConfigV1 {
  bool enabled;
  uint8_t gpio;
  bool normallyClosed;
  InputMode mode;
  uint16_t debounceMs;
  char name[24];
  ActionConfigV4 onAction;
  ActionConfigV4 offAction;
};

struct AppConfigV1 {
  uint32_t magic;
  uint16_t schemaVersion;
  uint16_t reserved;
  uint32_t sequence;
  uint32_t crc32;
  char boardId[40];
  NetworkConfig network;
  PixLiteConfigV3 pixlite;
  SecurityConfig security;
  InputConfigV1 inputs[MAX_INPUTS];
};

// Frozen schema-v2 layout. Schema v3 adds StatusLedConfig without changing
// any v2 field or silently discarding an installation's existing settings.
struct HardwareBindingV2 {
  char boardId[40];
  char profileVersion[16];
  uint8_t inputGpios[MAX_INPUTS];
};

struct AppConfigV2 {
  uint32_t magic;
  uint16_t schemaVersion;
  uint16_t reserved;
  uint32_t sequence;
  uint32_t crc32;
  HardwareBindingV2 hardware;
  NetworkConfig network;
  PixLiteConfigV3 pixlite;
  SecurityConfig security;
  InputConfigV4 inputs[MAX_INPUTS];
};

// Frozen schema-v3 layout. Schema v4 adds the managed PixLite collection while
// preserving every existing action and mapping. Legacy "primary" target IDs
// continue to resolve to the migrated first controller.
struct AppConfigV3 {
  uint32_t magic;
  uint16_t schemaVersion;
  uint16_t reserved;
  uint32_t sequence;
  uint32_t crc32;
  HardwareBindingV2 hardware;
  NetworkConfig network;
  PixLiteConfigV3 pixlite;
  SecurityConfig security;
  StatusLedConfig statusLed;
  InputConfigV4 inputs[MAX_INPUTS];
};

// Frozen schema-v4 layout. Schema v5 adds Test Color to actions without
// changing controller identities, passwords, GPIO bindings, or LED settings.
struct AppConfigV4 {
  uint32_t magic;
  uint16_t schemaVersion;
  uint16_t reserved;
  uint32_t sequence;
  uint32_t crc32;
  HardwareBindingV2 hardware;
  NetworkConfig network;
  uint8_t pixliteCount;
  uint8_t primaryPixliteIndex;
  uint8_t pixliteReserved[2];
  PixLiteConfig pixlites[MAX_PIXLITES];
  SecurityConfig security;
  StatusLedConfig statusLed;
  InputConfigV4 inputs[MAX_INPUTS];
};

inline void migrateActionV4(
    const ActionConfigV4 &source,
    ActionConfig &destination) {
  memset(&destination, 0, sizeof(destination));
  destination.kind = source.kind;
  destination.repeat = source.repeat;
  destination.outputSelection = source.outputSelection;
  destination.auxOutput = source.auxOutput;
  destination.intensityPercent = source.intensityPercent;
  destination.intensityStepPercent = source.intensityStepPercent;
  destination.holdDelayMs = source.holdDelayMs;
  destination.repeatIntervalMs = source.repeatIntervalMs;
  destination.fadeMs = source.fadeMs;
  destination.testRed = 255;
  migrationCopyText(
      destination.targetId,
      sizeof(destination.targetId),
      source.targetId);
  migrationCopyText(
      destination.mediaName,
      sizeof(destination.mediaName),
      source.mediaName);
}

inline void migrateInputV4(
    const InputConfigV4 &source,
    InputConfig &destination) {
  memset(&destination, 0, sizeof(destination));
  destination.enabled = source.enabled;
  destination.normallyClosed = source.normallyClosed;
  destination.mode = source.mode;
  destination.debounceMs = source.debounceMs;
  migrationCopyText(destination.name, sizeof(destination.name), source.name);
  migrateActionV4(source.onAction, destination.onAction);
  migrateActionV4(source.offAction, destination.offAction);
}

inline void migrateInputsV4(
    const InputConfigV4 *source,
    InputConfig *destination) {
  for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
    migrateInputV4(source[i], destination[i]);
  }
}

inline void initializeMigratedPixLites(AppConfig &destination) {
  destination.pixliteCount = 0;
  destination.primaryPixliteIndex = 0;
  for (uint8_t i = 0; i < MAX_PIXLITES; ++i) {
    PixLiteConfig &target = destination.pixlites[i];
    memset(&target, 0, sizeof(target));
    target.port = 80;
    snprintf(target.id, sizeof(target.id), "pixlite-%u", i + 1);
    snprintf(target.displayName, sizeof(target.displayName), "PixLite %u", i + 1);
    migrationCopyText(target.apiVersion, sizeof(target.apiVersion), "v1.9");
    migrationCopyText(target.username, sizeof(target.username), "oper");
  }
}

inline void migrateLegacyPixLite(
    const PixLiteConfigV3 &source,
    AppConfig &destination) {
  initializeMigratedPixLites(destination);
  if (!source.host[0] && !source.macAddress[0]) return;
  PixLiteConfig &target = destination.pixlites[0];
  target.enabled = true;
  migrationCopyText(target.id, sizeof(target.id), "primary");
  migrationCopyText(target.displayName, sizeof(target.displayName), "Primary PixLite");
  migrationCopyText(target.macAddress, sizeof(target.macAddress), source.macAddress);
  migrationCopyText(target.host, sizeof(target.host), source.host);
  target.port = source.port ? source.port : 80;
  migrationCopyText(target.apiVersion, sizeof(target.apiVersion), source.apiVersion);
  migrationCopyText(target.username, sizeof(target.username), source.username);
  migrationCopyText(target.password, sizeof(target.password), source.password);
  destination.pixliteCount = 1;
}

inline bool migrationPinAllowed(
    uint8_t pin,
    const uint8_t *allowedPins,
    uint8_t allowedPinCount) {
  for (uint8_t i = 0; i < allowedPinCount; ++i) {
    if (allowedPins[i] == pin) return true;
  }
  return false;
}

// Decodes a CRC-checked stored record. Unsafe legacy pins are deliberately
// unassigned: changing a GPIO silently could energise the wrong field wiring.
inline bool migrateStoredConfig(
    const void *stored,
    size_t storedLength,
    AppConfig &destination,
    const char *boardId,
    const char *profileVersion,
    const uint8_t *allowedPins,
    uint8_t allowedPinCount,
    bool &migrated) {
  migrated = false;
  if (!stored || storedLength < sizeof(uint32_t) + 2 * sizeof(uint16_t)) return false;
  uint16_t schemaVersion = 0;
  memcpy(
      &schemaVersion,
      static_cast<const uint8_t *>(stored) + offsetof(AppConfig, schemaVersion),
      sizeof(schemaVersion));
  if (schemaVersion == CONFIG_SCHEMA_VERSION && storedLength == sizeof(AppConfig)) {
    memcpy(&destination, stored, sizeof(destination));
    if (strcmp(destination.hardware.boardId, boardId) != 0) {
      // A firmware image for another supported PCB may encounter an existing
      // NVS record or imported backup. Portable behaviour is retained, but
      // physical terminals are never guessed across different boards.
      for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
        destination.hardware.inputGpios[i] = PIN_UNASSIGNED;
      }
      migrationCopyText(
          destination.hardware.boardId,
          sizeof(destination.hardware.boardId),
          boardId);
      migrationCopyText(
          destination.hardware.profileVersion,
          sizeof(destination.hardware.profileVersion),
          profileVersion);
      destination.crc32 = 0;
      migrated = true;
    } else if (
        strcmp(destination.hardware.profileVersion, profileVersion) != 0) {
      // A board-profile update may remove physically inaccessible or newly
      // reserved pins. Never redirect wiring: retain valid assignments and
      // explicitly unassign anything no longer permitted.
      for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
        const uint8_t gpio = destination.hardware.inputGpios[i];
        if (gpio != PIN_UNASSIGNED &&
            !migrationPinAllowed(gpio, allowedPins, allowedPinCount)) {
          destination.hardware.inputGpios[i] = PIN_UNASSIGNED;
        }
      }
      migrationCopyText(
          destination.hardware.profileVersion,
          sizeof(destination.hardware.profileVersion),
          profileVersion);
      destination.crc32 = 0;
      migrated = true;
    }
    return true;
  }
  if (schemaVersion == 4 && storedLength == sizeof(AppConfigV4)) {
    const AppConfigV4 &source = *reinterpret_cast<const AppConfigV4 *>(stored);
    memset(&destination, 0, sizeof(destination));
    destination.magic = source.magic;
    destination.schemaVersion = CONFIG_SCHEMA_VERSION;
    destination.reserved = source.reserved;
    destination.sequence = source.sequence;
    memcpy(&destination.hardware, &source.hardware, sizeof(source.hardware));
    destination.network = source.network;
    destination.pixliteCount = source.pixliteCount;
    destination.primaryPixliteIndex = source.primaryPixliteIndex;
    memcpy(destination.pixliteReserved, source.pixliteReserved, sizeof(source.pixliteReserved));
    memcpy(destination.pixlites, source.pixlites, sizeof(source.pixlites));
    destination.security = source.security;
    destination.statusLed = source.statusLed;
    migrateInputsV4(source.inputs, destination.inputs);
    migrationCopyText(
        destination.hardware.profileVersion,
        sizeof(destination.hardware.profileVersion),
        profileVersion);
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      const uint8_t gpio = source.hardware.inputGpios[i];
      destination.hardware.inputGpios[i] =
          gpio != PIN_UNASSIGNED &&
                  migrationPinAllowed(gpio, allowedPins, allowedPinCount)
              ? gpio
              : PIN_UNASSIGNED;
    }
    destination.crc32 = 0;
    migrated = true;
    return true;
  }
  if (schemaVersion == 3 && storedLength == sizeof(AppConfigV3)) {
    const AppConfigV3 &source = *reinterpret_cast<const AppConfigV3 *>(stored);
    memset(&destination, 0, sizeof(destination));
    destination.magic = source.magic;
    destination.schemaVersion = CONFIG_SCHEMA_VERSION;
    destination.reserved = source.reserved;
    destination.sequence = source.sequence;
    memcpy(&destination.hardware, &source.hardware, sizeof(source.hardware));
    migrationCopyText(
        destination.hardware.profileVersion,
        sizeof(destination.hardware.profileVersion),
        profileVersion);
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      const uint8_t gpio = source.hardware.inputGpios[i];
      destination.hardware.inputGpios[i] =
          gpio != PIN_UNASSIGNED &&
                  migrationPinAllowed(gpio, allowedPins, allowedPinCount)
              ? gpio
              : PIN_UNASSIGNED;
    }
    destination.network = source.network;
    migrateLegacyPixLite(source.pixlite, destination);
    destination.security = source.security;
    destination.statusLed = source.statusLed;
    migrateInputsV4(source.inputs, destination.inputs);
    destination.crc32 = 0;
    migrated = true;
    return true;
  }
  if (schemaVersion == 2 && storedLength == sizeof(AppConfigV2)) {
    const AppConfigV2 &source = *reinterpret_cast<const AppConfigV2 *>(stored);
    memset(&destination, 0, sizeof(destination));
    destination.magic = source.magic;
    destination.schemaVersion = CONFIG_SCHEMA_VERSION;
    destination.reserved = source.reserved;
    destination.sequence = source.sequence;
    migrationCopyText(
        destination.hardware.boardId,
        sizeof(destination.hardware.boardId),
        source.hardware.boardId);
    migrationCopyText(
        destination.hardware.profileVersion,
        sizeof(destination.hardware.profileVersion),
        profileVersion);
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      const uint8_t gpio = source.hardware.inputGpios[i];
      destination.hardware.inputGpios[i] =
          gpio != PIN_UNASSIGNED &&
                  migrationPinAllowed(gpio, allowedPins, allowedPinCount)
              ? gpio
              : PIN_UNASSIGNED;
    }
    destination.network = source.network;
    migrateLegacyPixLite(source.pixlite, destination);
    destination.security = source.security;
    migrateInputsV4(source.inputs, destination.inputs);
    destination.statusLed.enabled = true;
    destination.statusLed.brightnessPercent = 100;
    destination.crc32 = 0;
    migrated = true;
    return true;
  }
  if (schemaVersion != 1 || storedLength != sizeof(AppConfigV1)) return false;

  const AppConfigV1 &legacy = *reinterpret_cast<const AppConfigV1 *>(stored);
  memset(&destination, 0, sizeof(destination));
  destination.magic = legacy.magic;
  destination.schemaVersion = CONFIG_SCHEMA_VERSION;
  destination.reserved = legacy.reserved;
  destination.sequence = legacy.sequence;
  migrationCopyText(destination.hardware.boardId, sizeof(destination.hardware.boardId), boardId);
  migrationCopyText(
      destination.hardware.profileVersion,
      sizeof(destination.hardware.profileVersion),
      profileVersion);
  destination.network = legacy.network;
  migrateLegacyPixLite(legacy.pixlite, destination);
  destination.security = legacy.security;
  destination.statusLed.enabled = true;
  destination.statusLed.brightnessPercent = 100;
  for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
    const InputConfigV1 &source = legacy.inputs[i];
    InputConfig &target = destination.inputs[i];
    target.enabled = source.enabled;
    target.normallyClosed = source.normallyClosed;
    target.mode = source.mode;
    target.debounceMs = source.debounceMs;
    memcpy(target.name, source.name, sizeof(target.name));
    migrateActionV4(source.onAction, target.onAction);
    migrateActionV4(source.offAction, target.offAction);
    destination.hardware.inputGpios[i] =
        migrationPinAllowed(source.gpio, allowedPins, allowedPinCount)
            ? source.gpio
            : PIN_UNASSIGNED;
  }
  destination.crc32 = 0;
  migrated = true;
  return true;
}

}  // namespace advatek
