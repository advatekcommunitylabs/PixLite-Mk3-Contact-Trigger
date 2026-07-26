#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "Versions.h"

namespace advatek {

inline constexpr uint8_t MAX_INPUTS = 8;
inline constexpr uint8_t MAX_PIXLITES = 16;
inline constexpr uint8_t MAX_DISCOVERED_DEVICES = 64;
inline constexpr uint8_t MAX_MEDIA_FILES = 128;
inline constexpr uint8_t MAX_OUTPUTS = 9;
inline constexpr uint8_t PIN_UNASSIGNED = 0xFF;
inline constexpr uint32_t ACTION_TTL_MS = 2000;
// Status polling is round-robin across configured PixLites. One serialized
// request per second keeps the local web interface responsive even at the
// 16-device limit.
inline constexpr uint32_t STATUS_POLL_MS = 2000;
// Media changes much less often than playback state. Refreshing once a minute
// keeps scene-step actions current without adding noticeable PixLite traffic.
inline constexpr uint32_t MEDIA_REFRESH_MS = 60000;
inline constexpr uint32_t MEDIA_RETRY_MS = 30000;

enum class UplinkMode : uint8_t { Ethernet = 0, WifiStation = 1 };
enum class IpMode : uint8_t { Dhcp = 0, Static = 1 };
// Stored values retain their schema-v1 byte layout. AlwaysOn is now interpreted
// as direct-Ethernet recovery; the other legacy values select Wi-Fi recovery.
enum class AccessPointMode : uint8_t { Disabled = 0, AutoOff = 1, AlwaysOn = 2 };
enum class InputMode : uint8_t { Momentary = 0, Maintained = 1 };
enum class ActionKind : uint8_t {
  None = 0,
  PlayScene = 1,
  PlayPlaylist = 2,
  StopPlayback = 3,
  BlankOutputs = 4,
  IntensityBrighter = 5,
  IntensityDarker = 6,
  SetIntensity = 7,
  ReleaseIntensityOverride = 8,
  TestColor = 9,
  NextScene = 10,
  PreviousScene = 11,
};
enum class PlaybackRepeat : uint8_t { Once = 0, Forever = 1 };
enum class OutputSelection : uint8_t { Pixels = 0, Aux = 1, PixelsAndAllAux = 2 };
enum class LogLevel : uint8_t { Info = 0, Warning = 1, Error = 2 };

struct IPv4Config {
  IpMode mode;
  char address[16];
  char netmask[16];
  char gateway[16];
  char dns[16];
};

struct NetworkConfig {
  UplinkMode uplink;
  IPv4Config ethernet;
  IPv4Config wifi;
  char wifiSsid[64];
  char wifiPassword[64];
  AccessPointMode accessPointMode;
  char accessPointPassword[64];
  char hostname[32];
};

inline bool usesEthernetRecovery(const NetworkConfig &network) {
  return network.accessPointMode == AccessPointMode::AlwaysOn;
}

struct ActionConfig {
  ActionKind kind;
  PlaybackRepeat repeat;
  OutputSelection outputSelection;
  int8_t auxOutput;
  uint8_t intensityPercent;
  uint8_t intensityStepPercent;
  uint16_t holdDelayMs;
  uint16_t repeatIntervalMs;
  uint16_t fadeMs;
  // Test mode uses an explicit 8-bit RGB colour. PixLite applies it to all
  // pixel outputs; a later Live Mode action returns the controller to data.
  uint8_t testRed;
  uint8_t testGreen;
  uint8_t testBlue;
  char targetId[16];
  char mediaName[64];
};

struct InputConfig {
  bool enabled;
  bool normallyClosed;
  InputMode mode;
  uint16_t debounceMs;
  char name[24];
  ActionConfig onAction;
  ActionConfig offAction;
};

struct HardwareBinding {
  char boardId[40];
  char profileVersion[16];
  uint8_t inputGpios[MAX_INPUTS];
};

struct PixLiteConfig {
  bool enabled;
  char id[16];
  char displayName[32];
  char macAddress[13];
  char host[64];
  uint16_t port;
  char apiVersion[8];
  char username[12];
  char password[65];
};

struct SecurityConfig {
  char webPasswordSalt[17];
  char webPasswordHash[65];
};

struct StatusLedConfig {
  bool enabled;
  uint8_t brightnessPercent;
};

struct AppConfig {
  uint32_t magic;
  uint16_t schemaVersion;
  uint16_t reserved;
  uint32_t sequence;
  uint32_t crc32;
  HardwareBinding hardware;
  NetworkConfig network;
  uint8_t pixliteCount;
  uint8_t primaryPixliteIndex;
  uint8_t pixliteReserved[2];
  PixLiteConfig pixlites[MAX_PIXLITES];
  SecurityConfig security;
  StatusLedConfig statusLed;
  InputConfig inputs[MAX_INPUTS];
};

struct InputRuntime {
  bool initialized;
  bool lastRawActive;
  bool stableActive;
  uint32_t rawChangedAt;
  bool rampActive;
  uint32_t rampDueAt;
  uint8_t rampPercent;
};

struct PendingAction {
  bool valid;
  uint8_t inputIndex;
  bool activeEdge;
  uint32_t createdAt;
  ActionConfig action;
};

struct DiscoveredDevice {
  char productName[64];
  char familyName[32];
  char nickname[64];
  char macAddress[13];
  char ipAddress[16];
  char firmwareVersion[64];
  bool authRequired;
  bool emergencyRecovery;
  bool viaBroadcast;
  uint32_t productFamily;
};

struct MediaFile {
  char name[64];
  bool playlist;
};

struct OutputIntensity {
  char output[12];
  uint8_t programmedFactor;
  bool highPriority;
};

struct PixLiteStatus {
  bool online;
  char mode[16];
  char currentFile[64];
  char lastError[128];
  uint16_t httpStatus;
  uint16_t latencyMs;
  uint8_t outputCount;
  OutputIntensity outputs[MAX_OUTPUTS];
};

struct LogEntry {
  uint32_t timestampMs;
  LogLevel level;
  char message[160];
};

}  // namespace advatek
