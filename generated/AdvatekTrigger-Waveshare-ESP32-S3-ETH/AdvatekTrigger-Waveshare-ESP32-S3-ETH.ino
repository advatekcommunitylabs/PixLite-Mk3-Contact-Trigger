/*
  Advatek PixLite Mk3 Contact Closure Trigger
  Generated board artifact: Waveshare ESP32-S3-ETH
  Board profile: waveshare-esp32-s3-eth
  Arduino-ESP32 core: >=3.3.10 <4
  Source revision: development

  This file is generated from the canonical modular source. Changes made here
  will be overwritten. See the repository README and PORTING.md.

  SPDX-License-Identifier: MIT
*/

#define ADVATEK_BOARD_WAVESHARE_ESP32_S3_ETH 1
#include <Arduino.h>

// =============================================================================
// Module: src/core/Versions.h
// =============================================================================
namespace advatek {

inline constexpr const char *FIRMWARE_VERSION = "1.0.0-beta.6";
inline constexpr const char *WEB_UI_VERSION = "1.0.0-beta.6";
inline constexpr uint16_t CONFIG_SCHEMA_VERSION = 5;
inline constexpr const char *SOURCE_REVISION = "development";

}  // namespace advatek

// =============================================================================
// Module: src/core/Types.h
// =============================================================================
#include <stddef.h>
#include <stdint.h>
#include <string.h>

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
  TestColorFade = 12,
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
  // Monotonic edge counter lets slow web clients observe every debounced
  // make/break transition without coupling GPIO scanning to HTTP polling.
  uint32_t eventSequence;
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
  // PixLite reports no current file after Live/Stop. Keep the last scene
  // separately so Next/Previous continues from where the operator left off.
  // Status records live in PSRAM, so this does not consume scarce static DRAM.
  char lastScene[64];
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

// =============================================================================
// Module: src/core/MediaOrderPolicy.h
// =============================================================================
#include <stddef.h>


namespace advatek {

inline char mediaOrderFoldAscii(char value) {
  return value >= 'A' && value <= 'Z'
      ? static_cast<char>(value + ('a' - 'A'))
      : value;
}

// Compare file names in the same case-insensitive, human-friendly order used
// by the UI. Number runs are compared by value, so Scene 2 precedes Scene 10.
// The comparison avoids locale tables and heap allocation on the ESP32.
inline int8_t compareMediaNames(const char *left, const char *right) {
  while (*left && *right) {
    if (*left >= '0' && *left <= '9' && *right >= '0' && *right <= '9') {
      const char *leftRun = left;
      const char *rightRun = right;
      while (*leftRun == '0') ++leftRun;
      while (*rightRun == '0') ++rightRun;

      const char *leftEnd = leftRun;
      const char *rightEnd = rightRun;
      while (*leftEnd >= '0' && *leftEnd <= '9') ++leftEnd;
      while (*rightEnd >= '0' && *rightEnd <= '9') ++rightEnd;

      const ptrdiff_t leftDigits = leftEnd - leftRun;
      const ptrdiff_t rightDigits = rightEnd - rightRun;
      if (leftDigits != rightDigits) return leftDigits < rightDigits ? -1 : 1;
      for (ptrdiff_t i = 0; i < leftDigits; ++i) {
        if (leftRun[i] != rightRun[i]) return leftRun[i] < rightRun[i] ? -1 : 1;
      }

      while (*left >= '0' && *left <= '9') ++left;
      while (*right >= '0' && *right <= '9') ++right;
      continue;
    }

    const char a = mediaOrderFoldAscii(*left++);
    const char b = mediaOrderFoldAscii(*right++);
    if (a != b) return a < b ? -1 : 1;
  }
  if (*left == *right) return 0;
  return *left ? 1 : -1;
}

// The file catalogue is bounded to 128 entries in PSRAM. Insertion sort is
// compact, deterministic, and requires only one MediaFile of working storage.
inline void sortMediaFiles(MediaFile *files, uint8_t count) {
  if (!files) return;
  for (uint8_t i = 1; i < count; ++i) {
    const MediaFile item = files[i];
    uint8_t position = i;
    while (position &&
           compareMediaNames(item.name, files[position - 1].name) < 0) {
      files[position] = files[position - 1];
      --position;
    }
    files[position] = item;
  }
}

}  // namespace advatek

// =============================================================================
// Module: src/core/SceneStepPolicy.h
// =============================================================================
namespace advatek {

// PixLite file names are case-preserving, but matching them case-insensitively
// makes stepping robust if a status response changes only the filename case.
inline bool sceneStepNameEquals(const char *left, const char *right) {
  if (!left || !right) return false;
  while (*left && *right) {
    char a = *left++;
    char b = *right++;
    if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + ('a' - 'A'));
    if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + ('a' - 'A'));
    if (a != b) return false;
  }
  return *left == '\0' && *right == '\0';
}

inline bool sceneStepIsSceneName(const char *name) {
  if (!name) return false;
  const char *end = name;
  while (*end) ++end;
  if (end - name < 4) return false;
  const char *suffix = end - 4;
  return suffix[0] == '.' &&
         (suffix[1] == 's' || suffix[1] == 'S') &&
         (suffix[2] == 'c' || suffix[2] == 'C') &&
         (suffix[3] == 'n' || suffix[3] == 'N');
}

// Live mode intentionally has no current file. Retain the most recently
// selected scene as the cursor so a Stop/Live action between button presses
// does not make Next restart at the first file. A playing playlist is not a
// scene cursor and therefore also falls back to the last scene.
inline const char *sceneStepReference(
    const char *currentFile,
    const char *lastScene) {
  if (sceneStepIsSceneName(currentFile)) return currentFile;
  return lastScene ? lastScene : "";
}

// Returns an index into the controller's cached file-list order, skipping
// playlists. If no scene is active, Next starts at the first scene and
// Previous starts at the last. Both directions wrap at their respective end.
inline int16_t sceneStepIndex(
    const MediaFile *files,
    uint8_t fileCount,
    const char *currentFile,
    bool previous) {
  if (!files || !fileCount) return -1;
  int16_t firstScene = -1;
  int16_t lastScene = -1;
  int16_t currentScene = -1;
  for (uint8_t i = 0; i < fileCount; ++i) {
    if (files[i].playlist) continue;
    if (firstScene < 0) firstScene = i;
    lastScene = i;
    if (sceneStepNameEquals(files[i].name, currentFile)) currentScene = i;
  }
  if (firstScene < 0) return -1;
  if (currentScene < 0) return previous ? lastScene : firstScene;

  for (uint16_t offset = 1; offset <= fileCount; ++offset) {
    const int16_t candidate = previous
        ? static_cast<int16_t>(
              (currentScene + fileCount - (offset % fileCount)) % fileCount)
        : static_cast<int16_t>((currentScene + offset) % fileCount);
    if (!files[candidate].playlist) return candidate;
  }
  return currentScene;
}

}  // namespace advatek

// =============================================================================
// Module: src/core/MediaRefreshPolicy.h
// =============================================================================
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

// =============================================================================
// Module: src/core/Util.h
// =============================================================================
#include <Arduino.h>

namespace advatek {

inline void copyText(char *destination, size_t capacity, const char *source) {
  if (!destination || capacity == 0) return;
  if (!source) source = "";
  strlcpy(destination, source, capacity);
}

inline void copyText(char *destination, size_t capacity, const String &source) {
  copyText(destination, capacity, source.c_str());
}

inline bool dueAt(uint32_t now, uint32_t deadline) {
  return static_cast<int32_t>(now - deadline) >= 0;
}

inline uint8_t percentToFactor(uint8_t percent) {
  if (percent > 100) percent = 100;
  return static_cast<uint8_t>((static_cast<uint16_t>(percent) * 255U + 50U) / 100U);
}

inline uint8_t factorToPercent(uint8_t factor) {
  return static_cast<uint8_t>((static_cast<uint16_t>(factor) * 100U + 127U) / 255U);
}

inline String jsonEscape(const char *value) {
  String out;
  if (!value) return out;
  out.reserve(strlen(value) + 8);
  for (const char *p = value; *p; ++p) {
    switch (*p) {
      case '"': out += F("\\\""); break;
      case '\\': out += F("\\\\"); break;
      case '\b': out += F("\\b"); break;
      case '\f': out += F("\\f"); break;
      case '\n': out += F("\\n"); break;
      case '\r': out += F("\\r"); break;
      case '\t': out += F("\\t"); break;
      default:
        if (static_cast<uint8_t>(*p) >= 0x20) out += *p;
        break;
    }
  }
  return out;
}

inline bool parseIPv4(const char *text, IPAddress &address) {
  return text && address.fromString(text);
}

inline uint32_t crc32Bytes(const uint8_t *data, size_t length) {
  uint32_t crc = 0xFFFFFFFFUL;
  for (size_t i = 0; i < length; ++i) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; ++bit) {
      crc = (crc >> 1U) ^ (0xEDB88320UL & (0U - (crc & 1U)));
    }
  }
  return ~crc;
}

}  // namespace advatek

// =============================================================================
// Module: src/core/JsonLite.h
// =============================================================================
#include <Arduino.h>

/*
  JsonLite is a small, allocation-free JSON tokenizer derived from the design
  of Serge Zaitsev's MIT-licensed jsmn parser. It intentionally implements only
  tokenization and typed lookups; the caller owns all storage.
*/

namespace advatek {

enum class JsonTokenType : uint8_t {
  Undefined = 0,
  Object = 1,
  Array = 2,
  String = 3,
  Primitive = 4,
};

struct JsonToken {
  JsonTokenType type;
  int16_t start;
  int16_t end;
  int16_t size;
  int16_t parent;
};

class JsonDocument {
 public:
  JsonDocument(const char *json, JsonToken *tokens, size_t capacity)
      : json_(json), tokens_(tokens), capacity_(capacity), count_(0) {}

  bool parse() {
    count_ = 0;
    int16_t parent = -1;
    for (int16_t position = 0; json_ && json_[position]; ++position) {
      const char c = json_[position];
      if (c == '{' || c == '[') {
        const int16_t index = allocate(c == '{' ? JsonTokenType::Object : JsonTokenType::Array, position, parent);
        if (index < 0) return false;
        if (parent >= 0) tokens_[parent].size++;
        parent = index;
      } else if (c == '}' || c == ']') {
        const JsonTokenType expected = c == '}' ? JsonTokenType::Object : JsonTokenType::Array;
        if (parent < 0 || tokens_[parent].type != expected) return false;
        tokens_[parent].end = position + 1;
        parent = tokens_[parent].parent;
      } else if (c == '"') {
        const int16_t start = position + 1;
        bool escaped = false;
        for (++position; json_[position]; ++position) {
          if (!escaped && json_[position] == '"') break;
          if (!escaped && json_[position] == '\\') escaped = true;
          else escaped = false;
        }
        if (!json_[position]) return false;
        const int16_t index = allocate(JsonTokenType::String, start, parent);
        if (index < 0) return false;
        tokens_[index].end = position;
        if (parent >= 0) tokens_[parent].size++;
      } else if (isWhitespace(c) || c == ':' || c == ',') {
        continue;
      } else {
        const int16_t start = position;
        while (json_[position] && !isDelimiter(json_[position])) ++position;
        const int16_t index = allocate(JsonTokenType::Primitive, start, parent);
        if (index < 0) return false;
        tokens_[index].end = position;
        if (parent >= 0) tokens_[parent].size++;
        --position;
      }
    }
    return parent == -1 && count_ > 0;
  }

  int16_t root() const { return count_ ? 0 : -1; }
  int16_t count() const { return count_; }
  const JsonToken &token(int16_t index) const { return tokens_[index]; }

  bool equals(int16_t index, const char *value) const {
    if (!valid(index) || !value) return false;
    const size_t length = strlen(value);
    const JsonToken &t = tokens_[index];
    return static_cast<size_t>(t.end - t.start) == length &&
           strncmp(json_ + t.start, value, length) == 0;
  }

  int16_t objectValue(int16_t objectIndex, const char *key) const {
    if (!valid(objectIndex) || tokens_[objectIndex].type != JsonTokenType::Object) return -1;
    int16_t index = objectIndex + 1;
    while (index < count_ && tokens_[index].start < tokens_[objectIndex].end) {
      if (tokens_[index].parent == objectIndex &&
          tokens_[index].type == JsonTokenType::String &&
          equals(index, key)) {
        const int16_t valueIndex = index + 1;
        return valid(valueIndex) && tokens_[valueIndex].parent == objectIndex ? valueIndex : -1;
      }
      index = skip(index);
    }
    return -1;
  }

  int16_t arrayItem(int16_t arrayIndex, int16_t itemIndex) const {
    if (!valid(arrayIndex) || tokens_[arrayIndex].type != JsonTokenType::Array || itemIndex < 0) return -1;
    int16_t seen = 0;
    int16_t index = arrayIndex + 1;
    while (index < count_ && tokens_[index].start < tokens_[arrayIndex].end) {
      if (tokens_[index].parent == arrayIndex) {
        if (seen++ == itemIndex) return index;
      }
      index = skip(index);
    }
    return -1;
  }

  int16_t arrayLength(int16_t arrayIndex) const {
    if (!valid(arrayIndex) || tokens_[arrayIndex].type != JsonTokenType::Array) return 0;
    int16_t length = 0;
    for (int16_t index = arrayIndex + 1;
         index < count_ && tokens_[index].start < tokens_[arrayIndex].end;
         index = skip(index)) {
      if (tokens_[index].parent == arrayIndex) length++;
    }
    return length;
  }

  bool stringValue(int16_t index, char *destination, size_t capacity) const {
    if (!valid(index) || tokens_[index].type != JsonTokenType::String || !destination || capacity == 0) return false;
    const JsonToken &t = tokens_[index];
    size_t out = 0;
    for (int16_t i = t.start; i < t.end && out + 1 < capacity; ++i) {
      char c = json_[i];
      if (c == '\\' && i + 1 < t.end) {
        const char escaped = json_[++i];
        switch (escaped) {
          case '"': c = '"'; break;
          case '\\': c = '\\'; break;
          case '/': c = '/'; break;
          case 'b': c = '\b'; break;
          case 'f': c = '\f'; break;
          case 'n': c = '\n'; break;
          case 'r': c = '\r'; break;
          case 't': c = '\t'; break;
          default: c = '?'; break;
        }
      }
      destination[out++] = c;
    }
    destination[out] = '\0';
    return true;
  }

  String stringValue(int16_t index) const {
    if (!valid(index) || tokens_[index].type != JsonTokenType::String) return String();
    const JsonToken &t = tokens_[index];
    String value;
    value.reserve(t.end - t.start);
    for (int16_t i = t.start; i < t.end; ++i) {
      char c = json_[i];
      if (c == '\\' && i + 1 < t.end) {
        const char escaped = json_[++i];
        if (escaped == 'n') c = '\n';
        else if (escaped == 'r') c = '\r';
        else if (escaped == 't') c = '\t';
        else c = escaped;
      }
      value += c;
    }
    return value;
  }

  long integerValue(int16_t index, long fallback = 0) const {
    if (!valid(index) || tokens_[index].type != JsonTokenType::Primitive) return fallback;
    char buffer[24];
    const int16_t tokenLength = tokens_[index].end - tokens_[index].start;
    const int16_t length =
        tokenLength < static_cast<int16_t>(sizeof(buffer) - 1)
            ? tokenLength
            : static_cast<int16_t>(sizeof(buffer) - 1);
    memcpy(buffer, json_ + tokens_[index].start, length);
    buffer[length] = '\0';
    char *end = nullptr;
    const long result = strtol(buffer, &end, 10);
    return end == buffer ? fallback : result;
  }

  bool booleanValue(int16_t index, bool fallback = false) const {
    if (!valid(index)) return fallback;
    if (equals(index, "true")) return true;
    if (equals(index, "false")) return false;
    return fallback;
  }

  int16_t skip(int16_t index) const {
    if (!valid(index)) return count_;
    const int16_t end = tokens_[index].end;
    ++index;
    while (index < count_ && tokens_[index].start < end) ++index;
    return index;
  }

 private:
  static bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
  }

  static bool isDelimiter(char c) {
    return isWhitespace(c) || c == ',' || c == ']' || c == '}';
  }

  bool valid(int16_t index) const { return index >= 0 && index < count_; }

  int16_t allocate(JsonTokenType type, int16_t start, int16_t parent) {
    if (count_ >= static_cast<int16_t>(capacity_) || start < 0) return -1;
    JsonToken &token = tokens_[count_];
    token.type = type;
    token.start = start;
    token.end = INT16_MAX;
    token.size = 0;
    token.parent = parent;
    return count_++;
  }

  const char *json_;
  JsonToken *tokens_;
  size_t capacity_;
  int16_t count_;
};

}  // namespace advatek

// =============================================================================
// Module: src/core/PixLiteResponsePolicy.h
// =============================================================================
namespace advatek {

struct PixLiteApiError {
  bool present = false;
  int code = 0;
  char message[96]{};
};

/*
  PixLite API errors are carried in a top-level JSON "err" object. The HTTP
  status can still be 200, so callers must check both transport and API
  status before reporting success.
*/
inline bool inspectPixLiteResponse(
    const char *body,
    JsonToken *tokens,
    size_t tokenCapacity,
    PixLiteApiError &error) {
  error = PixLiteApiError{};
  JsonDocument document(body, tokens, tokenCapacity);
  if (!document.parse() ||
      document.token(document.root()).type != JsonTokenType::Object) {
    return false;
  }

  const int16_t errorObject = document.objectValue(document.root(), "err");
  if (errorObject < 0) return true;
  if (document.token(errorObject).type != JsonTokenType::Object) return false;

  error.present = true;
  error.code = static_cast<int>(
      document.integerValue(document.objectValue(errorObject, "code"), 0));
  const int16_t message = document.objectValue(errorObject, "msg");
  if (message >= 0) {
    document.stringValue(message, error.message, sizeof(error.message));
  }
  return true;
}

}  // namespace advatek

// =============================================================================
// Module: src/core/RuntimePolicy.h
// =============================================================================
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

// =============================================================================
// Module: src/core/RecoveryPolicy.h
// =============================================================================
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

// =============================================================================
// Module: src/core/PixLiteStatusPolicy.h
// =============================================================================
#include <string.h>

namespace advatek {

// PixLite Mk3 statusRead reports programmed priority as the strings "Hi" and
// "Lo". Keep this conversion outside the JSON traversal so it remains easy to
// exercise in host tests with real-controller fixtures.
inline bool programPriorityIsHigh(const char *value) {
  return value && strcasecmp(value, "Hi") == 0;
}

}  // namespace advatek

// =============================================================================
// Module: src/boards/BoardProfile.h
// =============================================================================
#include <Arduino.h>

namespace advatek {

// BoardProfile is deliberately data-only so portable validation and migration
// tests can include it without pulling in a particular Ethernet driver.
enum class EthernetKind : uint8_t {
  None = 0,
  W5500ArduinoSpi = 1,
  Rmii = 2,
};

enum class InputHardwareKind : uint8_t {
  DirectGpioToGround = 0,
  IsolatedDigitalInput = 1,
};

// Addressable RGB LEDs do not all transmit colour bytes in the same order.
// Keep this in the board profile so shared status behaviour continues to use
// logical red/green/blue values on every supported PCB.
enum class StatusLedColorOrder : uint8_t {
  Rgb = 0,
  Grb = 1,
};

struct SpiEthernetPins {
  int8_t miso;
  int8_t mosi;
  int8_t sclk;
  int8_t chipSelect;
  int8_t reset;
  int8_t interrupt;
  int8_t phyAddress;
};

struct RtcCapability {
  bool available;
  int8_t scl;
  int8_t sda;
  uint8_t address;
};

struct PinReservation {
  uint8_t pin;
  const char *reason;
};

struct BoardProfile {
  // Stable identifiers are persisted in configuration and release metadata.
  const char *id;
  const char *displayName;
  const char *profileVersion;

  // The platform layer invokes this callback; the concrete board header owns
  // any ETH, SPI, or RMII dependencies needed by its implementation.
  EthernetKind ethernetKind;
  SpiEthernetPins ethernet;
  InputHardwareKind inputHardwareKind;
  const char *inputTerminalPrefix;
  const uint8_t *allowedInputPins;
  uint8_t allowedInputPinCount;
  const uint8_t *cameraSharedPins;
  uint8_t cameraSharedPinCount;
  const PinReservation *reservedPins;
  uint8_t reservedPinCount;
  uint8_t maximumInputs;
  int8_t recoveryButtonPin;
  int8_t statusLedPin;
  StatusLedColorOrder statusLedColorOrder;
  bool requiresPsram;
  uint8_t minimumFlashMb;
  uint8_t minimumPsramMb;
  RtcCapability rtc;
  bool (*beginEthernet)(const BoardProfile &profile);
};

inline bool pinAllowed(const BoardProfile &profile, uint8_t pin) {
  for (uint8_t i = 0; i < profile.allowedInputPinCount; ++i) {
    if (profile.allowedInputPins[i] == pin) return true;
  }
  return false;
}

inline bool pinCameraShared(const BoardProfile &profile, uint8_t pin) {
  for (uint8_t i = 0; i < profile.cameraSharedPinCount; ++i) {
    if (profile.cameraSharedPins[i] == pin) return true;
  }
  return false;
}

}  // namespace advatek

// =============================================================================
// Module: src/boards/W5500SpiBoard.h
// =============================================================================
#include <ETH.h>
#include <SPI.h>


namespace advatek {

// All supported Waveshare targets use Arduino-ESP32's W5500 SPI adapter. The
// board profile supplies the wiring, keeping reset and initialization policy
// shared instead of duplicating it in every target.
inline bool beginW5500SpiEthernet(const BoardProfile &profile) {
  const SpiEthernetPins &pins = profile.ethernet;
  pinMode(pins.reset, OUTPUT);
  digitalWrite(pins.reset, LOW);
  delay(50);
  digitalWrite(pins.reset, HIGH);
  delay(200);
  SPI.begin(pins.sclk, pins.miso, pins.mosi);
  return ETH.begin(
      ETH_PHY_W5500,
      pins.phyAddress,
      pins.chipSelect,
      pins.interrupt,
      pins.reset,
      SPI);
}

}  // namespace advatek

// =============================================================================
// Module: src/boards/WaveshareEsp32S3Eth.h
// =============================================================================
namespace advatek {

// These eight contacts remain reachable on the outside headers with the
// Waveshare PoE daughterboard fitted.
inline constexpr uint8_t WAVESHARE_INPUT_PINS[] = {1, 2, 15, 16, 18, 38, 39, 40};
inline constexpr uint8_t WAVESHARE_CAMERA_SHARED_PINS[] = {1, 2, 15, 18, 38, 39, 40};
inline constexpr PinReservation WAVESHARE_RESERVED_PINS[] = {
    {0, "BOOT and credential recovery"},
    {3, "ESP32-S3 strapping pin"},
    {4, "TF card chip select"},
    {5, "TF card MISO"},
    {6, "TF card MOSI"},
    {7, "TF card clock"},
    {9, "W5500 reset"},
    {10, "W5500 interrupt"},
    {11, "W5500 MOSI"},
    {12, "W5500 MISO"},
    {13, "W5500 clock"},
    {14, "W5500 chip select"},
    {17, "Obstructed by the fitted PoE daughterboard"},
    {19, "USB D-"},
    {20, "USB D+"},
    {21, "Onboard RGB LED"},
    {33, "Internally occupied by octal PSRAM"},
    {34, "Internally occupied by octal PSRAM"},
    {35, "Internally occupied by octal PSRAM"},
    {36, "Internally occupied by octal PSRAM"},
    {37, "Internally occupied by octal PSRAM"},
    {43, "UART transmit"},
    {44, "UART receive"},
    {45, "ESP32-S3 strapping pin"},
    {46, "ESP32-S3 strapping pin"},
    {47, "Obstructed by the fitted PoE daughterboard"},
};

inline const BoardProfile WAVESHARE_ESP32_S3_ETH = {
    "waveshare-esp32-s3-eth",
    "Waveshare ESP32-S3-ETH",
    "1.2.0",
    EthernetKind::W5500ArduinoSpi,
    {12, 11, 13, 14, 9, 10, 1},
    InputHardwareKind::DirectGpioToGround,
    "",
    WAVESHARE_INPUT_PINS,
    static_cast<uint8_t>(sizeof(WAVESHARE_INPUT_PINS) / sizeof(WAVESHARE_INPUT_PINS[0])),
    WAVESHARE_CAMERA_SHARED_PINS,
    static_cast<uint8_t>(
        sizeof(WAVESHARE_CAMERA_SHARED_PINS) / sizeof(WAVESHARE_CAMERA_SHARED_PINS[0])),
    WAVESHARE_RESERVED_PINS,
    static_cast<uint8_t>(sizeof(WAVESHARE_RESERVED_PINS) / sizeof(WAVESHARE_RESERVED_PINS[0])),
    8,
    0,
    21,
    StatusLedColorOrder::Grb,
    true,
    16,
    8,
    {false, -1, -1, 0},
    beginW5500SpiEthernet,
};

}  // namespace advatek

// =============================================================================
// Module: src/boards/WaveshareEsp32S3Eth8Di8Ro.h
// =============================================================================
namespace advatek {

// The terminal numbers follow the enclosure labels. Waveshare's isolated
// input stage presents an active-low signal to the ESP32, matching the shared
// contact engine's pull-up and polarity handling.
inline constexpr uint8_t WAVESHARE_8DI_INPUT_PINS[] = {
    4, 5, 6, 7, 8, 9, 10, 11};
inline constexpr PinReservation WAVESHARE_8DI_RESERVED_PINS[] = {
    {0, "BOOT and credential recovery"},
    {4, "Isolated digital input DI1"},
    {5, "Isolated digital input DI2"},
    {6, "Isolated digital input DI3"},
    {7, "Isolated digital input DI4"},
    {8, "Isolated digital input DI5"},
    {9, "Isolated digital input DI6"},
    {10, "Isolated digital input DI7"},
    {11, "Isolated digital input DI8"},
    {12, "W5500 interrupt"},
    {13, "W5500 MOSI"},
    {14, "W5500 MISO"},
    {15, "W5500 clock"},
    {16, "W5500 chip select"},
    {17, "Isolated RS485 transmit"},
    {18, "Isolated RS485 receive"},
    {19, "USB D-"},
    {20, "USB D+"},
    {35, "Internally occupied by octal PSRAM"},
    {36, "Internally occupied by octal PSRAM"},
    {37, "Internally occupied by octal PSRAM"},
    {38, "Onboard RGB LED"},
    {39, "W5500 reset"},
    {41, "RTC and relay-expander I2C clock"},
    {42, "RTC and relay-expander I2C data"},
    {43, "UART transmit"},
    {44, "UART receive"},
    {45, "TF card MISO"},
    {46, "Onboard buzzer and ESP32-S3 strapping pin"},
    {47, "TF card MOSI"},
    {48, "TF card clock"},
};

inline const BoardProfile WAVESHARE_ESP32_S3_ETH_8DI_8RO = {
    "waveshare-esp32-s3-eth-8di-8ro",
    "Waveshare ESP32-S3-ETH-8DI-8RO",
    "1.1.0",
    EthernetKind::W5500ArduinoSpi,
    {14, 13, 15, 16, 39, 12, 1},
    InputHardwareKind::IsolatedDigitalInput,
    "DI",
    WAVESHARE_8DI_INPUT_PINS,
    static_cast<uint8_t>(
        sizeof(WAVESHARE_8DI_INPUT_PINS) /
        sizeof(WAVESHARE_8DI_INPUT_PINS[0])),
    nullptr,
    0,
    WAVESHARE_8DI_RESERVED_PINS,
    static_cast<uint8_t>(
        sizeof(WAVESHARE_8DI_RESERVED_PINS) /
        sizeof(WAVESHARE_8DI_RESERVED_PINS[0])),
    8,
    0,
    38,
    StatusLedColorOrder::Rgb,
    true,
    16,
    8,
    {true, 41, 42, 0x51},
    beginW5500SpiEthernet,
};

}  // namespace advatek

// =============================================================================
// Module: src/boards/ActiveBoard.h
// =============================================================================
namespace advatek {

// Generated Arduino sketches define exactly one target macro. The canonical
// maintainer build defaults to the original ESP32-S3-ETH for compatibility.
inline const BoardProfile &activeBoardProfile() {
#if defined(ADVATEK_BOARD_WAVESHARE_ESP32_S3_ETH_8DI_8RO)
  return WAVESHARE_ESP32_S3_ETH_8DI_8RO;
#else
  return WAVESHARE_ESP32_S3_ETH;
#endif
}

}  // namespace advatek

// =============================================================================
// Module: src/core/Defaults.h
// =============================================================================
namespace advatek {

inline ActionConfig defaultAction() {
  ActionConfig action{};
  action.kind = ActionKind::None;
  // Continuous playback is the least surprising contact-closure behaviour:
  // the scene or playlist remains active until a later Stop/Live-mode action.
  action.repeat = PlaybackRepeat::Forever;
  action.outputSelection = OutputSelection::Pixels;
  action.auxOutput = 1;
  action.intensityPercent = 100;
  action.intensityStepPercent = 5;
  action.holdDelayMs = 400;
  action.repeatIntervalMs = 250;
  action.fadeMs = 250;
  action.testRed = 255;
  action.testGreen = 0;
  action.testBlue = 0;
  copyText(action.targetId, sizeof(action.targetId), "primary");
  return action;
}

inline IPv4Config defaultIPv4() {
  IPv4Config config{};
  config.mode = IpMode::Dhcp;
  copyText(config.address, sizeof(config.address), "192.168.0.200");
  copyText(config.netmask, sizeof(config.netmask), "255.255.255.0");
  copyText(config.gateway, sizeof(config.gateway), "192.168.0.1");
  copyText(config.dns, sizeof(config.dns), "192.168.0.1");
  return config;
}

inline void setDefaultPixLite(PixLiteConfig &pixlite, uint8_t index) {
  memset(&pixlite, 0, sizeof(pixlite));
  pixlite.port = 80;
  snprintf(pixlite.id, sizeof(pixlite.id), "pixlite-%u", index + 1);
  snprintf(pixlite.displayName, sizeof(pixlite.displayName), "PixLite %u", index + 1);
  copyText(pixlite.apiVersion, sizeof(pixlite.apiVersion), "v1.9");
  copyText(pixlite.username, sizeof(pixlite.username), "oper");
}

inline void setDefaultConfig(AppConfig &config, const BoardProfile &board, const char *hostname) {
  memset(&config, 0, sizeof(config));
  config.magic = 0x41544354UL;  // "ATCT"
  config.schemaVersion = CONFIG_SCHEMA_VERSION;
  config.sequence = 1;
  copyText(config.hardware.boardId, sizeof(config.hardware.boardId), board.id);
  copyText(
      config.hardware.profileVersion,
      sizeof(config.hardware.profileVersion),
      board.profileVersion);
  config.network.uplink = UplinkMode::Ethernet;
  config.network.ethernet = defaultIPv4();
  config.network.wifi = defaultIPv4();
  // Commission over Ethernet. Wi-Fi is an explicit operational uplink, while
  // the device AP is reserved for time-limited BOOT recovery.
  config.network.accessPointMode = AccessPointMode::Disabled;
  copyText(config.network.hostname, sizeof(config.network.hostname), hostname);
  config.pixliteCount = 0;
  config.primaryPixliteIndex = 0;
  for (uint8_t i = 0; i < MAX_PIXLITES; ++i) {
    setDefaultPixLite(config.pixlites[i], i);
  }
  config.statusLed.enabled = true;
  config.statusLed.brightnessPercent = 100;
  for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
    InputConfig &input = config.inputs[i];
    input.enabled = false;
    config.hardware.inputGpios[i] =
        i < board.allowedInputPinCount ? board.allowedInputPins[i] : PIN_UNASSIGNED;
    input.normallyClosed = false;
    input.mode = InputMode::Momentary;
    input.debounceMs = 100;
    snprintf(input.name, sizeof(input.name), "Input %u", i + 1);
    input.onAction = defaultAction();
    input.offAction = defaultAction();
  }
}

}  // namespace advatek

// =============================================================================
// Module: src/core/ConfigMigration.h
// =============================================================================
#include <stdio.h>


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

// =============================================================================
// Module: src/core/PixLiteTargets.h
// =============================================================================
namespace advatek {

// Target lookup is shared by GPIO dispatch, the HTTP API and validation. The
// legacy "primary" identifier remains valid after schema migration so existing
// installations do not need every action rewritten during an upgrade.
inline int8_t pixLiteIndexById(const AppConfig &config, const char *id) {
  if (config.pixliteCount == 0) return -1;
  if (!id || !id[0] || strcmp(id, "primary") == 0) {
    return config.primaryPixliteIndex < config.pixliteCount
               ? static_cast<int8_t>(config.primaryPixliteIndex)
               : 0;
  }
  for (uint8_t i = 0; i < config.pixliteCount && i < MAX_PIXLITES; ++i) {
    if (config.pixlites[i].enabled && strcmp(config.pixlites[i].id, id) == 0) {
      return static_cast<int8_t>(i);
    }
  }
  return -1;
}

inline int8_t pixLiteIndexByMac(const AppConfig &config, const char *mac) {
  if (!mac || !mac[0]) return -1;
  for (uint8_t i = 0; i < config.pixliteCount && i < MAX_PIXLITES; ++i) {
    if (strcasecmp(config.pixlites[i].macAddress, mac) == 0) {
      return static_cast<int8_t>(i);
    }
  }
  return -1;
}

inline int8_t pixLiteIndexByHost(const AppConfig &config, const char *host) {
  if (!host || !host[0]) return -1;
  for (uint8_t i = 0; i < config.pixliteCount && i < MAX_PIXLITES; ++i) {
    if (strcasecmp(config.pixlites[i].host, host) == 0) {
      return static_cast<int8_t>(i);
    }
  }
  return -1;
}

}  // namespace advatek

// =============================================================================
// Module: src/core/Validation.h
// =============================================================================
#include <stdio.h>




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

// =============================================================================
// Module: src/core/InputEngine.h
// =============================================================================
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
      ++runtime.eventSequence;
      return {true, rawActive};
    }
    return {false, runtime.stableActive};
  }
};

}  // namespace advatek

// =============================================================================
// Module: src/core/ActionPolicy.h
// =============================================================================
namespace advatek {

inline bool actionStillFresh(uint32_t createdAt, uint32_t now) {
  return static_cast<uint32_t>(now - createdAt) <= ACTION_TTL_MS;
}

inline uint8_t nextRampPercent(uint8_t current, uint8_t step, bool darker) {
  const int next = static_cast<int>(current) + (darker ? -step : step);
  if (next < 0) return 0;
  if (next > 100) return 100;
  return static_cast<uint8_t>(next);
}

}  // namespace advatek

// =============================================================================
// Module: src/core/LatestAction.h
// =============================================================================
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

// =============================================================================
// Module: src/core/AdarPacket.h
// =============================================================================
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace advatek {

inline constexpr char ADAR_MAGIC[] = "DiscProt";
inline constexpr uint16_t ADAR_DISCOVERY_REQUEST = 0x1201;
inline constexpr uint16_t ADAR_DISCOVERY_RESPONSE = 0x2102;
inline constexpr uint16_t ADAR_VERSION_1_1 = 0x0101;
inline constexpr uint16_t ADAR_PORT = 49151;
inline constexpr char ADAR_DEVICE_MULTICAST[] = "239.255.251.1";
inline constexpr char ADAR_CLIENT_MULTICAST[] = "239.255.251.2";
inline constexpr uint32_t PIXLITE_MK3_FAMILY = 0x50697833UL;  // ASCII "Pix3"
inline constexpr size_t ADAR_DISCOVERY_BASE_SIZE = 34;

inline void writeBe16(uint8_t *out, uint16_t value) {
  out[0] = static_cast<uint8_t>(value >> 8U);
  out[1] = static_cast<uint8_t>(value);
}

inline void writeBe32(uint8_t *out, uint32_t value) {
  out[0] = static_cast<uint8_t>(value >> 24U);
  out[1] = static_cast<uint8_t>(value >> 16U);
  out[2] = static_cast<uint8_t>(value >> 8U);
  out[3] = static_cast<uint8_t>(value);
}

inline uint16_t readBe16(const uint8_t *in) {
  return static_cast<uint16_t>(in[0] << 8U) | in[1];
}

inline bool parseMac12(const char *text, uint8_t out[6]) {
  if (!text || strlen(text) != 12) return false;
  for (uint8_t i = 0; i < 6; ++i) {
    char pair[3] = {text[i * 2], text[i * 2 + 1], '\0'};
    char *end = nullptr;
    const long value = strtol(pair, &end, 16);
    if (end != pair + 2 || value < 0 || value > 255) return false;
    out[i] = static_cast<uint8_t>(value);
  }
  return true;
}

inline size_t buildAdarDiscoveryRequest(
    uint8_t *out,
    size_t capacity,
    const char excluded[][13],
    uint8_t excludedCount,
    const uint8_t rangeStart[6] = nullptr,
    const uint8_t rangeEnd[6] = nullptr) {
  if (!out || excludedCount > 50) return 0;
  const size_t required = ADAR_DISCOVERY_BASE_SIZE + static_cast<size_t>(excludedCount) * 6U;
  if (capacity < required) return 0;
  memcpy(out, ADAR_MAGIC, 8);
  writeBe16(out + 8, ADAR_DISCOVERY_REQUEST);
  writeBe16(out + 10, ADAR_VERSION_1_1);
  writeBe32(out + 12, 0xFFFFFFFFUL);
  writeBe32(out + 16, 0xFFFFFFFFUL);
  memset(out + 20, 0, 6);
  memset(out + 26, 0xFF, 6);
  if (rangeStart) memcpy(out + 20, rangeStart, 6);
  if (rangeEnd) memcpy(out + 26, rangeEnd, 6);
  writeBe16(out + 32, excludedCount);
  for (uint8_t i = 0; i < excludedCount; ++i) {
    if (!parseMac12(excluded[i], out + 34 + i * 6)) return 0;
  }
  return required;
}

inline bool validAdarResponseHeader(const uint8_t *data, size_t length) {
  return data && length > 12 && memcmp(data, ADAR_MAGIC, 8) == 0 &&
         readBe16(data + 8) == ADAR_DISCOVERY_RESPONSE &&
         readBe16(data + 10) == ADAR_VERSION_1_1 && data[12] == '{';
}

}  // namespace advatek

// =============================================================================
// Module: src/core/AdarCodec.h
// =============================================================================
namespace advatek {

inline bool parseAdarDiscoveryResponse(
    const uint8_t *packet,
    size_t length,
    bool viaBroadcast,
    DiscoveredDevice &device) {
  if (!validAdarResponseHeader(packet, length) || length >= 1024) return false;
  char json[1024];
  const size_t jsonLength = length - 12;
  memcpy(json, packet + 12, jsonLength);
  json[jsonLength] = '\0';
  JsonToken tokens[96];
  JsonDocument document(json, tokens, 96);
  if (!document.parse()) return false;
  const int16_t root = document.root();
  const int16_t family = document.objectValue(root, "prodFamily");
  const int16_t mac = document.objectValue(root, "macAddr");
  const int16_t ip = document.objectValue(root, "ipAddr");
  const int16_t product = document.objectValue(root, "prodName");
  if (family < 0 || mac < 0 || ip < 0 || product < 0) return false;

  memset(&device, 0, sizeof(device));
  device.productFamily = static_cast<uint32_t>(document.integerValue(family, -1));
  if (device.productFamily != PIXLITE_MK3_FAMILY ||
      !document.stringValue(mac, device.macAddress, sizeof(device.macAddress)) ||
      !document.stringValue(ip, device.ipAddress, sizeof(device.ipAddress)) ||
      !document.stringValue(product, device.productName, sizeof(device.productName))) {
    return false;
  }
  uint8_t parsedMac[6];
  IPAddress parsedIp;
  if (!parseMac12(device.macAddress, parsedMac) || !parseIPv4(device.ipAddress, parsedIp)) return false;
  const int16_t familyName = document.objectValue(root, "prodFamilyName");
  const int16_t nickname = document.objectValue(root, "nickname");
  const int16_t firmware = document.objectValue(root, "fwVer");
  if (familyName >= 0) document.stringValue(familyName, device.familyName, sizeof(device.familyName));
  if (nickname >= 0) document.stringValue(nickname, device.nickname, sizeof(device.nickname));
  if (firmware >= 0) document.stringValue(firmware, device.firmwareVersion, sizeof(device.firmwareVersion));
  device.authRequired = document.booleanValue(document.objectValue(root, "authReqd"), false);
  device.emergencyRecovery =
      document.booleanValue(document.objectValue(root, "inEmergRecovery"), false);
  device.viaBroadcast = viaBroadcast;
  return true;
}

}  // namespace advatek

// =============================================================================
// Module: src/core/PixLiteProtocol.h
// =============================================================================
namespace advatek {

inline String pixLiteFileListRequest(uint32_t id) {
  return "{\"req\":\"fileList\",\"id\":" + String(id) +
         ",\"params\":{\"pattern\":[\"*.scn\",\"*.pl\"]}}";
}

inline String pixLitePlaybackRequest(const ActionConfig &action, uint32_t id) {
  String body = "{\"req\":\"modePlayback\",\"id\":" + String(id) + ",\"params\":{";
  if (action.kind == ActionKind::BlankOutputs) {
    body += "\"op\":\"Blank\"}}";
  } else {
    body += "\"op\":\"File\",\"file\":\"" + jsonEscape(action.mediaName) +
            "\",\"cnt\":" +
            String(action.repeat == PlaybackRepeat::Forever ? 0 : 1) + "}}";
  }
  return body;
}

inline String pixLiteStopRequest(uint32_t id) {
  // modeCtrl/Stop is deliberately not used here: the PixLite API defines it
  // as a no-op when the current mode is continuous, which includes a file
  // played with cnt:0. Explicitly returning to live mode stops both transient
  // and looping playback deterministically.
  return "{\"req\":\"modeLive\",\"id\":" + String(id) +
         ",\"params\":{\"fadeMs\":0}}";
}

inline String pixLiteTestColorRequest(const ActionConfig &action, uint32_t id) {
  // These are the same v1.9 operations emitted by the PixLite Mk3 Test Mode
  // page for Custom Color and RGB Fade. Zero selectors apply them globally.
  const char *operation =
      action.kind == ActionKind::TestColorFade ? "colorFade" : "setColor";
  return "{\"req\":\"modeTestData\",\"id\":" + String(id) +
         ",\"params\":{\"op\":\"" + operation + String("\",\"color\":[") +
         String(action.testRed) + "," + String(action.testGreen) + "," +
         String(action.testBlue) +
         "],\"colorRes\":\"8Bit\",\"pixPortNum\":0,\"pixNum\":0}}";
}

inline void appendIntensityItem(
    String &body,
    const char *output,
    const char *operation,
    uint8_t factor,
    uint16_t fadeMs,
    bool &first) {
  if (!first) body += ',';
  first = false;
  body += "{\"out\":\"";
  body += output;
  body += "\",\"op\":\"";
  body += operation;
  body += '"';
  // PixLite v1.9 rejects Lo when either a factor or duration is supplied.
  // A release is immediate at the API layer, even when the configured
  // intensity action uses a fade for HiSet.
  if (strcmp(operation, "Lo") != 0) {
    body += ",\"fc\":" + String(factor);
    if (fadeMs) body += ",\"durMs\":" + String(fadeMs);
  }
  body += '}';
}

inline String pixLiteIntensityRequest(
    const ActionConfig &action,
    uint32_t id,
    uint8_t configuredAuxMask = 0xFF) {
  const char *operation = "HiSet";
  if (action.kind == ActionKind::ReleaseIntensityOverride) operation = "Lo";
  const uint8_t factor = percentToFactor(action.intensityPercent);
  String body = "{\"req\":\"progInt\",\"id\":" + String(id) + ",\"params\":{\"item\":[";
  bool first = true;
  if (action.outputSelection == OutputSelection::Pixels ||
      action.outputSelection == OutputSelection::PixelsAndAllAux) {
    appendIntensityItem(body, "Pix", operation, factor, action.fadeMs, first);
  }
  if (action.outputSelection == OutputSelection::Aux) {
    char output[12];
    snprintf(output, sizeof(output), "Aux:%d", action.auxOutput);
    appendIntensityItem(body, output, operation, factor, action.fadeMs, first);
  } else if (action.outputSelection == OutputSelection::PixelsAndAllAux) {
    for (uint8_t aux = 1; aux <= 8; ++aux) {
      if ((configuredAuxMask & (1U << (aux - 1U))) == 0) continue;
      char output[12];
      snprintf(output, sizeof(output), "Aux:%u", aux);
      appendIntensityItem(body, output, operation, factor, action.fadeMs, first);
    }
  }
  body += "]}}";
  return body;
}

inline String pixLiteStatusRequest(uint32_t id) {
  return "{\"req\":\"statusRead\",\"id\":" + String(id) + ",\"params\":{}}";
}

inline bool mediaNameIsPlaylist(const char *name) {
  if (!name) return false;
  const size_t length = strlen(name);
  return length >= 3 && strcasecmp(name + length - 3, ".pl") == 0;
}

inline bool mediaNameIsScene(const char *name) {
  if (!name) return false;
  const size_t length = strlen(name);
  return length >= 4 && strcasecmp(name + length - 4, ".scn") == 0;
}

}  // namespace advatek

// =============================================================================
// Module: src/core/ConfigJson.h
// =============================================================================
namespace advatek {

inline const char *actionKindName(ActionKind kind) {
  static const char *names[] = {
      "none", "playScene", "playPlaylist", "stop", "blank",
      "brighter", "darker", "setIntensity", "releaseIntensity", "testColor",
      "nextScene", "previousScene", "testColorFade"};
  const uint8_t index = static_cast<uint8_t>(kind);
  return index < sizeof(names) / sizeof(names[0]) ? names[index] : "none";
}

inline ActionKind parseActionKind(const char *name) {
  for (uint8_t i = 0; i <= static_cast<uint8_t>(ActionKind::TestColorFade); ++i) {
    if (strcmp(name, actionKindName(static_cast<ActionKind>(i))) == 0) {
      return static_cast<ActionKind>(i);
    }
  }
  return ActionKind::None;
}

inline void appendActionJson(String &json, const ActionConfig &action) {
  json += "{\"kind\":\"" + String(actionKindName(action.kind)) + "\",";
  json += "\"targetId\":\"" + jsonEscape(action.targetId) + "\",";
  json += "\"mediaName\":\"" + jsonEscape(action.mediaName) + "\",";
  json += "\"repeat\":\"" +
          String(action.repeat == PlaybackRepeat::Forever ? "forever" : "once") + "\",";
  json += "\"output\":\"";
  if (action.outputSelection == OutputSelection::Aux) json += "aux";
  else if (action.outputSelection == OutputSelection::PixelsAndAllAux) json += "pixelsAndAux";
  else json += "pixels";
  json += "\",\"auxOutput\":" + String(action.auxOutput);
  json += ",\"intensityPercent\":" + String(action.intensityPercent);
  json += ",\"stepPercent\":" + String(action.intensityStepPercent);
  json += ",\"holdDelayMs\":" + String(action.holdDelayMs);
  json += ",\"repeatIntervalMs\":" + String(action.repeatIntervalMs);
  json += ",\"fadeMs\":" + String(action.fadeMs);
  char testColor[8];
  snprintf(
      testColor,
      sizeof(testColor),
      "#%02X%02X%02X",
      action.testRed,
      action.testGreen,
      action.testBlue);
  json += ",\"testColor\":\"" + String(testColor) + "\"";
  // Numeric channels make action payloads unambiguous for lightweight JSON
  // clients while the hex value remains convenient for the web colour input.
  json += ",\"testRed\":" + String(action.testRed);
  json += ",\"testGreen\":" + String(action.testGreen);
  json += ",\"testBlue\":" + String(action.testBlue) + "}";
}

inline String configJson(const AppConfig &config, bool includeSecrets = false) {
  String json;
  json.reserve(7000);
  json = "{\"schemaVersion\":" + String(config.schemaVersion) +
         ",\"boardId\":\"" + jsonEscape(config.hardware.boardId) + "\",";
  json += "\"hardwareBinding\":{\"boardId\":\"" +
          jsonEscape(config.hardware.boardId) + "\",\"profileVersion\":\"" +
          jsonEscape(config.hardware.profileVersion) + "\",\"inputGpios\":[";
  for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
    if (i) json += ',';
    if (config.hardware.inputGpios[i] == PIN_UNASSIGNED) json += "null";
    else json += String(config.hardware.inputGpios[i]);
  }
  json += "]},";
  json += "\"network\":{\"uplink\":\"" +
          String(config.network.uplink == UplinkMode::Ethernet ? "ethernet" : "wifi") + "\",";
  json += "\"hostname\":\"" + jsonEscape(config.network.hostname) + "\",";
  json += "\"wifiSsid\":\"" + jsonEscape(config.network.wifiSsid) + "\",";
  auto appendIp = [&json](const char *name, const IPv4Config &ip) {
    json += "\"" + String(name) + "\":{\"mode\":\"" +
            String(ip.mode == IpMode::Static ? "static" : "dhcp") + "\",";
    json += "\"address\":\"" + jsonEscape(ip.address) + "\",";
    json += "\"netmask\":\"" + jsonEscape(ip.netmask) + "\",";
    json += "\"gateway\":\"" + jsonEscape(ip.gateway) + "\",";
    json += "\"dns\":\"" + jsonEscape(ip.dns) + "\"},";
  };
  appendIp("ethernet", config.network.ethernet);
  appendIp("wifi", config.network.wifi);
  json += "\"recoveryConnection\":\"";
  json += usesEthernetRecovery(config.network) ? "ethernet\"" : "wifi\"";
  if (includeSecrets) {
    json += ",\"wifiPassword\":\"" + jsonEscape(config.network.wifiPassword) + "\"";
    json += ",\"apPassword\":\"" + jsonEscape(config.network.accessPointPassword) + "\"";
  }
  json += "},\"statusLed\":{\"enabled\":" +
          String(config.statusLed.enabled ? "true" : "false") +
          ",\"brightnessPercent\":" +
          String(config.statusLed.brightnessPercent) + "},\"pixlites\":[";
  for (uint8_t i = 0; i < config.pixliteCount && i < MAX_PIXLITES; ++i) {
    if (i) json += ',';
    const PixLiteConfig &pixlite = config.pixlites[i];
    json += "{\"id\":\"" + jsonEscape(pixlite.id) +
            "\",\"name\":\"" + jsonEscape(pixlite.displayName) +
            "\",\"mac\":\"" + jsonEscape(pixlite.macAddress) +
            "\",\"host\":\"" + jsonEscape(pixlite.host) +
            "\",\"port\":" + String(pixlite.port) +
            ",\"apiVersion\":\"" + jsonEscape(pixlite.apiVersion) +
            "\",\"username\":\"" + jsonEscape(pixlite.username) + "\"";
    if (includeSecrets) {
      json += ",\"password\":\"" + jsonEscape(pixlite.password) + "\"";
    }
    json += '}';
  }
  json += "],\"primaryPixliteId\":\"";
  if (config.pixliteCount) {
    const uint8_t primary =
        config.primaryPixliteIndex < config.pixliteCount
            ? config.primaryPixliteIndex
            : 0;
    json += jsonEscape(config.pixlites[primary].id);
  }
  json += "\",\"inputs\":[";
  for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
    if (i) json += ',';
    const InputConfig &input = config.inputs[i];
    json += "{\"enabled\":" + String(input.enabled ? "true" : "false");
    json += ",\"name\":\"" + jsonEscape(input.name) + "\"";
    json += ",\"gpio\":";
    if (config.hardware.inputGpios[i] == PIN_UNASSIGNED) json += "null";
    else json += String(config.hardware.inputGpios[i]);
    json += ",\"normallyClosed\":" + String(input.normallyClosed ? "true" : "false");
    json += ",\"mode\":\"" +
            String(input.mode == InputMode::Momentary ? "momentary" : "maintained") + "\"";
    json += ",\"debounceMs\":" + String(input.debounceMs) + ",\"onAction\":";
    appendActionJson(json, input.onAction);
    json += ",\"offAction\":";
    appendActionJson(json, input.offAction);
    json += '}';
  }
  json += "],\"secretsOmitted\":";
  json += includeSecrets ? "false}" : "true}";
  return json;
}

inline bool updateString(
    const JsonDocument &doc,
    int16_t object,
    const char *key,
    char *destination,
    size_t capacity) {
  const int16_t token = doc.objectValue(object, key);
  return token >= 0 && doc.stringValue(token, destination, capacity);
}

inline int8_t hexNibble(char value) {
  if (value >= '0' && value <= '9') return value - '0';
  if (value >= 'a' && value <= 'f') return value - 'a' + 10;
  if (value >= 'A' && value <= 'F') return value - 'A' + 10;
  return -1;
}

inline bool parseTestColor(
    const char *text,
    uint8_t &red,
    uint8_t &green,
    uint8_t &blue) {
  if (!text || strlen(text) != 7 || text[0] != '#') return false;
  uint8_t parsed[3]{};
  for (uint8_t channel = 0; channel < 3; ++channel) {
    const int8_t high = hexNibble(text[1 + channel * 2]);
    const int8_t low = hexNibble(text[2 + channel * 2]);
    if (high < 0 || low < 0) return false;
    parsed[channel] = static_cast<uint8_t>((high << 4) | low);
  }
  red = parsed[0];
  green = parsed[1];
  blue = parsed[2];
  return true;
}

inline void parseAction(const JsonDocument &doc, int16_t object, ActionConfig &action) {
  if (object < 0 || doc.token(object).type != JsonTokenType::Object) return;
  char text[64];
  if (updateString(doc, object, "kind", text, sizeof(text))) action.kind = parseActionKind(text);
  updateString(doc, object, "targetId", action.targetId, sizeof(action.targetId));
  updateString(doc, object, "mediaName", action.mediaName, sizeof(action.mediaName));
  if (updateString(doc, object, "repeat", text, sizeof(text))) {
    action.repeat = strcmp(text, "forever") == 0 ? PlaybackRepeat::Forever : PlaybackRepeat::Once;
  }
  if (updateString(doc, object, "output", text, sizeof(text))) {
    if (strcmp(text, "aux") == 0) action.outputSelection = OutputSelection::Aux;
    else if (strcmp(text, "pixelsAndAux") == 0) {
      action.outputSelection = OutputSelection::PixelsAndAllAux;
    } else {
      action.outputSelection = OutputSelection::Pixels;
    }
  }
  action.auxOutput =
      constrain(doc.integerValue(doc.objectValue(object, "auxOutput"), action.auxOutput), 1, 8);
  action.intensityPercent = constrain(
      doc.integerValue(doc.objectValue(object, "intensityPercent"), action.intensityPercent), 0, 100);
  action.intensityStepPercent = constrain(
      doc.integerValue(doc.objectValue(object, "stepPercent"), action.intensityStepPercent), 1, 100);
  action.holdDelayMs = constrain(
      doc.integerValue(doc.objectValue(object, "holdDelayMs"), action.holdDelayMs), 50, 5000);
  action.repeatIntervalMs = constrain(
      doc.integerValue(doc.objectValue(object, "repeatIntervalMs"), action.repeatIntervalMs), 50, 5000);
  action.fadeMs =
      constrain(doc.integerValue(doc.objectValue(object, "fadeMs"), action.fadeMs), 0, 5000);
  char testColor[8];
  if (updateString(doc, object, "testColor", testColor, sizeof(testColor))) {
    parseTestColor(testColor, action.testRed, action.testGreen, action.testBlue);
  }
  action.testRed = constrain(
      doc.integerValue(doc.objectValue(object, "testRed"), action.testRed),
      0,
      255);
  action.testGreen = constrain(
      doc.integerValue(doc.objectValue(object, "testGreen"), action.testGreen),
      0,
      255);
  action.testBlue = constrain(
      doc.integerValue(doc.objectValue(object, "testBlue"), action.testBlue),
      0,
      255);
}

inline void parseIpConfig(
    const JsonDocument &doc,
    int16_t network,
    const char *key,
    IPv4Config &settings) {
  const int16_t object = doc.objectValue(network, key);
  if (object < 0) return;
  char mode[12];
  if (updateString(doc, object, "mode", mode, sizeof(mode))) {
    settings.mode = strcmp(mode, "static") == 0 ? IpMode::Static : IpMode::Dhcp;
  }
  updateString(doc, object, "address", settings.address, sizeof(settings.address));
  updateString(doc, object, "netmask", settings.netmask, sizeof(settings.netmask));
  updateString(doc, object, "gateway", settings.gateway, sizeof(settings.gateway));
  updateString(doc, object, "dns", settings.dns, sizeof(settings.dns));
}

inline void initializeJsonPixLite(PixLiteConfig &pixlite, uint8_t index) {
  memset(&pixlite, 0, sizeof(pixlite));
  pixlite.enabled = true;
  pixlite.port = 80;
  snprintf(pixlite.id, sizeof(pixlite.id), "pixlite-%u", index + 1);
  snprintf(pixlite.displayName, sizeof(pixlite.displayName), "PixLite %u", index + 1);
  copyText(pixlite.apiVersion, sizeof(pixlite.apiVersion), "v1.9");
  copyText(pixlite.username, sizeof(pixlite.username), "oper");
}

inline void parsePixLiteConfig(
    const JsonDocument &doc,
    int16_t object,
    PixLiteConfig &pixlite) {
  updateString(doc, object, "id", pixlite.id, sizeof(pixlite.id));
  updateString(doc, object, "name", pixlite.displayName, sizeof(pixlite.displayName));
  updateString(doc, object, "mac", pixlite.macAddress, sizeof(pixlite.macAddress));
  updateString(doc, object, "host", pixlite.host, sizeof(pixlite.host));
  updateString(doc, object, "username", pixlite.username, sizeof(pixlite.username));
  updateString(doc, object, "password", pixlite.password, sizeof(pixlite.password));
  pixlite.port = constrain(
      doc.integerValue(doc.objectValue(object, "port"), pixlite.port),
      1,
      65535);
  pixlite.enabled = true;
}

// Applies a complete or partial configuration document. Omitted secrets are
// intentionally retained so redacted backups can be restored on the same unit.
inline bool applyConfigJson(
    const char *json,
    AppConfig &config,
    const BoardProfile &board,
    JsonToken *tokens,
    size_t tokenCapacity,
    char *error,
    size_t errorCapacity) {
  if (!tokens || tokenCapacity < 64) {
    copyText(error, errorCapacity, "Configuration parser memory is unavailable");
    return false;
  }
  JsonDocument doc(json, tokens, tokenCapacity);
  if (!doc.parse() || doc.token(doc.root()).type != JsonTokenType::Object) {
    copyText(error, errorCapacity, "Invalid JSON document");
    return false;
  }
  const int16_t root = doc.root();
  const int16_t network = doc.objectValue(root, "network");
  if (network >= 0) {
    char text[72];
    if (updateString(doc, network, "uplink", text, sizeof(text))) {
      config.network.uplink = strcmp(text, "wifi") == 0 ? UplinkMode::WifiStation : UplinkMode::Ethernet;
    }
    updateString(doc, network, "hostname", config.network.hostname, sizeof(config.network.hostname));
    updateString(doc, network, "wifiSsid", config.network.wifiSsid, sizeof(config.network.wifiSsid));
    updateString(doc, network, "wifiPassword", config.network.wifiPassword, sizeof(config.network.wifiPassword));
    updateString(
        doc, network, "apPassword", config.network.accessPointPassword,
        sizeof(config.network.accessPointPassword));
    const bool recoveryConnectionUpdated =
        updateString(
            doc, network, "recoveryConnection", text, sizeof(text));
    if (recoveryConnectionUpdated) {
      config.network.accessPointMode =
          strcmp(text, "ethernet") == 0
              ? AccessPointMode::AlwaysOn
              : AccessPointMode::Disabled;
    }
    // Continue accepting backups from releases that exposed normal AP modes.
    if (!recoveryConnectionUpdated &&
        updateString(doc, network, "apMode", text, sizeof(text))) {
      if (strcmp(text, "disabled") == 0) config.network.accessPointMode = AccessPointMode::Disabled;
      else if (strcmp(text, "always") == 0) config.network.accessPointMode = AccessPointMode::AlwaysOn;
      else config.network.accessPointMode = AccessPointMode::AutoOff;
    }
    parseIpConfig(doc, network, "ethernet", config.network.ethernet);
    parseIpConfig(doc, network, "wifi", config.network.wifi);
  }
  const int16_t pixlites = doc.objectValue(root, "pixlites");
  if (pixlites >= 0 && doc.token(pixlites).type == JsonTokenType::Array) {
    PixLiteConfig parsed[MAX_PIXLITES]{};
    const uint8_t requested =
        static_cast<uint8_t>(
            constrain(doc.arrayLength(pixlites), 0, MAX_PIXLITES));
    for (uint8_t i = 0; i < requested; ++i) {
      initializeJsonPixLite(parsed[i], i);
      const int16_t item = doc.arrayItem(pixlites, i);
      char requestedId[16]{};
      char requestedMac[13]{};
      updateString(doc, item, "id", requestedId, sizeof(requestedId));
      updateString(doc, item, "mac", requestedMac, sizeof(requestedMac));
      const int8_t existing =
          requestedId[0]
              ? pixLiteIndexById(config, requestedId)
              : pixLiteIndexByMac(config, requestedMac);
      if (existing >= 0) {
        parsed[i] = config.pixlites[existing];
      }
      parsePixLiteConfig(doc, item, parsed[i]);
    }
    memset(config.pixlites, 0, sizeof(config.pixlites));
    memcpy(config.pixlites, parsed, requested * sizeof(PixLiteConfig));
    config.pixliteCount = requested;
    if (config.primaryPixliteIndex >= config.pixliteCount) {
      config.primaryPixliteIndex = 0;
    }
  }
  // Accept the schema-v3 singular object for API compatibility. It updates
  // the primary entry but never discards additional schema-v4 targets.
  const int16_t legacyPixlite = doc.objectValue(root, "pixlite");
  if (legacyPixlite >= 0) {
    if (config.pixliteCount == 0) {
      initializeJsonPixLite(config.pixlites[0], 0);
      copyText(config.pixlites[0].id, sizeof(config.pixlites[0].id), "primary");
      config.pixliteCount = 1;
    }
    parsePixLiteConfig(
        doc,
        legacyPixlite,
        config.pixlites[config.primaryPixliteIndex]);
  }
  char primaryId[16]{};
  if (updateString(doc, root, "primaryPixliteId", primaryId, sizeof(primaryId))) {
    const int8_t primary = pixLiteIndexById(config, primaryId);
    if (primary >= 0) config.primaryPixliteIndex = static_cast<uint8_t>(primary);
  }
  const int16_t statusLed = doc.objectValue(root, "statusLed");
  if (statusLed >= 0 && doc.token(statusLed).type == JsonTokenType::Object) {
    config.statusLed.enabled =
        doc.booleanValue(
            doc.objectValue(statusLed, "enabled"),
            config.statusLed.enabled);
    config.statusLed.brightnessPercent = constrain(
        doc.integerValue(
            doc.objectValue(statusLed, "brightnessPercent"),
            config.statusLed.brightnessPercent),
        1,
        100);
  }
  const int16_t inputs = doc.objectValue(root, "inputs");
  bool bindingProvided = false;
  const int16_t hardware = doc.objectValue(root, "hardwareBinding");
  if (hardware >= 0 && doc.token(hardware).type == JsonTokenType::Object) {
    const int16_t gpios = doc.objectValue(hardware, "inputGpios");
    if (gpios >= 0 && doc.token(gpios).type == JsonTokenType::Array) {
      bindingProvided = true;
      for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
        const int16_t token = doc.arrayItem(gpios, i);
        const long requested = doc.integerValue(token, PIN_UNASSIGNED);
        config.hardware.inputGpios[i] =
            requested >= 0 && requested <= 48 &&
                    pinAllowed(board, static_cast<uint8_t>(requested))
                ? static_cast<uint8_t>(requested)
                : PIN_UNASSIGNED;
      }
    }
  }
  if (inputs >= 0 && doc.token(inputs).type == JsonTokenType::Array) {
    const int16_t requestedInputCount = doc.arrayLength(inputs);
    const int16_t inputCount =
        requestedInputCount < MAX_INPUTS ? requestedInputCount : MAX_INPUTS;
    for (int16_t i = 0; i < inputCount; ++i) {
      const int16_t item = doc.arrayItem(inputs, i);
      InputConfig &input = config.inputs[i];
      input.enabled = doc.booleanValue(doc.objectValue(item, "enabled"), input.enabled);
      input.normallyClosed =
          doc.booleanValue(doc.objectValue(item, "normallyClosed"), input.normallyClosed);
      if (!bindingProvided) {
        const long requested = doc.integerValue(
            doc.objectValue(item, "gpio"), config.hardware.inputGpios[i]);
        config.hardware.inputGpios[i] =
            requested >= 0 && requested <= 48 &&
                    pinAllowed(board, static_cast<uint8_t>(requested))
                ? static_cast<uint8_t>(requested)
                : PIN_UNASSIGNED;
      }
      input.debounceMs = constrain(
          doc.integerValue(doc.objectValue(item, "debounceMs"), input.debounceMs), 10, 2000);
      updateString(doc, item, "name", input.name, sizeof(input.name));
      char mode[16];
      if (updateString(doc, item, "mode", mode, sizeof(mode))) {
        input.mode = strcmp(mode, "maintained") == 0 ? InputMode::Maintained : InputMode::Momentary;
      }
      parseAction(doc, doc.objectValue(item, "onAction"), input.onAction);
      parseAction(doc, doc.objectValue(item, "offAction"), input.offAction);
    }
  }
  copyText(config.hardware.boardId, sizeof(config.hardware.boardId), board.id);
  copyText(
      config.hardware.profileVersion,
      sizeof(config.hardware.profileVersion),
      board.profileVersion);
  const ValidationResult validation = validateConfig(config, board);
  if (!validation.valid) {
    copyText(error, errorCapacity, validation.message);
    return false;
  }
  return true;
}

}  // namespace advatek

// =============================================================================
// Module: src/web/WebAsset.h
// =============================================================================
// Generated by tools/embed-web.mjs from the minified Vite build.
// Do not edit this byte array by hand.
static const uint8_t WEB_INDEX_GZIP[] PROGMEM = {
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xad, 0x7d,
    0xdb, 0x72, 0xdb, 0x48, 0x92, 0xe8, 0xfb, 0x7e, 0x45, 0x19, 0x76, 0xab,
    0x81, 0xd3, 0x20, 0x44, 0x52, 0x92, 0x2d, 0x93, 0x22, 0xb9, 0x6e, 0xdb,
    0x3d, 0xad, 0x19, 0x5f, 0xb4, 0xbe, 0xcc, 0x65, 0x35, 0x9a, 0x25, 0x08,
    0x14, 0xc9, 0x1a, 0x81, 0x05, 0x34, 0xaa, 0xa8, 0xcb, 0x50, 0x88, 0xe8,
    0x4f, 0x38, 0x11, 0xbb, 0x11, 0xe7, 0xe5, 0x3c, 0xef, 0x2f, 0x6c, 0xc4,
    0x79, 0xdc, 0x4f, 0xe9, 0x2f, 0x39, 0x91, 0x59, 0x55, 0x40, 0x01, 0x20,
    0x65, 0xb9, 0xb7, 0xdb, 0x11, 0x2d, 0xa2, 0xee, 0x97, 0xac, 0xcc, 0xac,
    0xbc, 0xd5, 0xc9, 0xa3, 0x38, 0x8d, 0xe4, 0x6d, 0x46, 0xc9, 0x52, 0xae,
    0x92, 0xf1, 0x09, 0xfc, 0x9f, 0x24, 0x21, 0x5f, 0x8c, 0x1c, 0xca, 0x9d,
    0xf1, 0xc9, 0x92, 0x86, 0xf1, 0xf8, 0x64, 0x45, 0x65, 0x48, 0xa2, 0x65,
    0x98, 0x0b, 0x2a, 0x47, 0xce, 0xe7, 0x4f, 0x3f, 0x74, 0x8e, 0x1d, 0xb2,
    0xaf, 0xd3, 0x79, 0xb8, 0xa2, 0x23, 0xe7, 0x8a, 0xd1, 0xeb, 0x2c, 0xcd,
    0xa5, 0x43, 0xa2, 0x94, 0x4b, 0xca, 0xe5, 0xc8, 0xb9, 0x66, 0xb1, 0x5c,
    0x8e, 0x62, 0x7a, 0xc5, 0x22, 0xda, 0xc1, 0x0f, 0x9f, 0x71, 0x26, 0x59,
    0x98, 0x74, 0x44, 0x14, 0x26, 0x74, 0xd4, 0x6b, 0x36, 0x22, 0x97, 0x74,
    0x45, 0x3b, 0x51, 0x9a, 0xa4, 0xb9, 0xd5, 0xce, 0xe3, 0x7e, 0x17, 0xfe,
    0x61, 0x61, 0xc9, 0x64, 0x42, 0xc7, 0x2f, 0xe2, 0xab, 0x50, 0xd2, 0x4b,
    0xf2, 0x26, 0x9c, 0x09, 0xf2, 0xdf, 0xff, 0x45, 0x5e, 0xa6, 0x5c, 0x86,
    0x91, 0x24, 0x9f, 0x72, 0xb6, 0x58, 0xd0, 0xfc, 0x64, 0x5f, 0x15, 0x3b,
    0x11, 0x51, 0xce, 0x32, 0x49, 0x60, 0x7e, 0x23, 0x67, 0x95, 0xc6, 0xeb,
    0x84, 0x3a, 0x63, 0x77, 0xbe, 0xe6, 0x91, 0x64, 0x29, 0x77, 0xbd, 0x4d,
    0x94, 0x72, 0x21, 0x09, 0x1d, 0xc5, 0x69, 0xb4, 0x5e, 0x51, 0x2e, 0x83,
    0x28, 0xa7, 0xa1, 0xa4, 0xaf, 0x13, 0x0a, 0x5f, 0xae, 0x93, 0x30, 0x7e,
    0xe9, 0x78, 0x41, 0x4e, 0x93, 0x37, 0x4c, 0xc8, 0x21, 0x9b, 0xbb, 0x74,
    0x6f, 0x8f, 0x06, 0x62, 0x9d, 0xc1, 0x5c, 0x85, 0xfd, 0xdb, 0xd5, 0x1d,
    0x64, 0x39, 0x4d, 0xd2, 0x30, 0x76, 0x3c, 0x2f, 0xa7, 0x72, 0x9d, 0xf3,
    0xe1, 0x3c, 0xcd, 0x5d, 0xd5, 0x8f, 0x20, 0xe9, 0x9c, 0x94, 0x5d, 0xfd,
    0xb4, 0xa6, 0xf9, 0xed, 0x47, 0x9a, 0xd0, 0x48, 0xa6, 0xf9, 0x8b, 0x24,
    0x71, 0xbf, 0x85, 0xde, 0xce, 0x73, 0x9a, 0x8c, 0x1a, 0x4d, 0x5d, 0x7c,
    0xeb, 0x79, 0xcc, 0x15, 0xde, 0x90, 0xd3, 0x6b, 0xf2, 0x76, 0x2d, 0x43,
    0x18, 0xfd, 0xfb, 0x99, 0xa0, 0xf9, 0x15, 0xcd, 0x5d, 0x31, 0x1a, 0x6f,
    0xaa, 0x3e, 0x22, 0xe8, 0x43, 0x78, 0x6c, 0xee, 0x46, 0x01, 0xce, 0x7b,
    0x34, 0x72, 0xa2, 0x25, 0x4b, 0x62, 0x98, 0x80, 0xe3, 0x55, 0x05, 0x33,
    0x28, 0x18, 0x05, 0x61, 0x1c, 0xd3, 0xf8, 0x5d, 0x1a, 0x53, 0xe1, 0x65,
    0x81, 0x0c, 0x17, 0xef, 0x60, 0x23, 0x46, 0x23, 0xe7, 0xcd, 0xe9, 0xbb,
    0x3f, 0x38, 0x7b, 0x7b, 0x19, 0xcc, 0x1d, 0xbe, 0xeb, 0x23, 0xda, 0xdb,
    0x63, 0x6e, 0xe6, 0x15, 0x5e, 0x90, 0xaa, 0x51, 0xb8, 0x66, 0x56, 0xfe,
    0xa6, 0xec, 0x6c, 0xf0, 0xa8, 0xeb, 0x8b, 0xf5, 0x4c, 0xe6, 0x94, 0x0e,
    0x1e, 0x75, 0x0b, 0x6f, 0x68, 0xd6, 0x9d, 0x70, 0x57, 0x98, 0xa5, 0x8f,
    0x46, 0x9b, 0x62, 0xa8, 0x16, 0x8a, 0x88, 0x80, 0x71, 0x49, 0x17, 0x39,
    0x93, 0xb7, 0x7b, 0x7b, 0x6e, 0x54, 0x7d, 0x8d, 0xac, 0x1c, 0xcf, 0x17,
    0x41, 0x4e, 0xe7, 0x34, 0xcf, 0x69, 0x7e, 0x96, 0x26, 0x2c, 0x52, 0x65,
    0xeb, 0x49, 0xa3, 0x66, 0x19, 0xa8, 0x15, 0xe5, 0xa9, 0x10, 0xef, 0x73,
    0xb6, 0x60, 0x1c, 0x26, 0xb4, 0x16, 0xb4, 0x13, 0xe5, 0x34, 0xa6, 0x1c,
    0x20, 0x52, 0x38, 0x93, 0x28, 0xb0, 0x3e, 0x47, 0x0e, 0xe3, 0x51, 0xb2,
    0x8e, 0xa9, 0x33, 0x68, 0xd5, 0x0c, 0x79, 0xca, 0x6f, 0x57, 0xe9, 0xba,
    0x5d, 0x27, 0x5d, 0x31, 0xe9, 0x0c, 0x1a, 0x89, 0x22, 0x5c, 0xd1, 0x4e,
    0x8a, 0xb5, 0x1d, 0x3f, 0x2a, 0xca, 0x55, 0x80, 0x1d, 0xdd, 0xb0, 0xb9,
    0x2b, 0x02, 0x9a, 0x19, 0x58, 0x81, 0xdf, 0xa3, 0x47, 0xdd, 0xa1, 0x59,
    0x1c, 0x58, 0xa9, 0xe1, 0x9c, 0xca, 0x68, 0xe9, 0x8a, 0x60, 0x99, 0xd3,
    0xb9, 0x1f, 0x79, 0x45, 0xe1, 0xb9, 0xde, 0x30, 0x4a, 0x42, 0x21, 0xc8,
    0x1f, 0x08, 0xbd, 0x91, 0x94, 0xc7, 0x82, 0xbc, 0xce, 0xf3, 0x34, 0x57,
    0x8b, 0x9a, 0xaf, 0x01, 0xa2, 0x5c, 0xea, 0x73, 0x6f, 0x23, 0xd6, 0x19,
    0xcd, 0x5d, 0xea, 0xf9, 0x72, 0xc9, 0x44, 0x90, 0x85, 0xb7, 0xb0, 0x7d,
    0x23, 0x5e, 0x14, 0x09, 0x95, 0x24, 0xf5, 0x93, 0xd1, 0xa6, 0xf0, 0xcf,
    0x46, 0x82, 0x0a, 0xc1, 0x52, 0xfe, 0x51, 0xa6, 0x79, 0xb8, 0xa0, 0xc1,
    0x82, 0xca, 0x53, 0x49, 0x57, 0xae, 0x13, 0xaa, 0x73, 0x76, 0x16, 0x0a,
    0x71, 0x9d, 0xe6, 0xb1, 0xe3, 0x4d, 0x26, 0x8e, 0xe3, 0x9f, 0x8e, 0xce,
    0x7b, 0x7e, 0xdf, 0xef, 0x1d, 0xf9, 0xbd, 0xa7, 0x7e, 0xef, 0xd8, 0x3f,
    0x38, 0xf6, 0x0f, 0x9e, 0xfb, 0x87, 0xdd, 0x0b, 0xff, 0x6a, 0xf4, 0xa8,
    0xe7, 0x5f, 0xfb, 0xef, 0x46, 0x5d, 0xff, 0x2d, 0xfc, 0x7c, 0x31, 0x7a,
    0xd4, 0xd3, 0x73, 0xf9, 0xdd, 0xe8, 0xfc, 0xc2, 0xbf, 0x1c, 0x21, 0x04,
    0x87, 0x99, 0xff, 0xfb, 0xf2, 0xd7, 0xbf, 0x8d, 0xce, 0xcf, 0x1d, 0x9e,
    0x72, 0xea, 0xf8, 0xce, 0xbb, 0x94, 0x84, 0xb8, 0x38, 0xce, 0x85, 0x7f,
    0xee, 0x64, 0x49, 0x78, 0xfb, 0x31, 0xa2, 0x98, 0x73, 0x96, 0x84, 0xb7,
    0x44, 0xe0, 0x87, 0xc9, 0x82, 0xa4, 0x04, 0x80, 0x5a, 0xe7, 0x66, 0xe6,
    0x1b, 0x0a, 0x70, 0x7a, 0x23, 0x4d, 0xdd, 0x77, 0xf4, 0x46, 0xda, 0x75,
    0x73, 0x7a, 0xc5, 0xd2, 0xb5, 0x28, 0x9b, 0xd6, 0xdf, 0x56, 0x11, 0x21,
    0xd3, 0xcc, 0xf1, 0x9d, 0x37, 0xec, 0x8a, 0x92, 0x55, 0x1a, 0x53, 0xe2,
    0x42, 0x0a, 0xf6, 0x30, 0x0b, 0xa3, 0x4b, 0x0f, 0x0b, 0xcd, 0x92, 0x90,
    0x5f, 0x3a, 0xbe, 0xf3, 0x3d, 0xfc, 0x25, 0xe9, 0x5a, 0x66, 0x6b, 0x29,
    0x30, 0x47, 0x52, 0x21, 0x5f, 0x22, 0x12, 0xf3, 0x9d, 0x4f, 0x54, 0x48,
    0xd5, 0xc6, 0x7f, 0xff, 0x17, 0x11, 0x69, 0xc2, 0x62, 0x02, 0xf8, 0x6d,
    0x9d, 0xd7, 0x4b, 0xfe, 0x10, 0xc6, 0xb4, 0x59, 0xfa, 0xc3, 0xef, 0xbe,
    0xd7, 0x65, 0xc9, 0x1c, 0xb2, 0xb1, 0xd3, 0x9c, 0x2d, 0x96, 0x92, 0x42,
    0xcb, 0xa7, 0x80, 0x1e, 0x05, 0x93, 0xb7, 0xa4, 0x4c, 0x84, 0x12, 0x71,
    0x98, 0x5f, 0x36, 0xf2, 0x75, 0x12, 0xce, 0x8c, 0xca, 0x32, 0xdd, 0xf1,
    0x9d, 0x8f, 0x54, 0x12, 0x56, 0x7e, 0x43, 0x81, 0x9c, 0x26, 0x34, 0x14,
    0xd4, 0x2e, 0xf4, 0x41, 0x25, 0x91, 0xf4, 0x8a, 0xe6, 0x39, 0x83, 0x81,
    0x5c, 0xf8, 0xe1, 0x48, 0x8e, 0xc6, 0x25, 0x2a, 0x5b, 0x50, 0xa9, 0x51,
    0xe6, 0xf7, 0xb7, 0xa7, 0xb1, 0x2b, 0x3d, 0x3f, 0x87, 0xfc, 0xd0, 0x95,
    0x5e, 0x70, 0x15, 0x26, 0x6b, 0xea, 0xaf, 0x46, 0xae, 0xf4, 0xa9, 0x67,
    0xa7, 0x8d, 0x3e, 0xca, 0x9c, 0xf1, 0x85, 0x4b, 0x01, 0xa8, 0x3c, 0xff,
    0xa7, 0xb2, 0x46, 0xb4, 0xa4, 0xd1, 0x25, 0x8d, 0xfd, 0x18, 0x52, 0x74,
    0x21, 0x89, 0x85, 0x82, 0x9c, 0x66, 0x49, 0x18, 0x51, 0x77, 0xff, 0x7c,
    0xef, 0x64, 0xec, 0x7c, 0x7b, 0xb1, 0xbf, 0xf0, 0xe9, 0x68, 0xec, 0x6e,
    0x9c, 0x3d, 0x67, 0xe0, 0xec, 0x85, 0xab, 0x6c, 0xe8, 0xf8, 0xce, 0x09,
    0xfc, 0x4e, 0x24, 0xfc, 0x1c, 0xc3, 0xcf, 0x05, 0xfc, 0xfc, 0xd6, 0xf9,
    0x76, 0xe0, 0xec, 0xfd, 0xb4, 0x4e, 0x31, 0xfd, 0x5b, 0x48, 0x7f, 0x7c,
    0xf0, 0x7c, 0xe8, 0x14, 0xde, 0x39, 0xbd, 0xf0, 0x86, 0xe5, 0x54, 0xc2,
    0x38, 0x7e, 0x7d, 0x45, 0xb9, 0x04, 0x0c, 0x46, 0x39, 0xcd, 0x5d, 0x27,
    0x4b, 0x61, 0x85, 0xf2, 0x38, 0xbd, 0xe6, 0x8e, 0x2f, 0x47, 0xe3, 0x36,
    0xd1, 0x00, 0x98, 0xbd, 0x32, 0x44, 0x63, 0x48, 0x09, 0xe3, 0x42, 0x86,
    0x3c, 0xa2, 0xe9, 0x9c, 0xfc, 0xf8, 0xe9, 0xed, 0x9b, 0x53, 0x9e, 0xad,
    0xcd, 0xfa, 0x00, 0xc5, 0x28, 0xd1, 0x32, 0x82, 0xc8, 0xde, 0x9e, 0x0c,
    0x64, 0x98, 0x2f, 0xa8, 0x7c, 0x34, 0x1a, 0x21, 0x75, 0x99, 0x25, 0xeb,
    0xdc, 0xf5, 0x0a, 0xff, 0x51, 0xd7, 0xd3, 0x87, 0xe7, 0xfd, 0xc8, 0xee,
    0xf7, 0x34, 0x60, 0x3c, 0xa6, 0x37, 0xef, 0xe7, 0xae, 0xf4, 0xbe, 0xeb,
    0x19, 0xec, 0x79, 0xb5, 0xb7, 0x47, 0xc7, 0xdd, 0xc9, 0xf4, 0xd5, 0xe9,
    0x93, 0x0d, 0x2d, 0x88, 0xfb, 0xbb, 0xb3, 0xd3, 0xf7, 0xe4, 0xc9, 0x46,
    0x16, 0xde, 0x74, 0x30, 0x2d, 0x3f, 0xa6, 0xc5, 0x30, 0x14, 0xb7, 0x3c,
    0x22, 0x25, 0x1a, 0x9a, 0xc3, 0xc6, 0x8c, 0x36, 0x85, 0xc1, 0xc8, 0x1c,
    0x4f, 0xe6, 0x8f, 0x34, 0x8c, 0x69, 0x2e, 0x5c, 0x1a, 0x2c, 0xd5, 0x2f,
    0x6f, 0x78, 0xb6, 0xb7, 0xc7, 0x03, 0x41, 0xa5, 0xeb, 0xfc, 0xb9, 0xa3,
    0x09, 0x70, 0xa7, 0xc4, 0x0c, 0xfe, 0x99, 0xe7, 0xd3, 0x60, 0x96, 0xc6,
    0xb7, 0x65, 0xa9, 0x97, 0x8a, 0x7e, 0x77, 0x3e, 0xdd, 0x66, 0x00, 0xda,
    0x61, 0x96, 0x25, 0x2c, 0x42, 0xd2, 0xb5, 0xff, 0x77, 0x91, 0x72, 0xc7,
    0x4c, 0x8e, 0x8d, 0xc2, 0xeb, 0x90, 0x49, 0xa2, 0xd0, 0x9c, 0xf4, 0x37,
    0x41, 0x10, 0x50, 0x5f, 0x77, 0x3b, 0xe0, 0x85, 0x07, 0x64, 0x97, 0x05,
    0x42, 0x86, 0x72, 0x2d, 0x46, 0xa3, 0xd1, 0x61, 0xb7, 0x57, 0x51, 0x8f,
    0x2c, 0x4f, 0x57, 0x99, 0x74, 0x1d, 0xc3, 0x11, 0x68, 0x0e, 0x80, 0x64,
    0x25, 0xca, 0x82, 0xda, 0xd1, 0x68, 0x34, 0xe2, 0xeb, 0x24, 0xf1, 0xe4,
    0x32, 0x4f, 0xaf, 0x09, 0x4c, 0x10, 0xf1, 0xa5, 0xeb, 0x98, 0x09, 0x90,
    0x9c, 0xfe, 0xb4, 0x66, 0x39, 0x85, 0x0a, 0x7a, 0x41, 0xcf, 0x46, 0x91,
    0xdf, 0x40, 0x8c, 0x62, 0x17, 0x62, 0x84, 0xe9, 0xe3, 0x42, 0x7a, 0x85,
    0x26, 0xf5, 0x7a, 0x52, 0x2c, 0x80, 0xb9, 0xba, 0x38, 0x8a, 0x47, 0x2c,
    0x48, 0x2f, 0xad, 0x21, 0xfc, 0x01, 0x50, 0x3f, 0x0c, 0xe3, 0xee, 0x6e,
    0xfa, 0x81, 0xfe, 0xb4, 0x86, 0xb3, 0x3f, 0x0f, 0x59, 0x42, 0x63, 0xe2,
    0x3e, 0xd9, 0x98, 0x19, 0x17, 0xde, 0xd4, 0x17, 0xe5, 0xa0, 0x44, 0x45,
    0x3f, 0xd6, 0xb8, 0x71, 0x8f, 0x7a, 0xd5, 0xc6, 0x85, 0xae, 0x23, 0xd3,
    0x10, 0xc8, 0xfc, 0x90, 0x07, 0x92, 0xde, 0x48, 0xbd, 0x05, 0x23, 0x69,
    0x83, 0x24, 0xce, 0x7c, 0x22, 0x83, 0x15, 0x15, 0x22, 0x5c, 0xd0, 0x81,
    0x39, 0x61, 0x9e, 0xcf, 0x03, 0x21, 0x6f, 0x13, 0xd8, 0xc6, 0x3c, 0xa6,
    0xf9, 0x1b, 0x3a, 0x57, 0xb8, 0x69, 0x44, 0x27, 0xce, 0xe3, 0xf8, 0xe9,
    0x51, 0xf7, 0x60, 0xe6, 0x0c, 0x9c, 0xc7, 0xf3, 0xde, 0x51, 0xd8, 0x8f,
    0x1c, 0x9f, 0x07, 0x48, 0x83, 0xe0, 0xa0, 0xc0, 0xa9, 0x71, 0x1d, 0xb1,
    0x4c, 0xaf, 0x1d, 0xcf, 0x17, 0x54, 0x7e, 0x62, 0x2b, 0x9a, 0xae, 0xa5,
    0xeb, 0x7a, 0xa3, 0xb1, 0x5d, 0x2c, 0xa7, 0xab, 0xf4, 0x8a, 0x96, 0x25,
    0x0f, 0x8e, 0xba, 0x5d, 0xaf, 0x9a, 0xd0, 0xdf, 0x5d, 0xe9, 0x6d, 0xf4,
    0x3c, 0x65, 0x20, 0x73, 0xb6, 0x72, 0xbd, 0x40, 0xa6, 0x6f, 0xd2, 0x6b,
    0x9a, 0xbf, 0x0c, 0x05, 0x75, 0xad, 0xf3, 0xff, 0xb7, 0xa5, 0x94, 0x99,
    0x98, 0x0c, 0xfe, 0xba, 0xff, 0xd7, 0xfd, 0x7d, 0xbf, 0x86, 0x1a, 0xfe,
    0x1a, 0x24, 0x69, 0x14, 0x26, 0x7f, 0xdd, 0x9f, 0x3c, 0x69, 0xe4, 0x9c,
    0xff, 0x2d, 0xec, 0xfc, 0xa3, 0xdb, 0x79, 0xde, 0xb9, 0xf8, 0x6e, 0x7f,
    0xe1, 0x3b, 0x1d, 0x3b, 0xaf, 0xd3, 0x4e, 0xfa, 0x5b, 0xe7, 0xae, 0xf3,
    0x04, 0x52, 0x1d, 0x2f, 0x10, 0x09, 0x8b, 0xa8, 0xdb, 0xf5, 0x0f, 0x7a,
    0x76, 0x1d, 0xd5, 0x41, 0x35, 0x83, 0x8f, 0xae, 0x99, 0xc0, 0xe6, 0x92,
    0xf1, 0x78, 0xa0, 0xe9, 0x9a, 0x3a, 0xde, 0xa7, 0xf1, 0x20, 0x9d, 0x04,
    0x59, 0xce, 0x56, 0x61, 0x7e, 0x7b, 0xc6, 0x6e, 0x12, 0x26, 0xe9, 0x69,
    0x7c, 0x77, 0x07, 0x89, 0xea, 0x4b, 0x4c, 0x82, 0xf3, 0xee, 0xc5, 0x24,
    0x60, 0xf1, 0xdd, 0x9d, 0xa3, 0x0b, 0x3a, 0xfe, 0x8a, 0xc6, 0x2c, 0x04,
    0xb6, 0x6c, 0xe0, 0x38, 0x7e, 0x4e, 0x33, 0x1a, 0xca, 0x81, 0x33, 0x4f,
    0x73, 0x7a, 0x05, 0x68, 0x5e, 0xd1, 0x9d, 0x81, 0x93, 0xb1, 0x1b, 0x9a,
    0x08, 0xc7, 0x0f, 0xd7, 0x37, 0xef, 0x55, 0x52, 0xcf, 0x2f, 0x31, 0xfb,
    0x19, 0xcd, 0x23, 0xca, 0xe5, 0xa0, 0xd7, 0xed, 0xfa, 0x42, 0xd2, 0xcc,
    0x7c, 0x1f, 0xf9, 0xcb, 0x34, 0x89, 0x5f, 0xd1, 0x24, 0xbc, 0x7d, 0x2b,
    0x06, 0x87, 0xdd, 0xae, 0x6e, 0x1f, 0xb0, 0x7f, 0x7e, 0x15, 0x26, 0x6f,
    0xc5, 0xa0, 0x7f, 0xd4, 0xf5, 0x81, 0xfa, 0xe8, 0x9f, 0x25, 0xc1, 0x02,
    0x40, 0x98, 0x77, 0xbb, 0xdd, 0xae, 0x53, 0x54, 0xf3, 0xa7, 0xb4, 0xda,
    0xc2, 0x47, 0xf2, 0xee, 0x4e, 0x02, 0x9a, 0x3b, 0x53, 0x13, 0x21, 0x67,
    0xec, 0xe6, 0x0d, 0x93, 0xd4, 0xd9, 0x9e, 0x4c, 0xde, 0x5e, 0x1e, 0x38,
    0x77, 0x77, 0xfb, 0x7f, 0xd3, 0xdf, 0xee, 0x64, 0x00, 0x49, 0xde, 0x84,
    0xfc, 0x35, 0xfe, 0xee, 0xc9, 0x7e, 0x00, 0x1d, 0xbb, 0xd2, 0x5a, 0x6b,
    0x89, 0x7d, 0x19, 0x9c, 0xb8, 0xff, 0xb7, 0xc7, 0xe7, 0xdd, 0xce, 0xf3,
    0xb0, 0x33, 0xbf, 0xd8, 0x3c, 0x2d, 0x9e, 0xec, 0x33, 0x53, 0x61, 0x22,
    0xab, 0x91, 0xea, 0x43, 0xb4, 0x81, 0xac, 0x0f, 0x34, 0x1e, 0xbc, 0x5b,
    0xaf, 0x66, 0x34, 0x0f, 0x32, 0xb8, 0xd7, 0x9c, 0x72, 0xe9, 0x52, 0xbd,
    0xcd, 0x3d, 0xff, 0xc0, 0xf3, 0x7b, 0x4f, 0x3d, 0x9c, 0xed, 0xef, 0x72,
    0x4a, 0xf9, 0xce, 0xa2, 0x07, 0xfe, 0x51, 0x55, 0xf4, 0xfb, 0x64, 0x4d,
    0x77, 0x96, 0x3c, 0xf2, 0x9f, 0x61, 0x49, 0x6b, 0xb9, 0xb8, 0xb5, 0x5c,
    0x6e, 0x5a, 0x42, 0xc1, 0xdd, 0xdd, 0xf9, 0x85, 0x17, 0xac, 0xc2, 0xcc,
    0xe5, 0xa3, 0xf1, 0xf4, 0x24, 0xcd, 0xb0, 0xb0, 0xa2, 0x99, 0xce, 0x93,
    0x4d, 0xec, 0xf2, 0x80, 0xc5, 0x5e, 0xe1, 0x00, 0x42, 0x07, 0xc4, 0x16,
    0xb0, 0x78, 0xe2, 0x08, 0xbc, 0x48, 0xd0, 0xd8, 0x19, 0x38, 0x4e, 0x31,
    0x56, 0xa5, 0xe0, 0x52, 0x75, 0x77, 0xc7, 0x83, 0x65, 0x2a, 0xa4, 0x57,
    0x9c, 0xec, 0xab, 0x96, 0xc6, 0x53, 0x2f, 0xf8, 0x7b, 0xca, 0xb8, 0xeb,
    0x38, 0xde, 0xdd, 0xdd, 0xb7, 0x8d, 0xf6, 0x9d, 0xf1, 0x8b, 0x38, 0x26,
    0xa1, 0xbd, 0x2d, 0x64, 0xce, 0x72, 0x21, 0xcb, 0xea, 0xdf, 0x5a, 0x2c,
    0x2c, 0x2d, 0xef, 0x50, 0x72, 0xe4, 0x26, 0x8d, 0x19, 0xcc, 0x59, 0x22,
    0x69, 0xee, 0x66, 0xa3, 0x71, 0x16, 0xa4, 0x3c, 0x61, 0x9c, 0x7a, 0x41,
    0x42, 0xf9, 0x42, 0x2e, 0x7d, 0x3a, 0x72, 0xd3, 0x80, 0x01, 0x5d, 0xdc,
    0x52, 0x96, 0xf2, 0x70, 0x96, 0xd0, 0xb8, 0x2c, 0xcc, 0x47, 0x09, 0x4e,
    0x01, 0xa6, 0x33, 0x99, 0xc2, 0xc1, 0x1f, 0xec, 0xef, 0x3f, 0xd9, 0x54,
    0x89, 0x85, 0x3a, 0xf1, 0xfb, 0xd3, 0x81, 0xf3, 0x99, 0x87, 0x57, 0x21,
    0x4b, 0xa0, 0x01, 0x67, 0x18, 0xba, 0x4e, 0x94, 0x72, 0x4e, 0x15, 0x4b,
    0xe9, 0x05, 0x8c, 0x73, 0x9a, 0x03, 0x45, 0x1e, 0x4d, 0x4f, 0xd8, 0xf8,
    0x64, 0x9f, 0xe1, 0x3a, 0x25, 0xc1, 0x3a, 0x83, 0x9b, 0xd7, 0x7b, 0x1c,
    0xe2, 0x64, 0x0a, 0x2d, 0xab, 0x94, 0x02, 0xb8, 0x30, 0xf8, 0x64, 0x59,
    0x31, 0x1d, 0x58, 0x19, 0x77, 0x77, 0xce, 0x3b, 0x2a, 0xaf, 0xd3, 0xfc,
    0xd2, 0x29, 0x48, 0x3a, 0x9f, 0x43, 0xc5, 0xa9, 0x57, 0x4c, 0xfd, 0x66,
    0x8f, 0x88, 0xfa, 0xf0, 0x4a, 0x35, 0xad, 0xd2, 0x49, 0xd5, 0x90, 0xee,
    0xd3, 0x51, 0xcb, 0xe3, 0x0c, 0x1c, 0xdd, 0x98, 0x53, 0x4c, 0x4b, 0xea,
    0x78, 0x7e, 0xee, 0xbc, 0xc2, 0x3b, 0x34, 0x39, 0x3d, 0x73, 0xfc, 0x46,
    0x4d, 0x18, 0x5b, 0x7d, 0xd6, 0xcd, 0x12, 0xf6, 0x82, 0xb1, 0xac, 0x80,
    0x45, 0x42, 0x46, 0xef, 0x0d, 0x2c, 0x19, 0x09, 0xe3, 0x38, 0xa7, 0x42,
    0x38, 0x3e, 0xf7, 0x1f, 0xbe, 0xc8, 0x58, 0xdf, 0x06, 0x10, 0xb8, 0xaa,
    0xe7, 0x69, 0x92, 0xd0, 0x5c, 0x38, 0xfe, 0x14, 0xf8, 0x0c, 0xa2, 0x66,
    0xa4, 0x16, 0xb0, 0x05, 0xdb, 0x6a, 0x63, 0x0b, 0x82, 0x97, 0xcf, 0xa9,
    0xaf, 0x1a, 0x44, 0x36, 0x49, 0xd5, 0xa7, 0x05, 0x34, 0x39, 0x67, 0x8b,
    0x75, 0x4e, 0x63, 0x18, 0x44, 0xc6, 0xf8, 0x07, 0xba, 0x0a, 0xb3, 0x8c,
    0xf1, 0xc5, 0x07, 0x4d, 0xa9, 0x27, 0x0e, 0xb4, 0x9e, 0x43, 0x72, 0x45,
    0xbd, 0x01, 0xfa, 0x4d, 0x83, 0xaf, 0xe5, 0x92, 0xe6, 0x9c, 0x4a, 0x58,
    0x12, 0xaa, 0x7f, 0x4f, 0x02, 0x58, 0x9a, 0xcf, 0x99, 0xda, 0x66, 0x93,
    0x8a, 0x89, 0x1f, 0x33, 0x4a, 0xe3, 0xb7, 0xb3, 0x4c, 0x14, 0x04, 0xfe,
    0x4f, 0x6a, 0x05, 0xe6, 0xeb, 0x24, 0x79, 0xb5, 0xce, 0x12, 0x7a, 0x33,
    0x71, 0xe0, 0x37, 0x89, 0xf1, 0xc3, 0x19, 0x38, 0xcb, 0x30, 0x99, 0x9b,
    0xaf, 0x62, 0x3a, 0xb0, 0xbb, 0x02, 0x62, 0xbe, 0xce, 0xe9, 0x07, 0x1a,
    0x8a, 0x94, 0xdf, 0xdd, 0x39, 0x6f, 0x18, 0xbf, 0x24, 0x8a, 0x8f, 0x54,
    0x23, 0x7c, 0x4b, 0x57, 0x29, 0xa0, 0xf8, 0x24, 0x88, 0xe9, 0x22, 0x0f,
    0x63, 0x1a, 0xbf, 0x4d, 0x63, 0x3a, 0x71, 0x5e, 0xe9, 0x2f, 0x92, 0xd3,
    0x08, 0xb8, 0xee, 0x5b, 0xbc, 0x13, 0x38, 0x00, 0x82, 0x6f, 0x43, 0xb9,
    0x0c, 0xf2, 0x74, 0xcd, 0x63, 0xd7, 0x4d, 0x82, 0x15, 0x36, 0x30, 0xc1,
    0xeb, 0x71, 0xce, 0xc3, 0xe4, 0x87, 0x9c, 0xd2, 0xbb, 0xbb, 0xae, 0xb7,
    0xdf, 0xeb, 0xf6, 0x0f, 0xbd, 0x82, 0xfc, 0xe1, 0x7b, 0x62, 0xb2, 0xc8,
    0x3c, 0xa7, 0xd4, 0x2c, 0xcd, 0x0f, 0x2c, 0x5f, 0x5d, 0x87, 0x39, 0x42,
    0xcb, 0x5c, 0xff, 0xfe, 0x23, 0xcd, 0x81, 0xd7, 0xb9, 0xbb, 0x73, 0x7e,
    0xf9, 0xf9, 0x3f, 0xcc, 0x08, 0x3f, 0x67, 0x92, 0xad, 0x28, 0x6e, 0x0a,
    0xf6, 0x3c, 0x4f, 0xd2, 0x34, 0xb7, 0x7b, 0x5e, 0x63, 0x81, 0xb7, 0x02,
    0x7b, 0x7d, 0x4a, 0xa1, 0xd3, 0x15, 0xe3, 0x6b, 0x49, 0x05, 0xf6, 0x75,
    0x01, 0x87, 0x50, 0x31, 0x32, 0x9d, 0x45, 0xce, 0xe2, 0xda, 0x29, 0x64,
    0x88, 0xe4, 0xdc, 0xf3, 0xcc, 0x5f, 0xfa, 0x37, 0x17, 0x1e, 0x20, 0xbb,
    0x98, 0x5d, 0x11, 0x3c, 0x34, 0x23, 0x53, 0x2b, 0x0a, 0xf3, 0xd8, 0x19,
    0x9f, 0x88, 0x55, 0x98, 0x24, 0x78, 0x5e, 0x33, 0x40, 0x65, 0xe6, 0xf3,
    0x66, 0x32, 0x3d, 0x09, 0x09, 0xdc, 0x8c, 0x15, 0x6a, 0xbc, 0x01, 0xbc,
    0x88, 0xd2, 0x9f, 0x91, 0xf3, 0x3e, 0xa3, 0x70, 0xd6, 0x62, 0x77, 0xe9,
    0x15, 0xce, 0x58, 0xff, 0x38, 0xd9, 0x0f, 0xc7, 0xd3, 0xc1, 0xf4, 0x44,
    0xc8, 0x3c, 0xe5, 0x0b, 0x53, 0xb4, 0x5d, 0x4a, 0xe5, 0x8f, 0xa7, 0xc5,
    0xc9, 0x7e, 0xcc, 0xae, 0x6c, 0x9c, 0x39, 0x34, 0x1c, 0xde, 0xf9, 0x39,
    0xf0, 0xb8, 0x70, 0xa8, 0x89, 0x5c, 0x52, 0xc2, 0x35, 0x46, 0xf0, 0x1f,
    0x3d, 0xaa, 0x1f, 0x41, 0xdf, 0x79, 0x6c, 0xf2, 0x60, 0x49, 0x5b, 0xf8,
    0x15, 0x6a, 0xa4, 0x16, 0x0f, 0xa0, 0xd1, 0x9e, 0xf3, 0xd8, 0x24, 0x55,
    0xd5, 0x38, 0x41, 0xa4, 0xe9, 0xf8, 0x74, 0xdc, 0xf5, 0x9d, 0xc7, 0x0a,
    0x83, 0xea, 0x55, 0xfe, 0x69, 0xcd, 0xa2, 0xcb, 0x8e, 0x90, 0x61, 0x2e,
    0x6b, 0xab, 0x2c, 0x6a, 0xab, 0xec, 0xff, 0x2b, 0xae, 0x73, 0x58, 0xad,
    0x32, 0xcd, 0xc8, 0x93, 0xcd, 0x72, 0xe2, 0xc4, 0x29, 0x62, 0x20, 0xa7,
    0x70, 0xca, 0xf5, 0xbc, 0x29, 0x9c, 0xf1, 0x09, 0x60, 0xc9, 0xe5, 0xc4,
    0xf9, 0xe5, 0xff, 0xfe, 0xbb, 0x33, 0xf8, 0xd7, 0xef, 0x7a, 0x85, 0xc2,
    0x9b, 0x99, 0x5a, 0xc9, 0xd6, 0xb2, 0x44, 0x23, 0x61, 0x23, 0xf3, 0xf3,
    0x5e, 0x79, 0xdc, 0x11, 0x12, 0xa8, 0x5c, 0x67, 0x9d, 0x2c, 0x4f, 0x17,
    0x88, 0x75, 0xbc, 0x1a, 0x97, 0x0a, 0x5c, 0xfa, 0xc1, 0xc4, 0xf9, 0x40,
    0xc3, 0xf8, 0x16, 0x61, 0x3d, 0x02, 0xe4, 0x4a, 0x0e, 0x08, 0xd6, 0x22,
    0x30, 0x52, 0x31, 0x2d, 0x1a, 0xd7, 0x97, 0x4f, 0xae, 0xb7, 0x91, 0xf9,
    0xed, 0x26, 0x31, 0xb7, 0x08, 0xd7, 0xd9, 0x0f, 0x33, 0xb6, 0x0f, 0xd0,
    0x43, 0x1d, 0xcf, 0xb7, 0x89, 0x92, 0x45, 0xa1, 0x46, 0xe5, 0xcf, 0x09,
    0x62, 0xb8, 0xc9, 0xf9, 0x86, 0xc5, 0x83, 0x8a, 0xe9, 0xe2, 0xc8, 0x6f,
    0x6d, 0xe3, 0x51, 0xfc, 0x20, 0x08, 0xca, 0xca, 0xc5, 0xc5, 0xe0, 0xfc,
    0xc2, 0xf3, 0x81, 0x0e, 0xfa, 0xff, 0xe2, 0x7a, 0xfe, 0x8f, 0x6e, 0xa2,
    0xc9, 0x9a, 0x57, 0x44, 0x21, 0xde, 0x67, 0xbc, 0x0d, 0x30, 0xea, 0x8f,
    0xba, 0x36, 0xe1, 0xff, 0xd1, 0x95, 0xa3, 0xf3, 0x0b, 0x6f, 0x23, 0x83,
    0x79, 0x9a, 0xbf, 0x0e, 0xa3, 0xa5, 0x8b, 0xd2, 0x9a, 0xf2, 0x8e, 0xc7,
    0x46, 0xbb, 0xae, 0xd6, 0x53, 0xf6, 0x64, 0xc3, 0x8b, 0x0e, 0xde, 0x38,
    0x99, 0xbc, 0x9d, 0x7a, 0x43, 0xb6, 0xb7, 0xe7, 0x32, 0x8b, 0xc7, 0x96,
    0xe9, 0x62, 0x91, 0x50, 0xd7, 0x51, 0x97, 0x52, 0x80, 0x2d, 0xaa, 0x2f,
    0xa8, 0x9e, 0xcf, 0x02, 0x05, 0xef, 0x26, 0x65, 0xe2, 0x18, 0xc9, 0xa9,
    0x2e, 0x3d, 0x28, 0x13, 0x18, 0xd7, 0x49, 0x15, 0xbc, 0x2b, 0xce, 0xc6,
    0xa5, 0x01, 0x85, 0x0b, 0xf2, 0x47, 0xb8, 0xb1, 0xf0, 0x08, 0xd1, 0xcd,
    0x78, 0x3c, 0xee, 0xfa, 0xd1, 0xe8, 0x77, 0xe7, 0xfc, 0x02, 0x2e, 0x3a,
    0xf0, 0x77, 0x24, 0x7c, 0xd8, 0xcf, 0xab, 0x94, 0xc5, 0xa4, 0x7b, 0x77,
    0x27, 0x4e, 0x22, 0x23, 0xe3, 0xd2, 0xe2, 0xc7, 0x11, 0xe2, 0x93, 0x15,
    0xe3, 0xae, 0xe8, 0x44, 0xfe, 0x21, 0x4e, 0x24, 0xc3, 0xb9, 0xa8, 0x4b,
    0x88, 0xa0, 0xf2, 0x2c, 0x4f, 0x33, 0x9a, 0xcb, 0x5b, 0xd7, 0xe9, 0x74,
    0xb2, 0x75, 0x22, 0xa8, 0x70, 0x7c, 0x7d, 0x5b, 0xc9, 0x3c, 0x98, 0x4d,
    0xfb, 0x66, 0x31, 0x4f, 0x42, 0xb1, 0x74, 0x20, 0x2f, 0x9d, 0xcf, 0x05,
    0x95, 0x7f, 0x52, 0x62, 0xe7, 0xe6, 0x55, 0x45, 0x17, 0xf3, 0x0a, 0xaf,
    0x09, 0x4f, 0x21, 0xb0, 0x34, 0x70, 0x59, 0x7b, 0xe1, 0x6d, 0x5e, 0x80,
    0x28, 0x0e, 0x80, 0xcb, 0xb0, 0x38, 0x75, 0x10, 0xd3, 0xa7, 0xcf, 0x1b,
    0x9a, 0x3d, 0x1f, 0x49, 0x8b, 0xa7, 0x69, 0xc3, 0xc2, 0xa6, 0x98, 0x33,
    0x1e, 0x26, 0xc9, 0x2d, 0x34, 0xdc, 0x2b, 0x2c, 0x78, 0xf8, 0x17, 0x8b,
    0x8d, 0xaa, 0xd3, 0x4a, 0x38, 0x34, 0xfa, 0xbb, 0x13, 0xa5, 0x6b, 0x2e,
    0x1b, 0x67, 0x06, 0x88, 0x6d, 0x49, 0x4e, 0xd3, 0x39, 0x79, 0xb2, 0x49,
    0x83, 0x55, 0x78, 0x73, 0x56, 0x36, 0xd1, 0x7b, 0x6a, 0xd3, 0x52, 0xe4,
    0x50, 0x44, 0x78, 0x45, 0xe3, 0x4e, 0x89, 0x5c, 0x6c, 0x5c, 0x21, 0x11,
    0x57, 0xd0, 0x12, 0x10, 0xf9, 0x36, 0xae, 0x8e, 0xc7, 0x2e, 0x1b, 0x8d,
    0x59, 0xc0, 0xe2, 0xd1, 0x68, 0x44, 0x81, 0x17, 0xbd, 0xbb, 0x2b, 0xa5,
    0xb8, 0x75, 0x14, 0x8e, 0x3d, 0x29, 0x0d, 0x80, 0x33, 0xfe, 0x27, 0x82,
    0xff, 0x9d, 0x88, 0x2c, 0xe4, 0x36, 0x92, 0xa7, 0x9d, 0x38, 0x95, 0xe4,
    0xc9, 0x86, 0x6b, 0x2e, 0xd1, 0x62, 0x87, 0x1c, 0x40, 0x40, 0xfb, 0x50,
    0x61, 0x6b, 0x6d, 0xad, 0x5b, 0x60, 0x7c, 0x9e, 0x3a, 0xe3, 0x93, 0x19,
    0x62, 0x6f, 0xaa, 0xd9, 0x5e, 0x5a, 0xb2, 0xbd, 0x33, 0x43, 0x3d, 0x4e,
    0xcf, 0x88, 0x2a, 0xa1, 0x72, 0x80, 0x7d, 0x78, 0xfb, 0xe2, 0xa5, 0x4e,
    0x5b, 0x85, 0x11, 0xf0, 0xca, 0xf8, 0xc7, 0x59, 0xf3, 0x4b, 0x0e, 0x74,
    0xda, 0x2b, 0x4e, 0x66, 0xf9, 0xd8, 0x1a, 0xda, 0x54, 0x71, 0xd6, 0x40,
    0x8e, 0xef, 0xee, 0x34, 0xaa, 0xf2, 0x34, 0x9f, 0xc8, 0x83, 0x24, 0x94,
    0x94, 0x47, 0xb7, 0x48, 0x0d, 0x0b, 0xb2, 0x12, 0xd3, 0x01, 0x14, 0xd6,
    0x77, 0x7c, 0xe7, 0x5d, 0x2a, 0x89, 0xe6, 0x02, 0x41, 0xce, 0x50, 0x52,
    0xb1, 0x2f, 0x4f, 0x50, 0x09, 0x45, 0x05, 0xcc, 0x71, 0x2d, 0x65, 0xca,
    0x49, 0x1c, 0xca, 0xb0, 0x83, 0x17, 0x43, 0x45, 0xbb, 0xa8, 0xba, 0x11,
    0x8c, 0xff, 0xc8, 0xe8, 0x35, 0xc1, 0xf4, 0x93, 0x7d, 0x55, 0xb4, 0x5e,
    0x85, 0xc6, 0x4c, 0xd6, 0x6b, 0xbc, 0x8e, 0x99, 0x6c, 0x95, 0x35, 0xbd,
    0x87, 0x7c, 0x41, 0x73, 0x47, 0x55, 0x55, 0x27, 0xac, 0x5e, 0xf9, 0x03,
    0xa6, 0x55, 0xd5, 0xad, 0x89, 0x68, 0xb2, 0x59, 0xd4, 0xef, 0x1a, 0x16,
    0x6c, 0xd0, 0x55, 0x26, 0x6f, 0x3b, 0x0a, 0x4d, 0xc3, 0xde, 0xbd, 0x4b,
    0xc9, 0x0e, 0x96, 0x92, 0x20, 0x18, 0xa9, 0x7d, 0x84, 0xf6, 0x5f, 0x31,
    0x81, 0x5c, 0x11, 0x49, 0x39, 0x25, 0xe1, 0x5a, 0xa6, 0xab, 0x50, 0xb2,
    0x08, 0x0e, 0x16, 0x49, 0x73, 0xe0, 0x29, 0x09, 0x93, 0x82, 0x9c, 0x9e,
    0x19, 0xe6, 0x36, 0xd0, 0xe3, 0x52, 0x43, 0xfa, 0x76, 0xeb, 0x11, 0x68,
    0x29, 0x63, 0x9c, 0xf3, 0x6a, 0x8d, 0x2f, 0x1c, 0xaf, 0x44, 0xd4, 0x74,
    0x34, 0xa6, 0x41, 0xca, 0xa3, 0x84, 0x45, 0x97, 0x23, 0x90, 0x6f, 0xfc,
    0xc9, 0xa5, 0x01, 0x14, 0x15, 0x14, 0x44, 0x2a, 0x31, 0x0b, 0x3d, 0xef,
    0xab, 0x7a, 0x80, 0x2d, 0xb9, 0xb7, 0x83, 0xcf, 0x16, 0xfb, 0xac, 0x4e,
    0x1f, 0x07, 0xa9, 0x8a, 0x3e, 0x7d, 0xa6, 0x6b, 0x68, 0xc6, 0xfb, 0xca,
    0xae, 0xd5, 0x96, 0xde, 0xdb, 0x79, 0x4a, 0xad, 0xe9, 0xa9, 0xf2, 0x9e,
    0x75, 0x09, 0xff, 0xec, 0x4a, 0x14, 0x1d, 0x5a, 0x58, 0x6a, 0x9e, 0xe6,
    0x2b, 0xc7, 0x0b, 0x96, 0x2c, 0x8e, 0x29, 0x07, 0x25, 0x40, 0x23, 0xaf,
    0x83, 0x24, 0xa8, 0x81, 0xc6, 0x24, 0xde, 0x61, 0x01, 0x16, 0x6b, 0xb4,
    0x76, 0x80, 0x5c, 0x4f, 0x8d, 0xfa, 0xae, 0xb0, 0xb5, 0x8e, 0x12, 0xa9,
    0x74, 0x58, 0xec, 0xf8, 0x52, 0xc9, 0x4b, 0x1c, 0xcf, 0xe4, 0xad, 0xc2,
    0x08, 0x52, 0xd5, 0x49, 0xae, 0x92, 0x01, 0x2d, 0x40, 0xba, 0x42, 0x0f,
    0x56, 0x06, 0x60, 0x03, 0xc8, 0x80, 0xbf, 0xb5, 0x8c, 0xb5, 0x00, 0x09,
    0x8b, 0x0c, 0xe0, 0xaf, 0xae, 0x05, 0x04, 0xa9, 0x2a, 0x50, 0x4a, 0x19,
    0x41, 0x0c, 0xe4, 0xb7, 0x17, 0x41, 0x44, 0x00, 0xc2, 0xa7, 0x5c, 0xa6,
    0x70, 0x30, 0xdd, 0xcd, 0x8c, 0x2e, 0xc3, 0x2b, 0x06, 0xc2, 0x13, 0xb1,
    0x4a, 0x53, 0xb9, 0x74, 0xfc, 0x59, 0x92, 0x46, 0x97, 0x03, 0x87, 0xd3,
    0x30, 0xa7, 0x42, 0x3a, 0x6d, 0x7a, 0xf4, 0x27, 0xe0, 0x20, 0x2a, 0x1a,
    0x44, 0x4b, 0x1a, 0x34, 0x45, 0x1a, 0x84, 0x10, 0x37, 0x31, 0x02, 0xa6,
    0xd1, 0x93, 0x0d, 0xe5, 0x51, 0x1a, 0xd3, 0xcf, 0x1f, 0x4e, 0x5f, 0xa6,
    0xab, 0x2c, 0xe5, 0xa0, 0xc5, 0x94, 0x5e, 0x31, 0xf5, 0x86, 0x97, 0x28,
    0x95, 0x05, 0x11, 0xe5, 0xd0, 0xe0, 0xf6, 0x26, 0x5c, 0x55, 0x58, 0x5d,
    0x7a, 0x40, 0x78, 0xb0, 0xf1, 0xad, 0xfb, 0xc5, 0x27, 0x46, 0xb8, 0x30,
    0x31, 0xcb, 0x56, 0xdb, 0x25, 0x80, 0x42, 0xd0, 0xa6, 0xd4, 0xa9, 0x0b,
    0x0d, 0x54, 0x22, 0x12, 0x19, 0x06, 0x6c, 0x68, 0xa2, 0xae, 0xe0, 0x0c,
    0x90, 0x61, 0xc2, 0x9a, 0x92, 0x89, 0x84, 0x19, 0x64, 0xb1, 0x5a, 0x03,
    0xca, 0x04, 0x1c, 0xa1, 0x5a, 0xc0, 0xd2, 0x78, 0x92, 0x8d, 0xd6, 0xa7,
    0xd9, 0x53, 0x99, 0xfe, 0x3f, 0xe9, 0xac, 0x6c, 0xa4, 0xea, 0x4f, 0xad,
    0x49, 0x16, 0x72, 0x9a, 0xd4, 0xa0, 0xfc, 0xd6, 0x35, 0x1c, 0x1f, 0x05,
    0x8e, 0x8f, 0x2a, 0x8e, 0xaf, 0xb1, 0x99, 0x20, 0xc3, 0xdc, 0x80, 0xe6,
    0x4d, 0x82, 0x62, 0xac, 0xd2, 0xc5, 0x52, 0x20, 0xdd, 0x8d, 0x4b, 0x32,
    0xf0, 0x20, 0x97, 0xc1, 0x32, 0x14, 0x0a, 0xdb, 0x7a, 0x00, 0x03, 0x6a,
    0x0f, 0xe1, 0xdb, 0xff, 0x7a, 0x30, 0x50, 0x48, 0x7b, 0xea, 0x79, 0xbe,
    0x1c, 0x3d, 0xea, 0x1a, 0x96, 0x44, 0xee, 0xed, 0xb9, 0x4f, 0x5c, 0x0f,
    0xc6, 0xdf, 0x82, 0xbd, 0xb4, 0x26, 0x62, 0xdb, 0x8d, 0x85, 0xa4, 0x12,
    0xb2, 0x03, 0x97, 0x91, 0xaf, 0xdc, 0xe9, 0x0f, 0x29, 0x0c, 0x83, 0x3c,
    0xd9, 0x50, 0x03, 0x25, 0x9a, 0xc9, 0x2e, 0x26, 0x44, 0x5d, 0xf2, 0xc9,
    0x5a, 0x30, 0xbe, 0x20, 0x4c, 0x92, 0xd5, 0x5a, 0x48, 0x32, 0xa3, 0x24,
    0xa7, 0x6a, 0xf0, 0x34, 0x56, 0xd2, 0xa6, 0x60, 0xaa, 0x66, 0x5c, 0xe7,
    0xb8, 0xcc, 0x00, 0xf6, 0x15, 0x26, 0x72, 0xfc, 0xcd, 0x8a, 0xca, 0x65,
    0x1a, 0x0f, 0x9c, 0xb3, 0xf7, 0x1f, 0x3f, 0x39, 0x3e, 0xe8, 0x1f, 0x06,
    0xbf, 0xff, 0xf8, 0xfe, 0x5d, 0x20, 0x90, 0x4d, 0x64, 0xf3, 0x5b, 0x77,
    0x53, 0x4a, 0x5f, 0x65, 0xe1, 0x15, 0x9e, 0x7f, 0x19, 0xc4, 0x34, 0xa1,
    0x28, 0x3d, 0xd4, 0x8b, 0xf8, 0xc6, 0xf5, 0xfc, 0xb5, 0x5b, 0x13, 0x68,
    0xcc, 0xd3, 0x7c, 0x91, 0x4a, 0x49, 0x81, 0x03, 0x50, 0xbb, 0xca, 0x61,
    0x57, 0x79, 0x83, 0x8f, 0xcf, 0x6b, 0x0b, 0x74, 0x09, 0xcc, 0xba, 0x6b,
    0xb4, 0x39, 0xa7, 0xb1, 0x11, 0xda, 0xbb, 0xae, 0x0c, 0x40, 0x1c, 0x0c,
    0x52, 0xcf, 0x9a, 0xe2, 0x72, 0x42, 0x27, 0x15, 0x9c, 0x0e, 0xe8, 0x44,
    0x1f, 0x0f, 0xaf, 0x12, 0x00, 0xb2, 0xed, 0x02, 0x40, 0xe6, 0x21, 0x5f,
    0xd4, 0x96, 0xe7, 0x55, 0x43, 0xfb, 0x23, 0x1c, 0x75, 0x9f, 0xfb, 0xcc,
    0x8c, 0x4f, 0x8c, 0xe0, 0xee, 0x20, 0x8b, 0x0e, 0x88, 0x57, 0xa6, 0xdb,
    0xb8, 0x35, 0xad, 0x7a, 0x55, 0xe4, 0x5e, 0x7d, 0x40, 0x77, 0xa2, 0xd0,
    0xac, 0x5b, 0xbb, 0xac, 0xc6, 0x0d, 0xc0, 0x20, 0xe9, 0x7b, 0xf2, 0xae,
    0x82, 0x78, 0xf7, 0x2f, 0x59, 0x9c, 0x24, 0x9c, 0xd1, 0x64, 0xfc, 0x02,
    0xb3, 0x4e, 0x94, 0xe4, 0x92, 0xb0, 0x58, 0x75, 0xd6, 0x81, 0xa5, 0x82,
    0x26, 0xff, 0x4d, 0xdf, 0x5b, 0x23, 0x3f, 0x53, 0xb2, 0x81, 0xe6, 0x3a,
    0x44, 0x28, 0x02, 0x65, 0x66, 0x6d, 0xa3, 0xb6, 0x10, 0x34, 0xdb, 0x2a,
    0xf4, 0x04, 0xc6, 0x0b, 0x0b, 0x8e, 0x4f, 0xf6, 0xd5, 0x48, 0x6a, 0xe3,
    0x52, 0xf3, 0x9f, 0x33, 0x9a, 0xc4, 0x23, 0x47, 0xed, 0xa5, 0x33, 0x7e,
    0x45, 0x85, 0x64, 0x1c, 0xb5, 0x57, 0x36, 0x3d, 0x6a, 0x0d, 0xde, 0x94,
    0x7f, 0xb2, 0xe1, 0xd4, 0x65, 0x15, 0x28, 0x7c, 0xa9, 0x4f, 0xc3, 0x0e,
    0x67, 0x21, 0xef, 0xf4, 0x9d, 0xda, 0x10, 0xf0, 0x6c, 0x3b, 0x63, 0x54,
    0x57, 0x03, 0x83, 0x63, 0x20, 0xe6, 0x04, 0x2f, 0x17, 0x55, 0xd7, 0xaa,
    0x5c, 0x0d, 0x4c, 0x82, 0x52, 0x77, 0x00, 0x62, 0x11, 0xa8, 0xa4, 0xcb,
    0xce, 0x59, 0x42, 0x85, 0x43, 0x50, 0x8f, 0x01, 0x2a, 0x00, 0x9a, 0x8f,
    0x9c, 0x97, 0xcb, 0x34, 0x15, 0xd8, 0x03, 0xda, 0x03, 0x85, 0x8a, 0x8f,
    0x24, 0x50, 0x14, 0x49, 0x27, 0x98, 0xdf, 0xc0, 0xb8, 0xa0, 0x99, 0xaa,
    0x57, 0xd5, 0xd2, 0xf8, 0xc9, 0x26, 0xa7, 0x0a, 0xb3, 0x9a, 0x22, 0xad,
    0x89, 0xda, 0x7c, 0x2d, 0x4e, 0xad, 0xb3, 0xa4, 0x49, 0x46, 0xb6, 0x4d,
    0x19, 0xcf, 0xc1, 0x47, 0x49, 0x33, 0x67, 0xfc, 0x59, 0x50, 0x41, 0xc2,
    0x24, 0x5b, 0x86, 0x33, 0x8a, 0x5c, 0x9e, 0xa2, 0x00, 0x04, 0x55, 0x54,
    0x24, 0xe4, 0x31, 0xb9, 0xce, 0xc3, 0x4c, 0x90, 0x50, 0x12, 0xca, 0x40,
    0xf2, 0x46, 0x28, 0x8f, 0x83, 0x06, 0x4b, 0xdd, 0xde, 0x57, 0xa5, 0xe7,
    0x70, 0xc6, 0xca, 0xae, 0x40, 0x83, 0x8d, 0x6a, 0xba, 0xb5, 0xa7, 0xa6,
    0x6c, 0x03, 0x00, 0x8d, 0x0a, 0x06, 0x61, 0x50, 0x95, 0x81, 0x13, 0x6e,
    0x92, 0x9b, 0xd0, 0xf8, 0x26, 0x4d, 0x33, 0xb2, 0xe6, 0x92, 0x25, 0x04,
    0x8c, 0x09, 0x32, 0xe0, 0x68, 0x35, 0x6c, 0x36, 0x1a, 0x4e, 0x79, 0x44,
    0x9b, 0xad, 0x62, 0x5a, 0xb3, 0xc9, 0xf7, 0x3c, 0xa2, 0x55, 0x23, 0xbf,
    0x1a, 0xc0, 0x94, 0x1e, 0x7a, 0x8c, 0xb6, 0x07, 0xca, 0xde, 0xc0, 0x3e,
    0xc0, 0x98, 0xdb, 0xc9, 0xd3, 0x6b, 0x10, 0x05, 0xd5, 0x21, 0x4e, 0x1b,
    0x6a, 0x29, 0xf3, 0x2a, 0xfd, 0x51, 0x03, 0xbf, 0x52, 0x5b, 0x74, 0x77,
    0x57, 0x2a, 0x61, 0x00, 0x12, 0xc3, 0x9c, 0x85, 0x1d, 0x1c, 0x17, 0x94,
    0xe4, 0x05, 0x91, 0x55, 0xdf, 0x08, 0x67, 0x4a, 0xab, 0xd5, 0xe8, 0xa9,
    0x83, 0x6d, 0x2b, 0x19, 0x9d, 0x16, 0x03, 0xec, 0xe8, 0x22, 0x90, 0xe9,
    0xe7, 0x2c, 0x33, 0x5a, 0x43, 0x54, 0x7d, 0x60, 0x83, 0xfa, 0x0a, 0xb0,
    0x03, 0x14, 0x9d, 0xf1, 0x0b, 0xd0, 0x51, 0x53, 0x41, 0x64, 0x4a, 0xc2,
    0x24, 0x21, 0x4a, 0xa7, 0xa6, 0x37, 0xad, 0x34, 0x06, 0x09, 0x88, 0x96,
    0x40, 0x0b, 0x14, 0xfb, 0xc5, 0x15, 0x66, 0xf8, 0x56, 0x90, 0x17, 0xf1,
    0x8a, 0x71, 0x26, 0x64, 0x1e, 0x4a, 0xb8, 0x8b, 0x44, 0x78, 0x67, 0xaf,
    0x2e, 0x20, 0x5f, 0x42, 0x37, 0x6a, 0x98, 0xce, 0x58, 0xa9, 0xf0, 0x5a,
    0x90, 0x68, 0xb2, 0x1b, 0x00, 0xa3, 0x75, 0x7f, 0xe3, 0x33, 0xfc, 0xbb,
    0x0b, 0xac, 0xc2, 0xf5, 0x8d, 0x82, 0x2a, 0xd5, 0x0a, 0x5a, 0x32, 0xad,
    0x6f, 0x5a, 0x40, 0xf5, 0x62, 0x7d, 0xa3, 0x8d, 0x59, 0x76, 0x35, 0xa4,
    0xba, 0x7b, 0xc1, 0xe3, 0x17, 0xed, 0x16, 0x6b, 0x79, 0xcd, 0xa6, 0xd5,
    0xf8, 0xc8, 0x77, 0x96, 0x40, 0x82, 0xbc, 0x58, 0xdf, 0x3c, 0x14, 0x84,
    0xed, 0x85, 0x82, 0x91, 0x5b, 0x43, 0x25, 0x8f, 0x9b, 0xb0, 0x89, 0xb3,
    0x55, 0x90, 0xc9, 0x51, 0x6e, 0xe5, 0x80, 0x5c, 0x7a, 0xe4, 0xf4, 0x1c,
    0xb2, 0x0a, 0x6f, 0x46, 0xce, 0xb1, 0x05, 0xaa, 0x2c, 0x28, 0xb5, 0xa6,
    0x93, 0x49, 0xaf, 0x40, 0x08, 0xfc, 0xe2, 0x08, 0x2a, 0xb3, 0x99, 0x71,
    0x65, 0x69, 0xf3, 0x4d, 0x73, 0x18, 0x55, 0xa9, 0x6d, 0x83, 0xe9, 0xea,
    0xc1, 0xf4, 0xba, 0xdd, 0xda, 0x70, 0x9a, 0x8a, 0xdb, 0xc9, 0xa4, 0xd7,
    0xed, 0x3e, 0x70, 0x5c, 0x02, 0xd1, 0xe6, 0xcb, 0x25, 0xdc, 0xdd, 0x49,
    0x46, 0x73, 0x94, 0x9b, 0xb6, 0x07, 0x86, 0xc5, 0xee, 0x5b, 0xa0, 0xe6,
    0x98, 0x2c, 0xbd, 0xf1, 0x64, 0x72, 0xd4, 0x1e, 0x4c, 0x8d, 0xfe, 0x53,
    0x19, 0xb2, 0x44, 0x34, 0x78, 0x00, 0x30, 0x9a, 0xe0, 0x11, 0x8d, 0xeb,
    0xd8, 0x47, 0xb2, 0x15, 0xe3, 0x0b, 0x90, 0xf0, 0xaf, 0x57, 0x20, 0x6c,
    0x1d, 0xff, 0x98, 0x26, 0x31, 0x22, 0x76, 0x50, 0x3b, 0x13, 0x95, 0x7d,
    0xb2, 0x6f, 0x72, 0x1f, 0xc6, 0x5b, 0xd4, 0x7a, 0x00, 0x02, 0xe7, 0xa8,
    0x66, 0x67, 0x14, 0xd0, 0x33, 0x51, 0x78, 0x95, 0xac, 0x44, 0x73, 0x55,
    0xb0, 0xe8, 0xb6, 0x55, 0x39, 0x32, 0x5b, 0x75, 0xd4, 0x6d, 0xac, 0x8b,
    0xa5, 0x41, 0x9f, 0x4c, 0x0e, 0x1f, 0xbc, 0x4d, 0x4c, 0xab, 0xd8, 0x41,
    0x88, 0x82, 0x83, 0xa1, 0x4a, 0xcf, 0x23, 0xb6, 0x41, 0x10, 0x16, 0xfc,
    0xba, 0x61, 0x35, 0x15, 0xf9, 0x93, 0x49, 0xff, 0xe8, 0xa1, 0x63, 0x43,
    0x6b, 0xb3, 0x31, 0x98, 0xa4, 0x6d, 0x19, 0x0f, 0x66, 0x7e, 0x01, 0x98,
    0x1b, 0x63, 0x51, 0xf6, 0x03, 0x3b, 0x46, 0xa0, 0x51, 0xf2, 0xbe, 0x86,
    0x19, 0x48, 0x34, 0xb2, 0xa3, 0x92, 0x99, 0xbd, 0xad, 0xc9, 0x47, 0x95,
    0x50, 0x55, 0x71, 0x87, 0x4a, 0x6c, 0xee, 0x6e, 0x30, 0x6d, 0x40, 0x7d,
    0xb4, 0x8c, 0x02, 0xa3, 0xa1, 0x52, 0xa5, 0x8c, 0x22, 0x0d, 0xcc, 0x2e,
    0x15, 0xcb, 0x70, 0xb7, 0xc5, 0x94, 0x07, 0x89, 0x54, 0x8f, 0x95, 0xce,
    0x45, 0xa0, 0xec, 0x54, 0x55, 0x43, 0xd9, 0x55, 0x75, 0xfd, 0x33, 0xc5,
    0xc7, 0x5d, 0x28, 0x12, 0xc6, 0x71, 0x47, 0x69, 0x69, 0x3c, 0x2d, 0x8c,
    0x7f, 0x21, 0x65, 0xce, 0x66, 0x6b, 0x49, 0x5d, 0x27, 0x66, 0x02, 0xc7,
    0x00, 0x62, 0x05, 0x5d, 0x69, 0x74, 0xec, 0x55, 0x2d, 0xe3, 0xbd, 0xa0,
    0x2d, 0x91, 0xdd, 0x32, 0xc1, 0xd1, 0x78, 0xba, 0xf5, 0xb0, 0xe9, 0x89,
    0x81, 0xd6, 0x4c, 0x6d, 0x2a, 0x26, 0x28, 0x3a, 0xeb, 0x90, 0x34, 0xa3,
    0x96, 0xa4, 0xd1, 0x9c, 0x29, 0x9b, 0x1e, 0xaa, 0xfa, 0x3a, 0xcb, 0xd9,
    0x96, 0xa7, 0x99, 0xfe, 0x13, 0x86, 0x40, 0x51, 0x57, 0x48, 0x38, 0xf5,
    0xa2, 0x55, 0x72, 0x9e, 0x82, 0x9e, 0x8d, 0xad, 0x16, 0x75, 0xea, 0x8f,
    0xf7, 0x41, 0x62, 0x8a, 0x0d, 0x09, 0x0a, 0xe6, 0xa9, 0x20, 0xd7, 0x4b,
    0xe0, 0xb0, 0x41, 0x14, 0xa9, 0xf5, 0x11, 0xab, 0xf0, 0x92, 0x22, 0x56,
    0x98, 0xe5, 0x34, 0xbc, 0x74, 0x50, 0x7b, 0xaf, 0x95, 0x78, 0x35, 0xb1,
    0xef, 0x54, 0xb5, 0xf8, 0x64, 0xc3, 0xbf, 0xeb, 0x15, 0x53, 0x4b, 0x93,
    0x67, 0x68, 0xb0, 0xd1, 0x1c, 0xd2, 0x60, 0x91, 0xb1, 0x54, 0xd9, 0x85,
    0x4d, 0xa6, 0x4a, 0x3a, 0x46, 0x9e, 0x6c, 0xae, 0x26, 0x8e, 0xd1, 0xb2,
    0x11, 0x49, 0xf3, 0x15, 0x48, 0xec, 0x9d, 0x81, 0x13, 0x92, 0x8c, 0x71,
    0xd0, 0xef, 0xbe, 0x77, 0x55, 0x45, 0x23, 0xf4, 0xa5, 0x28, 0x0b, 0x46,
    0x0b, 0xe9, 0x90, 0x71, 0x19, 0x32, 0x4e, 0x63, 0x67, 0xe2, 0xbc, 0x2d,
    0x3f, 0x88, 0xb8, 0x66, 0x32, 0x5a, 0x3a, 0x03, 0xe7, 0x6d, 0x0a, 0x6a,
    0x1c, 0xd0, 0x27, 0x29, 0x79, 0xa9, 0xd3, 0x94, 0x00, 0xd7, 0xd6, 0x5a,
    0x86, 0x0b, 0x07, 0x87, 0x19, 0xd3, 0x59, 0xba, 0xe6, 0x11, 0xea, 0x58,
    0x81, 0x0c, 0x90, 0x95, 0x20, 0x26, 0xad, 0x64, 0x2c, 0xcc, 0x56, 0x9a,
    0xad, 0xb5, 0xb0, 0xa4, 0xda, 0x09, 0xb8, 0x1a, 0x97, 0x48, 0x72, 0x6b,
    0x81, 0x50, 0xb0, 0x48, 0x58, 0x45, 0xca, 0x7b, 0x1a, 0xdc, 0x1f, 0x2c,
    0x2c, 0xa0, 0x36, 0x5c, 0x5d, 0x09, 0x56, 0xe1, 0x8d, 0x82, 0xe2, 0x91,
    0x73, 0xd0, 0xab, 0x31, 0x7f, 0xbb, 0xf6, 0x63, 0x0b, 0x06, 0xb2, 0xba,
    0xc2, 0xf5, 0x3f, 0x6d, 0x2e, 0x3e, 0x5a, 0x2f, 0xe2, 0xfa, 0xdb, 0x1c,
    0x91, 0x1a, 0x07, 0xec, 0x45, 0x8b, 0x21, 0xea, 0xf4, 0x9c, 0xb1, 0xbd,
    0xa3, 0xea, 0x38, 0x82, 0x91, 0x14, 0x2e, 0xba, 0x66, 0x37, 0x9e, 0x6c,
    0x4e, 0x77, 0x5f, 0xb2, 0x19, 0x5e, 0x2e, 0x0d, 0x90, 0x8c, 0x58, 0xfb,
    0x72, 0xf9, 0x5e, 0xdd, 0x77, 0xbe, 0xe2, 0x7e, 0x69, 0x4d, 0xd4, 0xa8,
    0xd9, 0x00, 0x8f, 0xb6, 0x27, 0x85, 0xda, 0xfe, 0xe6, 0xa4, 0x56, 0x06,
    0x7c, 0x9c, 0x71, 0x13, 0x92, 0x76, 0x71, 0x6a, 0x16, 0x48, 0xee, 0x06,
    0xd5, 0xfa, 0xbc, 0x5a, 0x80, 0xfb, 0x45, 0xee, 0xcc, 0x9a, 0xd5, 0x2b,
    0x0d, 0x96, 0xc4, 0x5d, 0x09, 0xaf, 0x05, 0x32, 0x06, 0x68, 0xb7, 0xb3,
    0x1d, 0x86, 0x7c, 0xf4, 0xeb, 0xd4, 0xa3, 0x0d, 0xff, 0xf7, 0x42, 0x90,
    0x81, 0xe9, 0x2c, 0x4d, 0xc2, 0x1c, 0x99, 0xb3, 0x36, 0xe4, 0x46, 0xe5,
    0x95, 0x05, 0x0c, 0x8d, 0x67, 0xe9, 0x8d, 0x5a, 0x1c, 0x9e, 0xe6, 0x70,
    0x18, 0x6f, 0x5f, 0x26, 0xa9, 0x00, 0xeb, 0x12, 0x6d, 0x86, 0x8c, 0xcb,
    0x42, 0xf6, 0xc7, 0xe4, 0x9d, 0xce, 0x27, 0x11, 0x16, 0xd8, 0x36, 0x84,
    0x36, 0xae, 0xd4, 0x6a, 0x8c, 0xfa, 0xb9, 0x22, 0xc4, 0x28, 0x5a, 0xd4,
    0x38, 0x34, 0x3a, 0x28, 0x4f, 0x3e, 0x15, 0x52, 0x53, 0x10, 0x85, 0xc1,
    0xab, 0x04, 0x8d, 0xc6, 0x15, 0x33, 0x23, 0x53, 0xbc, 0x37, 0x95, 0xea,
    0x97, 0x87, 0xf7, 0xa0, 0x14, 0x3b, 0x44, 0x49, 0xd1, 0x6a, 0x5d, 0xd9,
    0x49, 0xa6, 0xb3, 0x86, 0x96, 0xc7, 0x9e, 0x70, 0xed, 0x9a, 0x5d, 0xe3,
    0x01, 0xb7, 0xcb, 0x81, 0x50, 0x52, 0xf0, 0x47, 0x97, 0xfb, 0x4e, 0xca,
    0x95, 0xf8, 0xc7, 0xf1, 0x77, 0x80, 0xe5, 0x1b, 0x10, 0xbb, 0x01, 0x05,
    0xf8, 0xef, 0xff, 0x2a, 0x89, 0x00, 0xae, 0xbc, 0x70, 0x40, 0x31, 0x4f,
    0x85, 0xd8, 0x92, 0xe3, 0x83, 0xf6, 0x42, 0x35, 0xec, 0x15, 0xa6, 0xa7,
    0xf9, 0xfc, 0x61, 0x5d, 0xcd, 0xe7, 0x76, 0x8b, 0x40, 0x29, 0xa1, 0x2b,
    0x63, 0xfd, 0xde, 0xca, 0x82, 0xbe, 0x4c, 0xd3, 0x5e, 0x51, 0x9b, 0xba,
    0xcd, 0x0b, 0x97, 0x8c, 0x8d, 0x85, 0x21, 0x5a, 0xf4, 0xfe, 0x7e, 0x0d,
    0x8d, 0xda, 0x8e, 0x9d, 0x7a, 0x1a, 0x3e, 0x1a, 0x6f, 0x78, 0x00, 0x6e,
    0x0d, 0x94, 0xcb, 0x57, 0x74, 0x1e, 0xae, 0x13, 0xe9, 0x56, 0xd6, 0xd5,
    0xa5, 0xda, 0xbe, 0xae, 0xc6, 0x41, 0x2c, 0xeb, 0x0d, 0x0d, 0x3b, 0x75,
    0xce, 0x2e, 0x46, 0x60, 0x79, 0x6d, 0x7d, 0xfb, 0x9a, 0x5f, 0x1a, 0x3c,
    0xea, 0xf9, 0x80, 0x06, 0x07, 0x40, 0x2a, 0xfd, 0xea, 0x38, 0xa2, 0x39,
    0xa9, 0x59, 0xee, 0xc1, 0x47, 0xd7, 0xf3, 0xcb, 0xf5, 0x80, 0xaf, 0x02,
    0x64, 0xca, 0xfe, 0xc2, 0x05, 0xa1, 0xeb, 0x43, 0xe7, 0x5b, 0x41, 0x7a,
    0x63, 0xb6, 0xa5, 0x3e, 0xba, 0x35, 0x1b, 0xa8, 0xa2, 0xe7, 0x02, 0x4b,
    0x62, 0x59, 0xeb, 0x8f, 0xc0, 0x33, 0x4a, 0x04, 0x0a, 0x6c, 0x47, 0xa3,
    0x51, 0x77, 0x6f, 0xcf, 0x55, 0x76, 0x05, 0xaa, 0xcc, 0xcb, 0x30, 0x93,
    0xeb, 0x9c, 0xba, 0x22, 0xd0, 0x95, 0x4e, 0x63, 0xcf, 0x7f, 0xed, 0x72,
    0x1f, 0x85, 0xf7, 0x5e, 0x51, 0xae, 0x20, 0x28, 0xc2, 0x74, 0x7a, 0xaf,
    0xd6, 0xcb, 0x3a, 0x1b, 0x31, 0xdf, 0xfa, 0x8e, 0xe0, 0x7e, 0x93, 0xe8,
    0xb4, 0x24, 0x15, 0xb2, 0x4c, 0xc7, 0x9e, 0x74, 0xc6, 0x25, 0xbd, 0x35,
    0xc3, 0x7b, 0x24, 0x34, 0xa3, 0xbe, 0xb7, 0x77, 0xee, 0x10, 0xc7, 0x77,
    0x5e, 0x73, 0xf4, 0xe3, 0x08, 0xb4, 0xf7, 0x91, 0x70, 0x45, 0x70, 0x49,
    0x6f, 0xbd, 0xbd, 0xbd, 0x72, 0x5c, 0xa6, 0x89, 0xb2, 0x6f, 0x70, 0x17,
    0x18, 0xb1, 0xc2, 0xf3, 0x2d, 0x6b, 0x92, 0x8d, 0x62, 0x13, 0x29, 0xb0,
    0x89, 0x9b, 0x10, 0xad, 0x46, 0xa8, 0x22, 0x2b, 0x53, 0x0f, 0xe0, 0x06,
    0x6f, 0x87, 0x38, 0xaf, 0x8d, 0x16, 0xfe, 0xe3, 0x46, 0xf9, 0xe7, 0xd6,
    0xb9, 0xb4, 0x0e, 0xce, 0x45, 0xd9, 0x32, 0xb7, 0xdb, 0x43, 0x74, 0x0a,
    0xd2, 0xd7, 0x66, 0xa3, 0x2f, 0x91, 0x2f, 0xf7, 0xeb, 0x05, 0x95, 0x2c,
    0xb4, 0x59, 0x34, 0x01, 0xd3, 0x99, 0x7a, 0x41, 0x75, 0xa5, 0xdf, 0xde,
    0x66, 0xe5, 0x33, 0x50, 0xab, 0x82, 0xf2, 0x21, 0x30, 0x86, 0x09, 0x52,
    0xae, 0x70, 0x97, 0x8b, 0xbe, 0x26, 0xad, 0x32, 0x4a, 0x86, 0x34, 0xad,
    0x73, 0xfc, 0x4c, 0x39, 0xa4, 0xd4, 0xa5, 0x46, 0xbe, 0xea, 0x10, 0x75,
    0x06, 0xb6, 0x29, 0x47, 0x79, 0x29, 0x79, 0x89, 0xca, 0xb4, 0x12, 0x30,
    0x6d, 0xe9, 0xba, 0xcf, 0x46, 0xb9, 0x3b, 0xb5, 0x96, 0xc7, 0x17, 0x65,
    0x82, 0x99, 0x9d, 0x1f, 0x8d, 0xb4, 0x6a, 0x62, 0xc0, 0x1e, 0x8d, 0x46,
    0xda, 0x54, 0x1c, 0x65, 0xae, 0x03, 0x66, 0x54, 0x05, 0xca, 0x47, 0xe9,
    0xee, 0x8e, 0xb5, 0x74, 0x07, 0x7e, 0x29, 0x23, 0x55, 0xa5, 0x2b, 0x87,
    0x27, 0x53, 0xba, 0xee, 0xe6, 0xa4, 0x0d, 0xc7, 0xeb, 0x5e, 0x55, 0x0d,
    0x37, 0x2a, 0xdb, 0x69, 0xaa, 0x5e, 0xdd, 0x02, 0x48, 0xe6, 0xf9, 0xb8,
    0x92, 0xaa, 0x5b, 0xcb, 0xd5, 0x49, 0x5b, 0xa2, 0xd7, 0x7c, 0x94, 0x4a,
    0x67, 0xa4, 0x86, 0xdf, 0x51, 0xcb, 0xcb, 0xa8, 0xde, 0x41, 0xb8, 0xbe,
    0xf9, 0x2d, 0xda, 0xd9, 0xdb, 0x13, 0x46, 0xca, 0x55, 0xd9, 0xc2, 0xab,
    0x71, 0xd7, 0xdb, 0x11, 0xe5, 0x32, 0x96, 0x7d, 0xea, 0x55, 0x34, 0x1d,
    0x2b, 0x31, 0xc4, 0x43, 0x06, 0x55, 0x9f, 0x09, 0x48, 0x07, 0xee, 0x6f,
    0xd9, 0xdc, 0xef, 0xef, 0x2f, 0x05, 0x57, 0xe8, 0xaf, 0xee, 0xbd, 0x18,
    0xee, 0x76, 0x36, 0x9d, 0x9e, 0x37, 0xd4, 0x3c, 0xbc, 0x70, 0x2e, 0xc8,
    0x79, 0x25, 0x06, 0xb8, 0x98, 0x56, 0x08, 0x18, 0xcd, 0xb2, 0x8d, 0xae,
    0x33, 0x3a, 0xcf, 0x4a, 0xe4, 0xab, 0x4a, 0x5a, 0x87, 0x22, 0xa1, 0xea,
    0x54, 0xd8, 0x58, 0xa5, 0xcc, 0xfc, 0x87, 0xd2, 0x49, 0x79, 0xa5, 0x95,
    0x5b, 0xed, 0xcc, 0xe8, 0x23, 0xc2, 0xaa, 0xd3, 0xac, 0xcd, 0xf5, 0x83,
    0x20, 0xe0, 0x3e, 0xfa, 0x53, 0x98, 0x02, 0xfa, 0x50, 0x95, 0x9a, 0x3d,
    0x93, 0x6e, 0x50, 0x8c, 0xe5, 0x33, 0x61, 0xb2, 0x30, 0x65, 0xea, 0x99,
    0x73, 0x60, 0x92, 0xd5, 0xe7, 0xd4, 0x33, 0xd0, 0x6b, 0xd2, 0xcb, 0x53,
    0x5a, 0xf9, 0x53, 0x68, 0xca, 0x63, 0x4a, 0x84, 0xeb, 0x1b, 0xd0, 0xa5,
    0xb6, 0x7c, 0x2c, 0x1a, 0xc5, 0xca, 0x7c, 0x28, 0x6c, 0xfb, 0x5e, 0x34,
    0xca, 0x41, 0x16, 0x14, 0xb1, 0x1d, 0x32, 0x1a, 0x45, 0x20, 0x0b, 0x8a,
    0xb4, 0x7c, 0x34, 0xb6, 0x74, 0x09, 0x39, 0x50, 0x56, 0xbb, 0x6e, 0x34,
    0x4a, 0x40, 0x2a, 0x2a, 0x82, 0x4b, 0x6f, 0x0e, 0x01, 0x66, 0x8d, 0x92,
    0xba, 0xc2, 0x56, 0x6f, 0x46, 0xd5, 0x66, 0x5a, 0x36, 0x03, 0x9a, 0x3f,
    0x90, 0x17, 0x3e, 0x43, 0x8e, 0x81, 0x4e, 0x78, 0xc9, 0x74, 0x0d, 0x78,
    0xc5, 0x13, 0x15, 0x7e, 0x34, 0x72, 0x7f, 0xaf, 0x74, 0xa2, 0xde, 0x64,
    0x72, 0x96, 0xa7, 0x2b, 0x26, 0x68, 0x90, 0x53, 0x91, 0x26, 0x57, 0x20,
    0x96, 0x0f, 0x94, 0x66, 0x15, 0xa9, 0x50, 0xe1, 0x05, 0x72, 0x49, 0xb9,
    0x8b, 0x1a, 0x68, 0x4c, 0xa9, 0x6b, 0x7e, 0x15, 0x9c, 0xee, 0xc3, 0x80,
    0x1f, 0xa4, 0xf5, 0x65, 0x88, 0xb8, 0x87, 0xbf, 0xd7, 0x46, 0x0f, 0x91,
    0xe7, 0x47, 0xba, 0xbb, 0x6c, 0x34, 0x5e, 0xbb, 0x99, 0xa2, 0xf0, 0xe5,
    0x4c, 0x5f, 0x1b, 0xc8, 0xa4, 0x96, 0xa5, 0x20, 0xb2, 0x7b, 0x8c, 0x0b,
    0x17, 0xb1, 0x1c, 0x08, 0x29, 0xbd, 0x47, 0xa3, 0x11, 0x47, 0x2e, 0xa2,
    0x6d, 0x6f, 0x69, 0xca, 0x00, 0xe1, 0xa3, 0x0d, 0x63, 0x09, 0xf4, 0xeb,
    0x64, 0x7c, 0xf1, 0xcb, 0xcf, 0xff, 0x49, 0x34, 0xca, 0x72, 0x06, 0x8e,
    0xcd, 0xc4, 0x3b, 0x3e, 0x2e, 0x36, 0xb7, 0x07, 0xf5, 0xc4, 0xf5, 0x36,
    0xa5, 0xb4, 0xab, 0x24, 0xed, 0xca, 0x8d, 0x12, 0x4d, 0x14, 0x4b, 0x89,
    0xd6, 0xdd, 0xdd, 0xa3, 0xfb, 0x0c, 0x46, 0xa9, 0xba, 0xae, 0x4f, 0xbd,
    0xba, 0x09, 0x66, 0xc9, 0x4c, 0xe5, 0xa6, 0x14, 0x70, 0x79, 0x53, 0xcf,
    0x62, 0x0a, 0xa9, 0x62, 0x0a, 0xa5, 0xb2, 0x87, 0xcd, 0xeb, 0xad, 0x69,
    0x17, 0x2d, 0xeb, 0x8e, 0x4f, 0xe1, 0x8e, 0xaf, 0x79, 0xc5, 0x93, 0xee,
    0x04, 0xf8, 0xc5, 0x01, 0xf7, 0x81, 0xed, 0xa8, 0xea, 0x2a, 0x26, 0xc4,
    0x66, 0x23, 0x9b, 0xa3, 0x30, 0x59, 0x53, 0xcf, 0xc3, 0x1b, 0x9f, 0x5f,
    0xbf, 0x99, 0x0d, 0x7e, 0x2a, 0x87, 0x11, 0xc1, 0xc1, 0x35, 0xa0, 0xf7,
    0x0f, 0x97, 0xda, 0xd7, 0x0a, 0x59, 0xdd, 0x04, 0x2c, 0xbe, 0x54, 0x15,
    0xaa, 0x6e, 0x04, 0xd2, 0xe6, 0xe1, 0x0b, 0x6b, 0xf1, 0x63, 0xdb, 0xb5,
    0x05, 0xbc, 0xf5, 0x3e, 0x52, 0xd9, 0xb6, 0xd8, 0x30, 0x9b, 0xa3, 0x44,
    0x8d, 0x68, 0x11, 0x61, 0x84, 0x8c, 0x68, 0x45, 0x6a, 0x0b, 0x99, 0x5a,
    0xce, 0x87, 0x53, 0xbc, 0x75, 0x82, 0x6b, 0xc5, 0x40, 0xab, 0x20, 0xef,
    0x93, 0x3f, 0x55, 0x42, 0x90, 0x29, 0x9a, 0x5b, 0x48, 0x6d, 0x1b, 0x82,
    0xc2, 0xa8, 0x6d, 0x6d, 0x5b, 0x92, 0x2a, 0x26, 0x48, 0x28, 0x04, 0x5b,
    0xc0, 0xad, 0x7e, 0x05, 0x02, 0x70, 0xb9, 0x0c, 0x39, 0x49, 0x71, 0x8d,
    0x55, 0x5b, 0x60, 0x15, 0xab, 0x8b, 0xfb, 0xf6, 0x95, 0xfb, 0xa4, 0xd7,
    0x05, 0x53, 0xca, 0x2a, 0x61, 0xdc, 0xa7, 0x07, 0xf7, 0xce, 0xc4, 0x14,
    0x2d, 0x4d, 0x3b, 0x7a, 0xdd, 0x5f, 0x7e, 0xfe, 0x77, 0xb8, 0xd5, 0x83,
    0x41, 0xa4, 0x67, 0xad, 0x21, 0x27, 0xe9, 0xfc, 0xbc, 0xba, 0xb0, 0xd9,
    0xf7, 0xa9, 0x0b, 0x5c, 0x3d, 0x8e, 0xaa, 0xfd, 0x92, 0x39, 0xda, 0xdb,
    0xb3, 0xcc, 0xe4, 0x03, 0x91, 0xae, 0xa8, 0x65, 0xb1, 0xc4, 0x2b, 0x5d,
    0xfb, 0xbd, 0xc3, 0x8b, 0x94, 0x96, 0xbb, 0xe1, 0xe0, 0x94, 0xe6, 0x84,
    0x86, 0xd1, 0x52, 0x7b, 0x9f, 0xab, 0x45, 0xd1, 0xdd, 0x37, 0x59, 0x31,
    0xbe, 0xdd, 0x96, 0xc3, 0xdb, 0xdb, 0x7b, 0xc4, 0x2d, 0x7d, 0xfb, 0x83,
    0xc6, 0x20, 0x9a, 0x3a, 0xfd, 0xa9, 0x57, 0x14, 0xda, 0x83, 0x74, 0xa6,
    0xfc, 0xa1, 0x8d, 0x3c, 0x36, 0xbc, 0xa2, 0x1d, 0xe5, 0x11, 0xd1, 0x34,
    0x9d, 0xab, 0xe2, 0x2a, 0x2c, 0x5c, 0x45, 0x78, 0xdf, 0x7d, 0xf7, 0x9d,
    0x7f, 0xcd, 0x78, 0x9c, 0x5e, 0x07, 0x51, 0x42, 0xc3, 0xdc, 0xb8, 0x63,
    0x5e, 0x7b, 0xfe, 0xcc, 0x75, 0x3e, 0x73, 0x34, 0x10, 0x24, 0x8a, 0xb7,
    0x16, 0x8e, 0xe7, 0x5f, 0x8f, 0x74, 0x69, 0xcb, 0x75, 0xf3, 0x83, 0x7f,
    0x0c, 0xce, 0x99, 0x0d, 0x8b, 0xa0, 0x0f, 0xca, 0x38, 0xfa, 0x5a, 0xdb,
    0x75, 0xfb, 0x6f, 0xbd, 0xcd, 0xf6, 0xca, 0xfd, 0x6e, 0xd7, 0xd0, 0xef,
    0x02, 0xec, 0x78, 0x60, 0x58, 0x70, 0xa4, 0xb4, 0xd9, 0xd1, 0x0c, 0xdc,
    0x83, 0x57, 0x19, 0xd8, 0xe1, 0x94, 0xbe, 0x3b, 0x04, 0x99, 0x09, 0x51,
    0x3a, 0xe0, 0x16, 0x6f, 0xab, 0x48, 0x08, 0x72, 0xf4, 0x6e, 0x38, 0x73,
    0x9d, 0x8f, 0xe1, 0x15, 0xe3, 0x8b, 0x20, 0x08, 0x1c, 0x6f, 0xd8, 0xb6,
    0x0e, 0x52, 0x5a, 0xc5, 0x87, 0x19, 0x05, 0xa9, 0x83, 0x3b, 0x30, 0x27,
    0x18, 0xb9, 0x7c, 0x50, 0x60, 0xbe, 0xdb, 0xdb, 0x53, 0xfd, 0xa0, 0x85,
    0x6e, 0x39, 0xd8, 0x0f, 0x54, 0xe6, 0xb7, 0x60, 0xb2, 0x04, 0x4b, 0xb7,
    0x73, 0xc5, 0x0e, 0xe8, 0x81, 0x57, 0xda, 0x78, 0xbf, 0x45, 0x1b, 0xef,
    0x72, 0xe5, 0x5e, 0xb9, 0xde, 0xe6, 0x74, 0x94, 0x06, 0xb3, 0x34, 0xcc,
    0xe3, 0x97, 0x61, 0x16, 0xce, 0x58, 0xc2, 0x24, 0x03, 0x7f, 0x8f, 0x30,
    0x49, 0xd2, 0x6b, 0x1a, 0x23, 0x12, 0x3d, 0x63, 0x5c, 0x4c, 0x26, 0xa7,
    0xfe, 0xd5, 0xf6, 0xa2, 0x38, 0xda, 0x1f, 0xc3, 0x3c, 0x06, 0x87, 0x1d,
    0x80, 0x41, 0x26, 0x52, 0x30, 0x3a, 0x8e, 0x95, 0xc9, 0x9d, 0xf2, 0x98,
    0x49, 0x68, 0xdc, 0xd1, 0x58, 0x68, 0x9b, 0x0a, 0xc2, 0xc8, 0xb5, 0xfc,
    0x54, 0x7b, 0x18, 0xbf, 0xa1, 0xf1, 0xa4, 0x44, 0x5b, 0x3b, 0xdb, 0xd1,
    0xd5, 0x46, 0xdb, 0x6a, 0x81, 0x3d, 0xa4, 0x55, 0x49, 0x71, 0xa9, 0x1c,
    0xfd, 0xca, 0x6a, 0xc5, 0xab, 0x0c, 0x5b, 0xb1, 0xe9, 0xf9, 0xdf, 0xbb,
    0x68, 0x52, 0xa9, 0x9c, 0x66, 0xa0, 0x8e, 0xf6, 0x97, 0xd1, 0x6e, 0x34,
    0x98, 0x69, 0xfc, 0xd0, 0xec, 0x6c, 0x93, 0x86, 0x05, 0xae, 0xd9, 0x9c,
    0x75, 0x84, 0x60, 0xb1, 0x5d, 0x02, 0x12, 0x3f, 0x0a, 0x16, 0x63, 0x09,
    0xe3, 0x4d, 0xd5, 0xb1, 0x7c, 0xf4, 0xac, 0xb2, 0x26, 0xfb, 0x65, 0x99,
    0x7b, 0x77, 0x87, 0xad, 0x3a, 0x9e, 0xff, 0x17, 0xd7, 0xf3, 0x7f, 0xd0,
    0x4e, 0x1e, 0x60, 0xea, 0xd7, 0x38, 0x17, 0x6f, 0x80, 0x54, 0x8f, 0xb6,
    0x82, 0xa3, 0xe7, 0xff, 0xd9, 0xf5, 0xfc, 0x57, 0xae, 0xb1, 0x36, 0xfb,
    0x64, 0x73, 0xc4, 0x7f, 0x46, 0x12, 0x6f, 0x79, 0xa8, 0x54, 0x1f, 0x95,
    0xbd, 0xdd, 0x57, 0x7b, 0xa8, 0xac, 0xc2, 0x68, 0x50, 0xd6, 0x06, 0x3b,
    0x58, 0x1f, 0x1a, 0xb0, 0x92, 0xe0, 0xd3, 0x87, 0xa8, 0x35, 0x56, 0x1a,
    0x7c, 0xfa, 0x61, 0xc6, 0xb4, 0x1b, 0x98, 0x95, 0x53, 0x25, 0xfa, 0xc6,
    0x12, 0xd6, 0xca, 0x35, 0x49, 0xe8, 0x0a, 0xe3, 0xa7, 0x2d, 0x07, 0x67,
    0xcb, 0x6e, 0xb0, 0x72, 0x6e, 0x86, 0x65, 0xb7, 0xdc, 0x14, 0x46, 0xe0,
    0x17, 0x5b, 0x2e, 0xca, 0xf7, 0x16, 0xe5, 0xfd, 0x69, 0x3b, 0x34, 0x0e,
    0xc3, 0x5d, 0x00, 0xe7, 0x05, 0x46, 0xc5, 0x36, 0x7a, 0x24, 0x1b, 0xc0,
    0xac, 0xcd, 0x27, 0x1a, 0xf8, 0x13, 0x6c, 0xfa, 0xf3, 0x9d, 0xcd, 0x15,
    0xdf, 0x4c, 0x07, 0xce, 0xfb, 0xf9, 0xdc, 0xa9, 0xc6, 0xf7, 0x83, 0x35,
    0xbe, 0xbf, 0xbb, 0xb9, 0x05, 0x9a, 0x1e, 0x0e, 0xcc, 0x7c, 0x76, 0xf0,
    0x0e, 0x4d, 0xaf, 0xdb, 0x1d, 0x96, 0xee, 0x95, 0xd2, 0xf2, 0xaa, 0x44,
    0xd1, 0x0f, 0x09, 0x09, 0x26, 0x60, 0x40, 0x24, 0xab, 0xcf, 0xbf, 0xd4,
    0x34, 0xa0, 0x1a, 0x62, 0xcf, 0xf3, 0xf2, 0xcc, 0x78, 0x17, 0xc3, 0x95,
    0xeb, 0xb0, 0x4c, 0xa9, 0x0e, 0xc0, 0xfa, 0x39, 0x8d, 0xd5, 0xb1, 0x60,
    0x59, 0xa7, 0x74, 0xf2, 0x44, 0x3a, 0x0f, 0x3f, 0x4d, 0x0e, 0xa7, 0x72,
    0x15, 0x8a, 0x4b, 0x34, 0x8b, 0x56, 0x3f, 0x4d, 0xce, 0x22, 0x94, 0xf4,
    0x3a, 0xbc, 0x85, 0x1c, 0xfd, 0xd3, 0xe4, 0xc4, 0x1c, 0x5b, 0x8a, 0xb9,
    0xf0, 0x0a, 0x88, 0x38, 0x80, 0x31, 0x9b, 0x14, 0x96, 0x71, 0xbc, 0x9a,
    0xf1, 0x78, 0x39, 0xe2, 0x92, 0x39, 0x35, 0x3f, 0x34, 0x87, 0x5a, 0x09,
    0xf1, 0xa0, 0x11, 0xc0, 0x67, 0x09, 0xac, 0xbc, 0x33, 0xc1, 0xdb, 0xad,
    0x33, 0xd0, 0x9f, 0xc3, 0x07, 0xd6, 0x97, 0x3e, 0xae, 0xdd, 0xae, 0x68,
    0x0d, 0x9f, 0xa0, 0x90, 0xe3, 0x4b, 0xaf, 0x80, 0x5d, 0x8a, 0xb5, 0x17,
    0x81, 0x35, 0xe6, 0xea, 0xf2, 0x81, 0xc4, 0xc5, 0xd5, 0xee, 0x17, 0xa2,
    0x69, 0xce, 0x6e, 0xaa, 0xde, 0x76, 0x74, 0x0c, 0x85, 0xc6, 0x0e, 0x3b,
    0x1f, 0x69, 0x98, 0x47, 0x4b, 0xa0, 0x17, 0x60, 0x21, 0xa4, 0x37, 0x54,
    0xed, 0xd9, 0x2f, 0x3f, 0xff, 0xa7, 0x33, 0xdc, 0xee, 0x4f, 0x54, 0x8e,
    0xa8, 0x41, 0xc1, 0x0a, 0x6f, 0xf8, 0x80, 0x4e, 0x51, 0x6d, 0xad, 0x07,
    0x5c, 0xaa, 0xaf, 0xe7, 0xe0, 0x28, 0x0a, 0x19, 0x32, 0x5f, 0xf3, 0x08,
    0x08, 0xc5, 0xc4, 0x21, 0x2e, 0x5a, 0xf0, 0x95, 0x29, 0x9e, 0x76, 0x9a,
    0xad, 0xcd, 0xd7, 0x56, 0x40, 0x9b, 0x46, 0xb5, 0x6b, 0x50, 0xcd, 0x66,
    0xf4, 0x3e, 0xdf, 0x9e, 0x1d, 0xde, 0x39, 0x2c, 0xba, 0x34, 0x1e, 0x3a,
    0xe6, 0x2f, 0xcb, 0x6a, 0x7e, 0x3a, 0x95, 0xfe, 0x50, 0x69, 0x59, 0x4b,
    0xbf, 0x1d, 0x28, 0xd7, 0xf2, 0xda, 0x51, 0x49, 0xc6, 0x8b, 0x55, 0xa7,
    0x9b, 0xcf, 0x9d, 0x5e, 0x36, 0xb6, 0x57, 0x8c, 0x1a, 0xa8, 0xd1, 0x5b,
    0x62, 0x93, 0x4a, 0xf5, 0x5a, 0xd9, 0x29, 0x9d, 0x6a, 0xf7, 0x04, 0xe5,
    0x92, 0x00, 0x4a, 0xc4, 0xd2, 0xe7, 0xa5, 0xe1, 0x1e, 0xfa, 0xeb, 0xdd,
    0x5c, 0x70, 0xb3, 0x2a, 0x37, 0x97, 0x97, 0x40, 0x70, 0x89, 0x71, 0x62,
    0x46, 0xdd, 0xb7, 0x5c, 0x86, 0xca, 0xd7, 0xb4, 0xaa, 0x06, 0xbc, 0x3d,
    0xa8, 0x79, 0x96, 0x4c, 0x18, 0x10, 0xf3, 0x1b, 0x6e, 0x30, 0xab, 0x90,
    0xaf, 0x81, 0x25, 0xd9, 0xe2, 0x07, 0x53, 0x6d, 0xf8, 0x2e, 0x89, 0xbc,
    0x2a, 0x71, 0xaf, 0x8f, 0x48, 0x29, 0x02, 0xad, 0x40, 0x05, 0x0d, 0xc4,
    0xa3, 0xd1, 0x38, 0x82, 0xd5, 0xac, 0xf9, 0xa9, 0xa8, 0x12, 0x9e, 0xcf,
    0x5a, 0xf6, 0xe4, 0x58, 0x5c, 0xf3, 0xf2, 0xf6, 0xc2, 0x83, 0xf0, 0x94,
    0x5b, 0x60, 0x53, 0xc5, 0x35, 0x60, 0xd9, 0xf0, 0xb3, 0x0b, 0xf7, 0x53,
    0x86, 0x24, 0x8f, 0x57, 0xa4, 0x0e, 0xf2, 0x14, 0x85, 0x7c, 0xc4, 0xee,
    0xee, 0x28, 0x98, 0xe2, 0x22, 0x38, 0x4d, 0xc4, 0x40, 0xfd, 0x02, 0x86,
    0x6f, 0x8b, 0x87, 0x26, 0x7a, 0x3d, 0xe0, 0x72, 0x19, 0xff, 0x99, 0x06,
    0x4a, 0xfb, 0xec, 0xe2, 0x61, 0x54, 0x82, 0xfe, 0x1d, 0x65, 0x76, 0x3b,
    0xc3, 0x74, 0xb1, 0x2e, 0xdc, 0x62, 0xb5, 0xb5, 0x6e, 0xab, 0xe2, 0x76,
    0xf7, 0x82, 0xee, 0xb0, 0xdd, 0x66, 0xca, 0xc5, 0x7a, 0xb6, 0x62, 0x52,
    0x61, 0x2d, 0x02, 0x01, 0x82, 0x64, 0x5b, 0x09, 0xb4, 0xcd, 0x73, 0xa4,
    0x64, 0x4f, 0x80, 0xcb, 0xf9, 0x4a, 0x1b, 0xfa, 0xbc, 0xe9, 0x7e, 0xe3,
    0x19, 0xd1, 0x40, 0xe5, 0x63, 0xe3, 0xe1, 0x6e, 0xe4, 0x95, 0x33, 0x8e,
    0xa7, 0x36, 0x25, 0xb7, 0xdc, 0x6d, 0xbc, 0x8a, 0x95, 0xc8, 0x2d, 0x67,
    0x1b, 0xcf, 0x37, 0x2e, 0x35, 0x26, 0xb9, 0x0a, 0xe4, 0x83, 0x02, 0x9d,
    0x7b, 0xd6, 0xd6, 0xb2, 0xe6, 0x57, 0xbf, 0xc0, 0x33, 0xab, 0xbc, 0x14,
    0x36, 0x2d, 0xfc, 0x4b, 0x47, 0x3c, 0x3c, 0x58, 0xc2, 0x66, 0xf8, 0x6b,
    0x1e, 0x1c, 0xc3, 0x86, 0x45, 0x4f, 0x0d, 0xf0, 0x2b, 0xc1, 0x98, 0x65,
    0x97, 0x04, 0xc0, 0x7c, 0x0a, 0xea, 0x17, 0x38, 0x29, 0x8f, 0x68, 0x65,
    0x75, 0x24, 0x4f, 0xba, 0xc8, 0xe1, 0x95, 0x02, 0xb4, 0xba, 0xc2, 0x4d,
    0x5a, 0x0a, 0xb7, 0xae, 0x5a, 0xd5, 0x52, 0xb0, 0x22, 0x2d, 0xc1, 0x0a,
    0x28, 0xe1, 0x1a, 0x12, 0x91, 0x47, 0x3d, 0x25, 0x64, 0xb1, 0x6c, 0x04,
    0x7e, 0x85, 0x9e, 0x4e, 0xd1, 0xc5, 0x9a, 0x9e, 0xce, 0x68, 0x53, 0x16,
    0x5b, 0x72, 0xb4, 0x6e, 0x66, 0x31, 0xdc, 0x75, 0x63, 0x28, 0x8b, 0x7c,
    0x7f, 0x1f, 0xbb, 0x66, 0xba, 0x50, 0x85, 0xaa, 0x6b, 0x6e, 0xa7, 0x6c,
    0x64, 0x0b, 0x6d, 0x36, 0x8b, 0xbe, 0x31, 0x2b, 0xb6, 0x83, 0x4f, 0xf4,
    0x5b, 0x37, 0x8e, 0x4a, 0xda, 0xb4, 0x6b, 0x44, 0xb0, 0x0c, 0xbf, 0xfa,
    0x5e, 0x59, 0x5e, 0x76, 0xb4, 0xb7, 0x89, 0xed, 0x61, 0x52, 0x8a, 0xeb,
    0x26, 0xce, 0x47, 0x2c, 0x46, 0xde, 0xbc, 0x7e, 0x45, 0xd6, 0x59, 0x1c,
    0x2a, 0xeb, 0x0b, 0x2b, 0x51, 0x99, 0x5e, 0xd0, 0x18, 0x94, 0xd2, 0xce,
    0x2e, 0x44, 0x55, 0xb1, 0x9d, 0xe5, 0x22, 0xfe, 0xd0, 0x4a, 0x47, 0xed,
    0x20, 0xae, 0x5b, 0xed, 0x0e, 0xd5, 0xe4, 0x5b, 0xf1, 0x76, 0x83, 0xad,
    0x1a, 0x86, 0xb2, 0xda, 0xbe, 0xbf, 0xe0, 0x19, 0xc8, 0xb2, 0xe4, 0xb6,
    0x63, 0xe2, 0x18, 0x6c, 0xdb, 0x17, 0xcb, 0x05, 0x08, 0x2f, 0xd0, 0x86,
    0x1c, 0x81, 0xfb, 0x3e, 0x08, 0x3f, 0x05, 0x9e, 0x34, 0x70, 0x73, 0x0b,
    0x73, 0xa9, 0x08, 0x56, 0x45, 0xc6, 0x26, 0x8e, 0xf2, 0xf7, 0x31, 0x1b,
    0x6b, 0x71, 0xb6, 0x3e, 0x6d, 0x73, 0xd9, 0x20, 0xfd, 0x6c, 0xcb, 0x58,
    0xb6, 0xb0, 0xd0, 0xda, 0xd3, 0x28, 0xa1, 0x52, 0xd2, 0x5c, 0xf8, 0x44,
    0xd9, 0x9c, 0x08, 0xa0, 0x90, 0xcb, 0xdb, 0x6c, 0x09, 0x7a, 0xfc, 0x4a,
    0xb6, 0xbd, 0xd1, 0x92, 0xca, 0x92, 0x8f, 0xf6, 0x7c, 0xcd, 0x2f, 0xeb,
    0x44, 0xc3, 0x48, 0x7b, 0xbe, 0xe6, 0x96, 0x75, 0xba, 0x61, 0xa3, 0x3d,
    0x5f, 0xf3, 0xca, 0x3a, 0xdd, 0x30, 0xd1, 0x9e, 0x1f, 0x73, 0xd3, 0x06,
    0xb0, 0xcf, 0x2d, 0x10, 0xd3, 0x4b, 0xb5, 0x8f, 0xab, 0xfc, 0x30, 0x48,
    0xd3, 0x55, 0x06, 0x1b, 0xb5, 0x4e, 0x03, 0xe9, 0x9b, 0x05, 0x1a, 0x50,
    0xdf, 0x5c, 0x7b, 0xa1, 0xcf, 0xea, 0x5e, 0xec, 0x61, 0xfa, 0x99, 0x85,
    0x61, 0x31, 0xaf, 0x42, 0xb1, 0x7e, 0xfb, 0x0a, 0x0c, 0xa5, 0xb6, 0xdd,
    0x9b, 0x3d, 0x3f, 0xcc, 0x6a, 0x2d, 0xd1, 0x99, 0xdd, 0xd0, 0xb9, 0xbc,
    0x18, 0xf0, 0x02, 0x8f, 0xc0, 0x5a, 0xcb, 0x53, 0x86, 0x5b, 0x38, 0x17,
    0x0d, 0x0d, 0x28, 0x5b, 0xdf, 0x01, 0xe5, 0xf4, 0x06, 0xc3, 0xca, 0x7e,
    0x09, 0x13, 0x6c, 0x3b, 0xb2, 0xfb, 0xa6, 0xae, 0x4f, 0x47, 0x9f, 0x3f,
    0xbc, 0xd1, 0xc1, 0x5d, 0xdf, 0xcf, 0xfe, 0x4e, 0x23, 0xf9, 0xf9, 0xc3,
    0x1b, 0x17, 0x20, 0xe7, 0xfb, 0x24, 0x9d, 0xb9, 0xe7, 0x8d, 0xc5, 0x95,
    0x3e, 0x62, 0xd9, 0xbe, 0x77, 0xe1, 0x6f, 0xc0, 0x80, 0x66, 0xd0, 0x8e,
    0x5e, 0x57, 0x78, 0x9e, 0xcf, 0x77, 0xc6, 0x8d, 0x0d, 0x31, 0x04, 0x1b,
    0x86, 0xd1, 0xa0, 0x3e, 0x0f, 0x40, 0xdd, 0x8f, 0x81, 0x37, 0xcd, 0x55,
    0xa4, 0x23, 0x55, 0x9c, 0xba, 0x0e, 0x84, 0x93, 0x5c, 0x67, 0x18, 0x24,
    0x4e, 0x45, 0x54, 0x63, 0xd1, 0xa5, 0xeb, 0xf9, 0x30, 0xda, 0x9c, 0x5e,
    0xa5, 0x97, 0xd6, 0x68, 0xe9, 0xae, 0x15, 0x62, 0xab, 0x6a, 0x85, 0xd4,
    0x89, 0xad, 0x58, 0x02, 0x43, 0xfb, 0x8d, 0x57, 0x5a, 0x80, 0xfe, 0x3a,
    0x18, 0x67, 0x4c, 0x1f, 0xa2, 0x86, 0x4a, 0x40, 0xad, 0xa3, 0x52, 0x60,
    0xb8, 0xbb, 0xa5, 0x6c, 0xfb, 0xba, 0xd3, 0xad, 0xa0, 0xca, 0x1b, 0x88,
    0xcf, 0xf9, 0x1e, 0x67, 0x49, 0x54, 0x1d, 0x8b, 0xd0, 0x32, 0x6f, 0xc3,
    0xec, 0xd0, 0x74, 0x7f, 0xd8, 0xdb, 0x63, 0x26, 0x48, 0x69, 0x80, 0x91,
    0x7d, 0xca, 0x60, 0x3f, 0x6e, 0x3a, 0xc2, 0x4d, 0xc2, 0xb8, 0x5a, 0x2e,
    0x57, 0x12, 0x95, 0xaa, 0x69, 0xa8, 0x41, 0xe3, 0x80, 0x68, 0xab, 0xbf,
    0xab, 0x10, 0x02, 0x6d, 0x82, 0xc8, 0x5c, 0x18, 0x9b, 0x6f, 0xa1, 0x64,
    0x87, 0x0e, 0x6a, 0x7e, 0x06, 0x6b, 0x97, 0x55, 0x0b, 0x98, 0xd3, 0x79,
    0x4e, 0xc5, 0xb2, 0x93, 0xa4, 0x8b, 0xaf, 0x84, 0x33, 0xac, 0x01, 0x4d,
    0xa8, 0xaa, 0x75, 0x97, 0x66, 0x73, 0x5f, 0x7a, 0xb2, 0x31, 0x91, 0x2e,
    0x03, 0x15, 0x2b, 0xc7, 0x0b, 0xb2, 0x30, 0xfe, 0x08, 0x80, 0xef, 0xf6,
    0xba, 0x5e, 0x41, 0xd0, 0xfc, 0x2c, 0xa1, 0x57, 0x34, 0xa9, 0x9b, 0x20,
    0x40, 0xb1, 0xd7, 0x3c, 0x76, 0x9f, 0x79, 0x85, 0x32, 0xdf, 0x53, 0x17,
    0xbf, 0xc2, 0xdc, 0x36, 0xa6, 0xff, 0x34, 0xf5, 0x30, 0xae, 0x00, 0xa1,
    0x5c, 0xe6, 0x8c, 0x8a, 0xc0, 0xd9, 0x0a, 0x22, 0x75, 0x59, 0x55, 0x06,
    0x9a, 0x8d, 0x07, 0xde, 0xa8, 0x6b, 0xf7, 0xe9, 0xc5, 0xd6, 0xfb, 0x34,
    0x8c, 0x00, 0x6f, 0xeb, 0x08, 0x2b, 0x0f, 0x17, 0x82, 0xf9, 0xa2, 0x14,
    0x07, 0xdb, 0xc3, 0xd5, 0xda, 0x6c, 0xd0, 0x5f, 0xba, 0x9f, 0x7c, 0x50,
    0x36, 0xf8, 0x76, 0x52, 0x48, 0xfd, 0xfe, 0x51, 0xd7, 0x2b, 0x60, 0x12,
    0xc3, 0x7f, 0x3a, 0xd9, 0x57, 0x11, 0x9e, 0xc1, 0x12, 0xf8, 0x36, 0xa1,
    0xe3, 0x41, 0x9e, 0xa6, 0x72, 0x33, 0x4f, 0xb9, 0xec, 0xcc, 0xc3, 0x15,
    0x4b, 0x6e, 0x07, 0x67, 0x29, 0xc4, 0x8c, 0x12, 0xfe, 0xdb, 0x94, 0x4b,
    0x41, 0xf3, 0x3c, 0x94, 0xfe, 0x9a, 0x75, 0x44, 0xc8, 0x45, 0x47, 0xd0,
    0x9c, 0xcd, 0x7d, 0x71, 0x2b, 0x24, 0x5d, 0x75, 0xd6, 0xcc, 0xef, 0xc0,
    0x19, 0xa7, 0x1d, 0x95, 0xe0, 0x7f, 0xa4, 0x8b, 0x94, 0x92, 0xcf, 0xa7,
    0x7e, 0x55, 0x76, 0xa8, 0xec, 0x1e, 0x1e, 0xcf, 0xfb, 0xf0, 0x6f, 0x08,
    0x47, 0x77, 0x81, 0x91, 0x98, 0x06, 0x8f, 0x7b, 0xc7, 0xf0, 0x4f, 0x15,
    0xe8, 0x88, 0x08, 0x16, 0x66, 0x00, 0x8b, 0x32, 0xec, 0x74, 0xd2, 0x1c,
    0x8e, 0xe4, 0x40, 0x87, 0x4d, 0x2c, 0x13, 0xfa, 0x83, 0xc7, 0xf3, 0xf9,
    0xd3, 0xf0, 0x20, 0x1e, 0x76, 0x3a, 0x62, 0x49, 0x93, 0xa4, 0x6c, 0xa4,
    0xd3, 0x91, 0x69, 0x36, 0xd0, 0x41, 0xaf, 0x87, 0x1d, 0x75, 0x19, 0x18,
    0x3c, 0xee, 0x1f, 0xc1, 0x3f, 0xf3, 0xdd, 0xb7, 0x0a, 0xa0, 0xdc, 0x7c,
    0xf0, 0xb8, 0x1f, 0xf5, 0xa3, 0x3e, 0x1d, 0x76, 0x3a, 0x10, 0xa8, 0x62,
    0xf0, 0xf8, 0xb0, 0x77, 0xd8, 0x3b, 0x3c, 0x18, 0x76, 0x3a, 0xe8, 0xce,
    0x3c, 0x78, 0x3c, 0x7b, 0x3e, 0x3b, 0x9e, 0xcd, 0x86, 0x9d, 0xce, 0x02,
    0x03, 0xda, 0x3d, 0x3e, 0x9a, 0x3f, 0x3f, 0xec, 0xcd, 0x87, 0x9d, 0x8e,
    0x32, 0x06, 0x1c, 0xe8, 0x08, 0x8f, 0x38, 0x9c, 0x30, 0x4e, 0xaf, 0x07,
    0x5d, 0xf2, 0x3c, 0xbb, 0x21, 0x07, 0xdd, 0xec, 0x86, 0x3c, 0xee, 0x76,
    0xbb, 0x07, 0x05, 0xae, 0xae, 0xb6, 0xd3, 0x52, 0xd0, 0x01, 0x4c, 0xd3,
    0xc5, 0x46, 0x2f, 0x4b, 0xff, 0x79, 0xff, 0xb8, 0x1f, 0xd6, 0x96, 0x85,
    0xce, 0xe9, 0x9c, 0xc6, 0xf5, 0x65, 0xc1, 0x4a, 0xd5, 0xa4, 0x75, 0x11,
    0x3d, 0xe9, 0xf9, 0x7c, 0x5e, 0xcd, 0xd8, 0xfa, 0x80, 0xd5, 0x7a, 0x3a,
    0x3f, 0x9a, 0x1f, 0x54, 0xd3, 0x9d, 0x87, 0xf3, 0x70, 0xfe, 0xbc, 0x9c,
    0x6e, 0xfc, 0x2c, 0x3e, 0x8a, 0xfb, 0xd5, 0x74, 0x9f, 0x1e, 0x3f, 0x7d,
    0xfa, 0x34, 0xac, 0xa6, 0x7b, 0x70, 0xf4, 0xec, 0xa8, 0xdf, 0xb7, 0x67,
    0xf7, 0x2c, 0xbb, 0x21, 0xfd, 0x43, 0x98, 0x9d, 0x1a, 0x79, 0xaf, 0x5f,
    0xfc, 0xaf, 0xcd, 0x2c, 0xbd, 0xe9, 0x08, 0xf6, 0x0f, 0xb0, 0xf5, 0x50,
    0x51, 0x30, 0x3b, 0xb3, 0xf4, 0xa6, 0x80, 0xa8, 0xe8, 0x1b, 0xe5, 0xcc,
    0xdf, 0x29, 0x7d, 0xf8, 0x95, 0x0b, 0x7f, 0x01, 0x18, 0x6f, 0xb3, 0x0a,
    0xf3, 0x05, 0xe3, 0x83, 0xae, 0x3d, 0xfb, 0xab, 0x30, 0x77, 0xf5, 0x34,
    0x41, 0xeb, 0xc5, 0x25, 0x34, 0x4c, 0x07, 0xbd, 0xc3, 0xec, 0xa6, 0x50,
    0xc2, 0x03, 0x1f, 0x99, 0x36, 0x5f, 0xe9, 0xfe, 0x10, 0x6c, 0x07, 0x8c,
    0x2f, 0x69, 0xce, 0xa4, 0x29, 0x10, 0xd6, 0x8a, 0xf8, 0xda, 0xca, 0x7b,
    0x93, 0xae, 0x25, 0xcc, 0x5a, 0x19, 0x4a, 0xe8, 0x8e, 0x14, 0x58, 0x79,
    0xba, 0xe6, 0x26, 0x5a, 0xe7, 0x22, 0xcd, 0x07, 0xda, 0x00, 0x4d, 0xa7,
    0x0e, 0x8c, 0x6c, 0xd4, 0x64, 0xf3, 0x54, 0x76, 0xb4, 0xfa, 0x61, 0x98,
    0x66, 0x61, 0x04, 0xf6, 0x31, 0xc1, 0x91, 0x29, 0x3d, 0x4f, 0xa3, 0xb5,
    0xe8, 0x5c, 0x31, 0xc1, 0x66, 0x09, 0xf5, 0xc3, 0xc6, 0xb7, 0xf2, 0x48,
    0xa8, 0xa7, 0xa9, 0x71, 0x36, 0x13, 0xd5, 0xa8, 0xeb, 0xa9, 0x66, 0x0e,
    0x83, 0x7e, 0x76, 0xa3, 0xe3, 0x22, 0xd7, 0xe6, 0x31, 0x34, 0x73, 0x54,
    0x71, 0x7c, 0xa0, 0x58, 0xa1, 0x62, 0xc2, 0x6e, 0x96, 0x14, 0x40, 0x68,
    0xf0, 0xf4, 0x30, 0xbb, 0x19, 0x66, 0x61, 0x1c, 0xc3, 0x66, 0x75, 0xc1,
    0x78, 0xd8, 0xed, 0x3d, 0xcd, 0x6e, 0xfc, 0x28, 0x4c, 0x22, 0xd7, 0xed,
    0x75, 0xbb, 0x57, 0xd7, 0xa4, 0x43, 0x7a, 0xbd, 0xe3, 0x6e, 0x76, 0xe3,
    0xed, 0xf7, 0x3d, 0x6f, 0x18, 0x33, 0x01, 0x8a, 0xb6, 0xc1, 0x3c, 0xa1,
    0x37, 0xc3, 0x30, 0x61, 0x0b, 0xde, 0x61, 0x92, 0xae, 0xc4, 0x00, 0x2e,
    0x06, 0x34, 0x1f, 0x2e, 0xc2, 0x6c, 0x00, 0x10, 0xd1, 0xde, 0x45, 0x99,
    0x66, 0xde, 0xb0, 0x04, 0x08, 0x29, 0xd3, 0xd5, 0xa0, 0x57, 0x8e, 0x1b,
    0x4e, 0xc7, 0xd3, 0x61, 0x96, 0x0a, 0x86, 0x0c, 0x93, 0x90, 0x2c, 0xba,
    0xbc, 0x1d, 0x02, 0x3c, 0x77, 0x87, 0xff, 0xe8, 0x28, 0x4b, 0xbc, 0x7e,
    0xb7, 0x08, 0x66, 0x79, 0xc8, 0xe3, 0xcd, 0x97, 0x06, 0xa1, 0x8d, 0xab,
    0x14, 0x1c, 0x0c, 0x81, 0xa2, 0x74, 0x62, 0x1a, 0xa5, 0x79, 0xa8, 0xec,
    0x2b, 0x52, 0x4e, 0x87, 0x2b, 0xc6, 0x55, 0x3c, 0xfd, 0x41, 0xbf, 0xd7,
    0xcd, 0x6e, 0x74, 0xcb, 0x64, 0x56, 0xb6, 0x8d, 0x51, 0x25, 0x86, 0x5b,
    0xa0, 0x63, 0xa8, 0x18, 0xe1, 0x8e, 0x80, 0xbd, 0xe6, 0x8b, 0x41, 0xd0,
    0x7d, 0x76, 0x44, 0x57, 0x36, 0x74, 0x1e, 0x55, 0xed, 0xa1, 0xdc, 0xac,
    0xd1, 0xa6, 0x55, 0xb2, 0x97, 0xdd, 0xa8, 0xcf, 0x6b, 0xb5, 0x1f, 0xcf,
    0xba, 0xdd, 0x76, 0xf3, 0x87, 0x74, 0x35, 0x54, 0x87, 0x03, 0x0f, 0x78,
    0x2f, 0xbb, 0x29, 0x78, 0x78, 0x55, 0x5f, 0x04, 0x58, 0x76, 0x58, 0x75,
    0x5d, 0x0e, 0xaf, 0x62, 0x03, 0x88, 0xeb, 0x02, 0x45, 0x49, 0xb8, 0xb1,
    0xe7, 0x81, 0xa7, 0xdb, 0xdb, 0xbe, 0x2c, 0x06, 0x16, 0x00, 0x6d, 0xf5,
    0xfa, 0xb0, 0x8d, 0x6a, 0xc3, 0xf2, 0x30, 0x66, 0x6b, 0x31, 0x38, 0xda,
    0x32, 0x5e, 0x6b, 0x3a, 0x7d, 0x35, 0x34, 0x12, 0x0e, 0x96, 0xc0, 0xec,
    0x6e, 0xea, 0xfb, 0xd0, 0x82, 0x08, 0xc4, 0x42, 0x5e, 0xa1, 0xc3, 0x23,
    0xeb, 0x3b, 0xeb, 0xe6, 0x21, 0x10, 0x76, 0x0c, 0x2b, 0x5c, 0x71, 0xd2,
    0x3e, 0x44, 0xdd, 0x2f, 0x2b, 0x32, 0x0c, 0x33, 0xd4, 0xb9, 0xaf, 0xfe,
    0xb3, 0x72, 0x66, 0x16, 0x0c, 0xaa, 0x41, 0x61, 0x8c, 0xcd, 0xc6, 0xb4,
    0x9f, 0x3f, 0xb7, 0x8e, 0xc9, 0x51, 0x76, 0x03, 0x58, 0x7d, 0xb8, 0x65,
    0x49, 0xef, 0xdf, 0x59, 0x7b, 0xc4, 0x84, 0x6d, 0x14, 0xf4, 0x1d, 0x67,
    0x37, 0x43, 0x7d, 0x18, 0x8f, 0xdb, 0xcb, 0xdd, 0xfd, 0xa6, 0x46, 0x0b,
    0x9e, 0x3d, 0x7b, 0x66, 0x37, 0xa2, 0x03, 0x2a, 0x11, 0xb6, 0x69, 0x2d,
    0x2d, 0x62, 0x6c, 0x98, 0xc6, 0x4d, 0x85, 0xac, 0xe1, 0xdf, 0x41, 0x76,
    0x43, 0x14, 0x29, 0x59, 0xb1, 0x1b, 0x97, 0x71, 0x22, 0xf2, 0xc5, 0xcc,
    0xb7, 0xeb, 0x90, 0xfe, 0xd1, 0x37, 0xbe, 0xcc, 0x43, 0x2e, 0xb2, 0x30,
    0xa7, 0x5c, 0x7a, 0xf5, 0x1e, 0x55, 0x6c, 0xcd, 0x6d, 0x5d, 0x2a, 0x1a,
    0xe8, 0x15, 0x01, 0x8b, 0x52, 0xde, 0xd1, 0x28, 0x54, 0x4d, 0xf2, 0xe0,
    0xb0, 0x9a, 0xe5, 0x41, 0x0d, 0xe5, 0xb4, 0x27, 0x5c, 0x80, 0x59, 0xf9,
    0x66, 0x15, 0xde, 0xe8, 0xe3, 0xa9, 0x90, 0x8f, 0x06, 0x6c, 0x04, 0xe9,
    0xb2, 0x76, 0x0f, 0x56, 0xcc, 0xec, 0x3a, 0x78, 0x30, 0x0e, 0xe1, 0x7f,
    0x1d, 0x49, 0x57, 0x19, 0x28, 0x74, 0x01, 0xb5, 0xaf, 0x57, 0x70, 0x69,
    0x44, 0x63, 0x2f, 0xb7, 0xef, 0xaf, 0x18, 0x07, 0x14, 0xd7, 0xf5, 0x7b,
    0xf3, 0xdc, 0xf3, 0x10, 0x12, 0x90, 0x94, 0x08, 0x35, 0xbb, 0xf6, 0x9c,
    0x90, 0x70, 0x7a, 0x5f, 0x03, 0x2a, 0xcf, 0xac, 0xd9, 0x01, 0x2a, 0xb5,
    0x10, 0x4d, 0xd7, 0xde, 0x0e, 0x43, 0xd9, 0xe0, 0xc3, 0x1b, 0x6a, 0xba,
    0x68, 0x9d, 0xf2, 0x67, 0xc7, 0xd5, 0xb8, 0x82, 0x6b, 0x16, 0xd3, 0x0d,
    0xce, 0x4d, 0x4d, 0x69, 0xd0, 0xdb, 0xef, 0xf4, 0x8a, 0x40, 0x67, 0x77,
    0xe0, 0xfc, 0x30, 0xbe, 0xf0, 0x31, 0x80, 0x51, 0x9a, 0x2b, 0xe7, 0xb2,
    0xfa, 0x39, 0xfa, 0xfb, 0x5a, 0x48, 0x36, 0xc7, 0xdb, 0x27, 0x70, 0xd6,
    0x03, 0xc0, 0x2e, 0xb4, 0x33, 0xa3, 0xf2, 0x9a, 0x52, 0xbe, 0xeb, 0x94,
    0xe0, 0x79, 0x0e, 0xe8, 0x2d, 0x9d, 0xe5, 0xe9, 0x75, 0x49, 0x9f, 0x49,
    0x97, 0xf4, 0x1b, 0xe0, 0x6f, 0x30, 0xa3, 0x0d, 0xf0, 0xc7, 0x75, 0xd4,
    0x00, 0xe7, 0x01, 0x31, 0x0e, 0x82, 0x16, 0xc8, 0x1d, 0x07, 0x6b, 0xe0,
    0xd2, 0xa3, 0x50, 0xd0, 0x36, 0xca, 0x3b, 0xa6, 0xab, 0x62, 0xd9, 0xf3,
    0x97, 0x7d, 0x7f, 0x79, 0xe0, 0x2f, 0x0f, 0x2b, 0xde, 0xc0, 0xe6, 0x4b,
    0x2d, 0x7e, 0xd4, 0xb0, 0xa8, 0x15, 0xa3, 0x59, 0x2c, 0x7b, 0x9b, 0xaa,
    0xfb, 0xfe, 0x01, 0x90, 0xbe, 0xbe, 0x95, 0xd2, 0x7b, 0x0e, 0x29, 0x07,
    0x9b, 0x06, 0xda, 0x5e, 0x1e, 0xda, 0x29, 0x07, 0x3b, 0xe6, 0xa9, 0xf7,
    0x49, 0x53, 0xb1, 0x67, 0xb0, 0x4a, 0x0c, 0xae, 0xe4, 0xdb, 0xd0, 0xac,
    0x1e, 0x3a, 0x10, 0x19, 0xd2, 0x25, 0xd0, 0x64, 0x11, 0x60, 0x96, 0x1f,
    0x54, 0x6e, 0xf3, 0x9b, 0xfb, 0x91, 0x09, 0xee, 0x03, 0x60, 0x54, 0x50,
    0x85, 0xda, 0x50, 0x0a, 0x10, 0x18, 0xe6, 0x1d, 0x88, 0x74, 0xca, 0xe0,
    0xd2, 0xdc, 0x3b, 0x38, 0x8a, 0xe9, 0xc2, 0xb7, 0x81, 0xd7, 0xfe, 0xe8,
    0x7b, 0x5e, 0x11, 0x58, 0x91, 0x44, 0x37, 0x0f, 0x3f, 0x3c, 0x87, 0xdb,
    0x0e, 0xcf, 0x71, 0x45, 0x71, 0x90, 0x32, 0xc1, 0x59, 0x0a, 0xac, 0x98,
    0xa3, 0x9b, 0xf2, 0x24, 0x74, 0x4b, 0x7a, 0xb2, 0xf5, 0x88, 0xf5, 0x1f,
    0x76, 0xc6, 0x12, 0x3a, 0x97, 0x83, 0x83, 0x1d, 0xcc, 0x8e, 0x75, 0xd0,
    0x6a, 0x83, 0xd8, 0x4a, 0x83, 0xef, 0x5f, 0xef, 0xee, 0x57, 0x01, 0xeb,
    0x21, 0x5d, 0x35, 0x3a, 0x44, 0x6f, 0x49, 0xbf, 0x96, 0x16, 0x36, 0x06,
    0x00, 0xbb, 0x39, 0x4f, 0xd2, 0xeb, 0x81, 0x92, 0xbd, 0xab, 0xee, 0xca,
    0x44, 0x9a, 0x24, 0x2c, 0x13, 0x4c, 0x0c, 0xd1, 0x87, 0x13, 0xfb, 0xa2,
    0x03, 0x9e, 0x42, 0x98, 0x0e, 0x7b, 0xc1, 0x2b, 0xf8, 0x34, 0x34, 0xb6,
    0x46, 0x72, 0x8e, 0x1a, 0x0b, 0x51, 0x27, 0xcb, 0x75, 0x96, 0x37, 0xb0,
    0x42, 0x9f, 0xb6, 0x59, 0x0b, 0xc0, 0x6a, 0xf0, 0xd1, 0x81, 0x11, 0x0c,
    0x9a, 0xc3, 0x80, 0xc3, 0x5d, 0xa0, 0x73, 0x7b, 0xbb, 0x26, 0xa0, 0xc0,
    0x2d, 0xe8, 0xc5, 0x26, 0xa6, 0xc7, 0x5f, 0x06, 0x0b, 0x83, 0x5d, 0x0f,
    0x1f, 0x44, 0x77, 0xb7, 0x71, 0x36, 0x6a, 0x7c, 0xf7, 0x2d, 0x00, 0xe4,
    0x07, 0x10, 0xc0, 0xb5, 0xce, 0xb6, 0xa8, 0x8c, 0x92, 0x5e, 0xf7, 0x9e,
    0x55, 0xa4, 0xac, 0xf7, 0x6c, 0x2b, 0xc5, 0xae, 0x1d, 0x2a, 0x0c, 0x0f,
    0x53, 0x9f, 0xfb, 0x0e, 0x3e, 0x48, 0xcf, 0x02, 0x2e, 0xe1, 0x03, 0xa5,
    0x9b, 0xb0, 0xe6, 0xf5, 0xdc, 0xac, 0x30, 0x8e, 0xf0, 0x1e, 0x8a, 0x6f,
    0xee, 0xd7, 0xf3, 0x79, 0x11, 0xc8, 0x34, 0x4d, 0x66, 0x61, 0xee, 0x07,
    0x18, 0x33, 0x4e, 0x7b, 0x8f, 0x3d, 0x94, 0xbf, 0x6a, 0xec, 0x77, 0x51,
    0x6f, 0xa4, 0x49, 0x4f, 0xb0, 0x30, 0xe5, 0x71, 0x0d, 0x2c, 0xfa, 0xd5,
    0x2d, 0x0d, 0x65, 0x66, 0x86, 0x29, 0x80, 0x93, 0x6a, 0xb8, 0x81, 0xa7,
    0x5f, 0xc7, 0x89, 0x1d, 0x59, 0xe4, 0xf5, 0x78, 0x27, 0x4e, 0xd1, 0x0b,
    0xfa, 0x85, 0xa3, 0xa1, 0xad, 0x6c, 0xda, 0x4b, 0x69, 0x30, 0x8a, 0xee,
    0x7a, 0x1b, 0x05, 0xb0, 0x97, 0x59, 0xb7, 0xa3, 0x61, 0x6b, 0x47, 0x6b,
    0x7d, 0x75, 0xc4, 0xa8, 0xb4, 0x0b, 0x58, 0x6c, 0x56, 0x11, 0x28, 0xfe,
    0x69, 0xb3, 0xa5, 0x53, 0xcd, 0x59, 0x55, 0x9d, 0x86, 0xd1, 0xf3, 0xb9,
    0xde, 0x90, 0xaf, 0x44, 0xe3, 0xdb, 0x79, 0x20, 0xb8, 0x00, 0xa1, 0x27,
    0x66, 0x1d, 0x38, 0x70, 0x53, 0x63, 0x96, 0x6b, 0x29, 0xb6, 0x6a, 0x0a,
    0x6b, 0x1c, 0x7d, 0x35, 0xff, 0xfb, 0xf4, 0xa8, 0x5b, 0xc3, 0xd0, 0xb5,
    0x6b, 0xbb, 0x3e, 0x5a, 0xdd, 0xee, 0x37, 0x35, 0x76, 0xc9, 0x86, 0x93,
    0xe3, 0xe6, 0xc6, 0x77, 0xbf, 0x1e, 0x70, 0x1e, 0x04, 0x27, 0x85, 0x75,
    0x27, 0xaf, 0xdd, 0xc5, 0x37, 0xbb, 0xc1, 0x41, 0xd5, 0x39, 0x47, 0x3f,
    0x51, 0xe3, 0x10, 0x7b, 0x61, 0x66, 0x75, 0xac, 0x99, 0x40, 0x83, 0x34,
    0xf0, 0xee, 0xa2, 0x39, 0x35, 0xc4, 0x72, 0x7e, 0x60, 0x69, 0xac, 0x37,
    0x8d, 0x03, 0x54, 0x31, 0x93, 0x07, 0x5f, 0x37, 0xe1, 0xa7, 0xf7, 0x60,
    0xd5, 0x7a, 0xff, 0xc4, 0x82, 0xa4, 0x16, 0x56, 0xd7, 0x06, 0x28, 0xa0,
    0xb1, 0xf4, 0x03, 0x15, 0x15, 0x13, 0x7e, 0x37, 0x40, 0x4e, 0x93, 0x08,
    0xbb, 0x76, 0xb7, 0xaa, 0x6d, 0x2a, 0xaa, 0xaf, 0x2f, 0x22, 0xa0, 0xfb,
    0x79, 0x55, 0x03, 0xb0, 0xc3, 0xdf, 0x98, 0xb7, 0xa8, 0xe0, 0xa4, 0x9c,
    0x34, 0x58, 0xdd, 0x6c, 0x6a, 0x6c, 0x85, 0x95, 0xd1, 0x12, 0x15, 0xfc,
    0x6a, 0x8a, 0x5e, 0x6f, 0xf6, 0x81, 0xdc, 0x8a, 0xb5, 0xd4, 0x7d, 0x6b,
    0xc8, 0x5f, 0x83, 0xe4, 0x9f, 0x1a, 0x6e, 0x0d, 0x83, 0x07, 0x6b, 0x70,
    0x7d, 0x5e, 0x91, 0xb7, 0xe7, 0x0f, 0xb9, 0x8f, 0x22, 0x92, 0x28, 0x29,
    0xac, 0x6e, 0x4b, 0x5f, 0x4c, 0x77, 0x11, 0xa9, 0x22, 0xb0, 0x2c, 0x7a,
    0x1e, 0x82, 0x72, 0x1e, 0x00, 0x24, 0xd6, 0xbc, 0x0e, 0xeb, 0x07, 0xee,
    0xb8, 0x5f, 0x3f, 0x38, 0x31, 0x44, 0xc7, 0xf8, 0x12, 0xaa, 0xd8, 0x25,
    0x2f, 0xc1, 0x91, 0x34, 0x99, 0x18, 0xc5, 0xf7, 0x5a, 0x73, 0x22, 0xb3,
    0x26, 0x0f, 0xa1, 0x4e, 0xf8, 0x43, 0x91, 0x75, 0x6f, 0x9e, 0x93, 0xde,
    0xdc, 0xbe, 0x84, 0x55, 0xf5, 0xc9, 0x3a, 0xa9, 0xae, 0x43, 0x7a, 0x04,
    0x8a, 0x31, 0x56, 0x98, 0x26, 0xbc, 0x29, 0x31, 0x0d, 0xc8, 0x9d, 0x2b,
    0xb0, 0x44, 0x69, 0xd0, 0xe3, 0xca, 0x00, 0xa1, 0x7d, 0x84, 0x8f, 0xb7,
    0x1d, 0xe1, 0x2a, 0x16, 0xca, 0xe6, 0x37, 0xc2, 0x3e, 0xcd, 0x93, 0x62,
    0xf7, 0x31, 0x36, 0xe2, 0xd9, 0x07, 0xac, 0x12, 0x4c, 0x88, 0x68, 0x72,
    0x86, 0x73, 0x45, 0x92, 0x86, 0xd1, 0x8c, 0x77, 0xde, 0x68, 0x6b, 0x88,
    0xa3, 0x67, 0x10, 0x47, 0x5d, 0xd0, 0x3b, 0x84, 0xd5, 0x29, 0x19, 0x31,
    0x00, 0xec, 0xf6, 0xf8, 0x06, 0x83, 0xce, 0x35, 0x9d, 0x5d, 0x32, 0xe0,
    0x33, 0xd1, 0x35, 0x1d, 0xee, 0xee, 0x97, 0x34, 0x2f, 0xc7, 0xbd, 0xb3,
    0xa2, 0x52, 0xa5, 0x6d, 0x0c, 0xdc, 0x3a, 0xbf, 0xfc, 0xfc, 0xff, 0x1c,
    0x8b, 0x70, 0xf6, 0x77, 0xdd, 0xad, 0x91, 0x57, 0x50, 0xd2, 0xd1, 0xa0,
    0x77, 0x24, 0xec, 0xb6, 0xcf, 0xc1, 0x99, 0xfe, 0xa2, 0xd9, 0x43, 0x75,
    0x77, 0xc9, 0x53, 0x00, 0x4a, 0xf7, 0x79, 0x37, 0xa6, 0x0b, 0xcf, 0x54,
    0xdc, 0x22, 0x24, 0xb8, 0x4f, 0xd8, 0x56, 0x0b, 0x69, 0xa3, 0x6f, 0x38,
    0x75, 0x2c, 0xd5, 0x2a, 0x83, 0x88, 0xac, 0x7d, 0x90, 0x4c, 0x39, 0x13,
    0xb0, 0xc6, 0xd0, 0xc9, 0x9e, 0xc5, 0x58, 0xf7, 0xb6, 0xa2, 0x9e, 0x12,
    0xd5, 0xd4, 0x90, 0xd0, 0xd1, 0xd1, 0x51, 0x1b, 0xc1, 0x03, 0x67, 0x36,
    0x9f, 0xcf, 0x0f, 0x8f, 0x6c, 0x99, 0x0b, 0xe3, 0x82, 0x4a, 0xb8, 0x84,
    0x83, 0xd6, 0x42, 0xab, 0x64, 0x8e, 0x9b, 0xc3, 0xd1, 0x61, 0xfd, 0x77,
    0x72, 0x85, 0xad, 0xf2, 0x18, 0x6e, 0x67, 0x13, 0x72, 0xb6, 0x52, 0xb7,
    0x8d, 0x7a, 0x76, 0x07, 0xb3, 0x49, 0xd0, 0x3f, 0x16, 0x04, 0x7c, 0xd6,
    0xc0, 0x80, 0x5a, 0x9f, 0x1b, 0x15, 0x8e, 0xbf, 0x6c, 0x10, 0x95, 0x20,
    0x25, 0x6c, 0x5a, 0xb2, 0x57, 0x2d, 0xf4, 0x6d, 0x9f, 0xb9, 0xb2, 0x26,
    0x86, 0x9d, 0x79, 0xc8, 0x91, 0x31, 0xa7, 0xe5, 0xa8, 0x3c, 0x2d, 0xbd,
    0x2e, 0x92, 0xcf, 0x23, 0xfc, 0xbf, 0x26, 0xa5, 0x70, 0x29, 0xc3, 0x53,
    0x04, 0xbb, 0xff, 0xbc, 0x71, 0x81, 0xa3, 0x3c, 0x2e, 0x02, 0x13, 0x1a,
    0xe4, 0x5e, 0xe4, 0x9d, 0xa7, 0xd7, 0xf7, 0xc9, 0x5f, 0x1b, 0xdc, 0x5d,
    0x11, 0x34, 0x69, 0xd7, 0x43, 0x11, 0x64, 0xb7, 0x81, 0xb6, 0xfa, 0x55,
    0x63, 0x1b, 0xfb, 0xac, 0x0f, 0x7f, 0x03, 0xf1, 0xde, 0x51, 0xd5, 0xb6,
    0x3e, 0x41, 0x0f, 0x91, 0x83, 0xd5, 0xe5, 0x45, 0xcf, 0xad, 0x36, 0x7e,
    0x0b, 0xf6, 0xfd, 0xb8, 0xd1, 0x1e, 0x09, 0x54, 0x84, 0xca, 0x2d, 0x12,
    0xc3, 0x32, 0x10, 0xe5, 0x83, 0x0e, 0xbe, 0x22, 0x00, 0x65, 0x1d, 0x62,
    0x73, 0xb9, 0x90, 0x68, 0x58, 0xdc, 0x9a, 0x16, 0xe9, 0xb0, 0x85, 0x4f,
    0xed, 0x26, 0x94, 0xfb, 0x80, 0xd2, 0xd3, 0xc1, 0x46, 0x91, 0x35, 0xeb,
    0xac, 0x52, 0x9e, 0x22, 0x3f, 0xe4, 0x97, 0xbf, 0x1a, 0x2c, 0x79, 0xd0,
    0x88, 0x7e, 0x67, 0xb3, 0xa9, 0x6a, 0xf6, 0xf5, 0x0c, 0xd5, 0x74, 0xb7,
    0x4a, 0x27, 0x1a, 0x29, 0xf9, 0xcd, 0x96, 0x4c, 0x46, 0x43, 0xd7, 0xb7,
    0xf3, 0x72, 0xb3, 0x53, 0xef, 0x51, 0xce, 0xff, 0x69, 0xbd, 0x67, 0x85,
    0x9b, 0x77, 0xf6, 0x5f, 0xcf, 0xde, 0x2e, 0x98, 0xb0, 0x03, 0x98, 0x6c,
    0xd4, 0x56, 0x09, 0x9a, 0xcc, 0xd5, 0x49, 0xe4, 0xa9, 0x04, 0x9e, 0xba,
    0xa6, 0xb7, 0xe9, 0xd6, 0x61, 0xfd, 0xf1, 0xd1, 0xf1, 0x61, 0xb7, 0x57,
    0xa9, 0xde, 0xe7, 0xf4, 0xe9, 0x2c, 0xde, 0xa5, 0xd8, 0xb1, 0x04, 0x8e,
    0xda, 0xe6, 0xac, 0x0d, 0x2e, 0xbf, 0x5a, 0x98, 0x5c, 0x17, 0x86, 0xd6,
    0x7a, 0xf8, 0xb5, 0x7c, 0xaf, 0xb2, 0xf2, 0x53, 0xa0, 0xb9, 0x29, 0xf5,
    0x87, 0xe1, 0x4c, 0xa4, 0xc9, 0x5a, 0xd2, 0x52, 0x21, 0xdb, 0x2d, 0x4b,
    0xc2, 0xd9, 0x68, 0x74, 0xa3, 0xa0, 0xf8, 0xb0, 0x5f, 0x11, 0xa0, 0xfe,
    0x41, 0x73, 0x09, 0x4b, 0x22, 0x63, 0x56, 0xac, 0x8f, 0xac, 0x84, 0xe9,
    0x30, 0xa7, 0x49, 0x08, 0x64, 0xa3, 0xd6, 0xcd, 0x20, 0x9c, 0x4b, 0x94,
    0x36, 0x69, 0x52, 0xef, 0x0c, 0xdb, 0x03, 0xdc, 0x21, 0x55, 0x32, 0x92,
    0xce, 0xa1, 0x11, 0xf3, 0xdd, 0xcf, 0x88, 0x83, 0x9a, 0xbf, 0xc5, 0x20,
    0xd8, 0x4b, 0x33, 0xd0, 0xae, 0x67, 0xdf, 0xe1, 0xf4, 0x77, 0xd3, 0xb6,
    0x9d, 0x75, 0xf4, 0x5c, 0x2a, 0xa6, 0x02, 0x7f, 0x01, 0x8e, 0x72, 0x41,
    0x94, 0x0e, 0x60, 0x0a, 0x4d, 0x3c, 0x18, 0xb7, 0x20, 0x12, 0x2c, 0xab,
    0x98, 0xfd, 0x33, 0x6a, 0x21, 0x3b, 0x4b, 0xa3, 0x8c, 0xea, 0x1e, 0xa6,
    0xa8, 0x85, 0x25, 0x50, 0xd2, 0xb5, 0x6b, 0xac, 0x57, 0x96, 0xd3, 0x8d,
    0x7d, 0xd5, 0xca, 0x72, 0xda, 0xd1, 0x72, 0xcb, 0x92, 0x41, 0xee, 0x3f,
    0x6b, 0x31, 0xc8, 0x75, 0x8b, 0x94, 0x23, 0xf8, 0x67, 0xce, 0x0d, 0xa5,
    0x74, 0x58, 0x27, 0x27, 0x5b, 0x4f, 0x90, 0x62, 0x2b, 0x4f, 0x39, 0x18,
    0x3b, 0x81, 0x5f, 0x61, 0xe8, 0x6f, 0x47, 0x71, 0xc5, 0x63, 0x7c, 0x67,
    0xb4, 0x82, 0xd9, 0x39, 0xbb, 0xa1, 0xf1, 0x50, 0x69, 0x6e, 0x7b, 0x4a,
    0x2c, 0xa6, 0x94, 0xe4, 0x4f, 0x35, 0x5b, 0xaf, 0x67, 0xff, 0xac, 0xcd,
    0xc3, 0x36, 0xf4, 0xed, 0x8f, 0x7b, 0xbd, 0x9e, 0x25, 0xa1, 0xaa, 0x49,
    0xcf, 0x0f, 0x77, 0x48, 0xcf, 0xdb, 0x93, 0x29, 0x8f, 0xce, 0x70, 0xcb,
    0xa6, 0xff, 0xc5, 0x85, 0x33, 0x5c, 0xe7, 0x4a, 0xfb, 0x62, 0xa8, 0xd1,
    0x67, 0x07, 0x2d, 0xe8, 0x85, 0xe2, 0xd5, 0x8c, 0xfa, 0xfe, 0xa8, 0xab,
    0xa7, 0x1c, 0xc0, 0xd3, 0xa5, 0x1b, 0xd3, 0x7c, 0xcf, 0x6a, 0x1e, 0x77,
    0xee, 0x5c, 0xdd, 0x0a, 0x2e, 0x6a, 0xfb, 0xf9, 0x48, 0xd9, 0xbd, 0x85,
    0x5c, 0x16, 0xff, 0x7c, 0x49, 0x6f, 0xe7, 0x79, 0xb8, 0xa2, 0x82, 0x6c,
    0x63, 0xb5, 0x36, 0xdd, 0x6f, 0xfc, 0xa7, 0x47, 0xdf, 0x6c, 0x9a, 0xc7,
    0xa3, 0xa5, 0x0a, 0x45, 0x0e, 0x70, 0x3e, 0x9f, 0x1f, 0xf9, 0xf0, 0x85,
    0x28, 0x09, 0xe5, 0x79, 0x32, 0xdd, 0x7c, 0x91, 0x67, 0x2c, 0xb9, 0x6f,
    0x1d, 0x9a, 0x6c, 0xab, 0xdc, 0xbb, 0x08, 0xaa, 0x50, 0x4c, 0x81, 0x0e,
    0x6d, 0xf0, 0x3f, 0x15, 0x36, 0xfe, 0x33, 0x5e, 0x04, 0xdd, 0x0a, 0x1c,
    0x8e, 0x81, 0x73, 0xf3, 0x36, 0xda, 0xac, 0xa3, 0xa2, 0xda, 0xca, 0x50,
    0xa2, 0x26, 0xbc, 0xb0, 0x2c, 0x12, 0x7c, 0xdb, 0x80, 0x00, 0x57, 0x1d,
    0xb5, 0xae, 0x3b, 0x99, 0xad, 0x9a, 0xb0, 0xc5, 0xa8, 0x25, 0xfd, 0xdd,
    0xea, 0xc9, 0xba, 0x96, 0xe9, 0xab, 0x58, 0x9a, 0x06, 0x53, 0xbb, 0x9b,
    0xff, 0x7b, 0xde, 0xb5, 0x99, 0xc6, 0x9d, 0xe5, 0xb6, 0xde, 0x2e, 0xef,
    0xb9, 0x50, 0x42, 0xd3, 0x78, 0x55, 0xde, 0x52, 0x8f, 0x80, 0x9d, 0xc1,
    0x20, 0x09, 0x85, 0xec, 0x44, 0x4b, 0x96, 0xc4, 0xf5, 0x25, 0x6c, 0x6f,
    0xce, 0x51, 0xdf, 0xde, 0x1c, 0x8d, 0x75, 0x8e, 0x9e, 0xda, 0xe6, 0x26,
    0x75, 0x6d, 0xa3, 0xad, 0x6b, 0xb7, 0x75, 0xe0, 0x87, 0x47, 0x36, 0xa2,
    0xfa, 0x7a, 0x01, 0x93, 0xb5, 0x1b, 0x7e, 0x25, 0xed, 0xf3, 0x2d, 0xb1,
    0x82, 0x5f, 0xe3, 0x48, 0xef, 0x59, 0xcd, 0x2f, 0x32, 0x9a, 0x2d, 0xbd,
    0xf4, 0xc6, 0x26, 0x01, 0x78, 0x47, 0x40, 0x2d, 0x53, 0xab, 0xdc, 0xd8,
    0x28, 0x2c, 0x76, 0x6a, 0x17, 0x76, 0x08, 0x14, 0x77, 0xb5, 0xdf, 0x90,
    0x89, 0x7d, 0x59, 0xb6, 0x24, 0x64, 0x0e, 0x0f, 0x68, 0xb7, 0x4e, 0x77,
    0x93, 0x91, 0x1e, 0x3e, 0xfc, 0x8a, 0x52, 0x14, 0x60, 0x61, 0x89, 0x96,
    0x95, 0x27, 0xfb, 0x30, 0xd1, 0xf1, 0x09, 0x5c, 0xf4, 0xc6, 0x27, 0x0a,
    0x28, 0xc6, 0xd5, 0x2b, 0x82, 0x08, 0x11, 0xe6, 0xdd, 0xc0, 0xc7, 0x3a,
    0x4e, 0x41, 0x2d, 0xd0, 0xab, 0x79, 0xa0, 0xfb, 0x4d, 0x38, 0x13, 0xc4,
    0x44, 0xbf, 0x34, 0xaf, 0x75, 0x2f, 0xd3, 0x15, 0xd5, 0xa1, 0x66, 0xc1,
    0x47, 0xee, 0xc5, 0xab, 0x3f, 0xbe, 0xf8, 0xf4, 0xfa, 0x0f, 0xe4, 0xcd,
    0x8b, 0xef, 0x3f, 0x5a, 0x6e, 0x81, 0x8d, 0x5a, 0xcd, 0xe0, 0xa9, 0xfb,
    0xe1, 0xf8, 0x04, 0x0d, 0x78, 0xac, 0x5e, 0x21, 0x9a, 0x25, 0xd1, 0x5b,
    0x05, 0xcf, 0x67, 0x85, 0x8d, 0x21, 0x8e, 0x95, 0xcf, 0x05, 0xd6, 0x2d,
    0xf3, 0xca, 0x97, 0x93, 0xc6, 0x3b, 0x3c, 0xf5, 0xea, 0xc5, 0xf5, 0x33,
    0x70, 0x63, 0xf5, 0xb4, 0x49, 0x3d, 0xcf, 0xf8, 0x4b, 0x8c, 0xf5, 0x73,
    0xb1, 0x98, 0xbb, 0xcf, 0xc3, 0xab, 0x5a, 0x1c, 0xeb, 0x9a, 0x0d, 0x91,
    0x09, 0xb9, 0xcb, 0x30, 0x32, 0x7f, 0x69, 0x6f, 0x5f, 0x85, 0xe3, 0x2f,
    0xcd, 0x70, 0xcc, 0x5b, 0xb1, 0x63, 0xfd, 0x98, 0xed, 0x9b, 0x14, 0x41,
    0xf1, 0x97, 0x9f, 0xff, 0xd3, 0x2c, 0x89, 0x61, 0x31, 0x20, 0xcc, 0xb6,
    0xed, 0xa6, 0x5b, 0xc6, 0xa8, 0xac, 0xac, 0x5d, 0xea, 0xbb, 0xf5, 0x09,
    0xcb, 0xe9, 0x88, 0xfc, 0x04, 0xeb, 0x3a, 0xe3, 0x5f, 0xfe, 0xcf, 0xff,
    0x6e, 0xb8, 0x3d, 0x2a, 0xa8, 0x00, 0x48, 0x00, 0x14, 0x3c, 0x3e, 0x11,
    0xc6, 0x40, 0x28, 0x36, 0xcf, 0x77, 0x96, 0x5d, 0x01, 0x9e, 0x25, 0xc6,
    0x0c, 0xc0, 0xa9, 0x4d, 0xbf, 0x71, 0x94, 0x54, 0xe6, 0xf8, 0x24, 0x2b,
    0x5d, 0x28, 0x95, 0x11, 0x07, 0xbc, 0x4c, 0x5c, 0xc1, 0xd0, 0xc9, 0x7e,
    0x36, 0x3e, 0x59, 0xf6, 0xda, 0x40, 0xb1, 0xec, 0xd5, 0xa2, 0xfc, 0xe3,
    0x50, 0xea, 0xaf, 0x4e, 0xd6, 0x22, 0xec, 0xa2, 0x97, 0x25, 0x46, 0x76,
    0x80, 0x42, 0xd6, 0xda, 0xa9, 0x36, 0x60, 0x98, 0xd5, 0x6c, 0x54, 0xa8,
    0xf1, 0xc6, 0x03, 0xa5, 0x2a, 0xfc, 0xb8, 0x55, 0x5e, 0x67, 0xdb, 0xef,
    0x71, 0x62, 0x1b, 0xb5, 0x04, 0x7b, 0xb5, 0x3f, 0xe2, 0xfb, 0x96, 0xe5,
    0xf8, 0xca, 0xc5, 0xd5, 0x0b, 0x53, 0x5f, 0xd7, 0x12, 0x3a, 0xed, 0x95,
    0xfd, 0x95, 0x0b, 0x6a, 0xbd, 0x6a, 0xa2, 0x17, 0xb4, 0xbf, 0x1b, 0xe6,
    0x97, 0xfd, 0xd6, 0xc2, 0xd6, 0x5f, 0x26, 0xac, 0xad, 0x6b, 0x17, 0x02,
    0xe4, 0xf4, 0x9e, 0x5a, 0xa1, 0xfe, 0xeb, 0x0b, 0x9b, 0x55, 0x71, 0x52,
    0x65, 0x9e, 0xaa, 0x57, 0xa7, 0xd7, 0x19, 0x84, 0x43, 0xea, 0x3d, 0xdd,
    0xe5, 0x21, 0x1b, 0x10, 0x70, 0x40, 0xd5, 0xc1, 0x71, 0xa2, 0x90, 0x13,
    0xe5, 0xd3, 0x40, 0x42, 0x12, 0xb3, 0xf9, 0x9c, 0x82, 0x2a, 0x12, 0xde,
    0x84, 0x0b, 0x70, 0x2a, 0xd6, 0x7a, 0x68, 0xd4, 0xec, 0xd4, 0x0e, 0x44,
    0xe9, 0x6d, 0x5d, 0x86, 0x8c, 0xd5, 0x91, 0x15, 0xaa, 0xf7, 0xe5, 0x76,
    0x2e, 0x45, 0xe3, 0xb9, 0x3c, 0x68, 0xae, 0xe1, 0x35, 0x8a, 0xf3, 0x99,
    0xdd, 0x92, 0xd3, 0xb3, 0xaa, 0x74, 0xb9, 0x6c, 0x6d, 0x57, 0xee, 0xc6,
    0xd3, 0x53, 0xca, 0x7f, 0x1c, 0x1f, 0x98, 0xa8, 0xdc, 0x7a, 0xb7, 0x03,
    0xa6, 0xf1, 0xe0, 0x6d, 0x38, 0x5d, 0xa3, 0x8b, 0xb7, 0xed, 0x7b, 0x4c,
    0x14, 0xe1, 0x6d, 0xc1, 0x75, 0xfd, 0xad, 0xb8, 0xfa, 0xc3, 0x8d, 0xe8,
    0xea, 0x67, 0x6a, 0x00, 0xe5, 0xad, 0x6d, 0x3a, 0xfa, 0x5f, 0x96, 0x20,
    0x65, 0xe9, 0xe4, 0xaa, 0xbe, 0x6c, 0x3f, 0x68, 0xcb, 0xbc, 0x6b, 0x37,
    0x40, 0x56, 0x7e, 0x47, 0x0a, 0x1c, 0x0f, 0x5a, 0x3d, 0x9a, 0x16, 0x1a,
    0x0f, 0xc3, 0x9d, 0xec, 0x2f, 0x0f, 0xcc, 0x48, 0xb7, 0x46, 0xbe, 0x45,
    0x74, 0x5a, 0xf7, 0xda, 0xb5, 0x0e, 0x2a, 0x3c, 0xd9, 0xf3, 0x12, 0x73,
    0x9b, 0x18, 0xae, 0x8a, 0x1e, 0x5c, 0xf7, 0x7b, 0xd5, 0xcd, 0xab, 0x99,
    0x62, 0x48, 0xe2, 0x7a, 0x49, 0xf0, 0x7b, 0x6d, 0x97, 0xb1, 0x16, 0xa4,
    0xe4, 0x64, 0x9c, 0xb1, 0x8e, 0x9e, 0x6c, 0x83, 0x1b, 0xaf, 0xc7, 0xdc,
    0x2e, 0x7d, 0x6b, 0x9b, 0x11, 0xb7, 0x6b, 0x2f, 0xf8, 0xbc, 0x41, 0x27,
    0x18, 0x32, 0xcf, 0xd3, 0x15, 0xfa, 0x7e, 0x1b, 0x9f, 0x69, 0xec, 0x7a,
    0xd7, 0x9b, 0x27, 0xef, 0x31, 0xba, 0x33, 0x06, 0x36, 0x5e, 0xc0, 0x13,
    0x27, 0x0c, 0xce, 0x13, 0x86, 0x0d, 0xa2, 0xb9, 0xe5, 0x48, 0xd7, 0x7c,
    0xc3, 0x44, 0x8f, 0xb9, 0x7a, 0x65, 0xb1, 0x31, 0x5a, 0xf4, 0xf2, 0x55,
    0x47, 0x15, 0x63, 0xee, 0x3a, 0x31, 0x8d, 0xd8, 0x0a, 0x9e, 0x31, 0xa8,
    0x0d, 0xb9, 0xf7, 0xbc, 0x1f, 0xf4, 0x9e, 0x1e, 0x07, 0xbd, 0x00, 0xdf,
    0x9b, 0xd8, 0x6f, 0xb6, 0x6f, 0xaf, 0x89, 0x7e, 0x4d, 0xc5, 0x0e, 0x97,
    0x5d, 0x3a, 0x0e, 0xb7, 0xde, 0xd0, 0x81, 0x77, 0xfa, 0xc6, 0xef, 0x33,
    0x8a, 0x0f, 0xb1, 0xec, 0x7c, 0x12, 0x05, 0x5e, 0x6b, 0x01, 0x58, 0xb2,
    0x1f, 0x6d, 0x71, 0x39, 0xa5, 0xb8, 0x8c, 0x69, 0x4e, 0xf0, 0x4d, 0x1c,
    0x0c, 0xa5, 0xb1, 0x3b, 0x0c, 0xb6, 0x19, 0xa9, 0x76, 0x7e, 0x6b, 0xac,
    0x43, 0xe9, 0x13, 0xa7, 0x81, 0xa1, 0xfa, 0x06, 0x46, 0x3d, 0xd2, 0x41,
    0x90, 0x20, 0x7e, 0xe7, 0xb5, 0x55, 0xb6, 0xbe, 0xad, 0x14, 0xbc, 0x29,
    0x67, 0x49, 0xc8, 0x2f, 0x09, 0x9b, 0x93, 0x35, 0x5f, 0x0b, 0x08, 0xe8,
    0x6d, 0xad, 0x55, 0x8b, 0xf6, 0xd8, 0xf6, 0x2e, 0x4e, 0xe3, 0x44, 0x28,
    0xb7, 0xf1, 0x36, 0xda, 0x33, 0x6f, 0x59, 0x1b, 0x7f, 0xe8, 0x16, 0xad,
    0x87, 0x46, 0x2b, 0xcc, 0x61, 0x7b, 0xaa, 0x97, 0xe8, 0xcb, 0x4e, 0xfb,
    0xd5, 0x48, 0xe0, 0x85, 0x79, 0xcb, 0xde, 0xbc, 0x64, 0x5a, 0x61, 0x02,
    0xfb, 0x45, 0xc2, 0xf1, 0xfd, 0xc7, 0x1f, 0x0f, 0xbc, 0xe5, 0x6a, 0xdf,
    0x3c, 0xed, 0x90, 0xd5, 0x9c, 0xa2, 0x35, 0xd4, 0xea, 0x76, 0x61, 0x06,
    0xba, 0x3c, 0x54, 0x6f, 0x70, 0x01, 0x29, 0x3c, 0x1c, 0x9f, 0xac, 0x13,
    0x85, 0x40, 0xd5, 0x33, 0x87, 0xe3, 0x93, 0xfd, 0xb5, 0xbd, 0x11, 0x58,
    0xfc, 0xac, 0x7a, 0x3e, 0xd0, 0xaa, 0x51, 0xbd, 0x58, 0x58, 0xab, 0x54,
    0xfb, 0xff, 0x56, 0xca, 0xaf, 0x19, 0xcd, 0xdf, 0x80, 0xee, 0x9f, 0x2d,
    0x6f, 0x05, 0xbe, 0xa5, 0xc5, 0x34, 0xcf, 0xaa, 0x48, 0xbf, 0xe1, 0x60,
    0x2b, 0x4a, 0xbf, 0x95, 0x88, 0x96, 0x74, 0xcc, 0x7e, 0x43, 0xa3, 0x4d,
    0xfc, 0x8f, 0xcb, 0xd6, 0xad, 0xc7, 0x0d, 0xaa, 0x6a, 0x76, 0x44, 0xb3,
    0x3a, 0xf9, 0xd3, 0xbc, 0x11, 0x83, 0xd7, 0x66, 0xb3, 0x54, 0x91, 0xd3,
    0x8f, 0xea, 0x09, 0xd8, 0x16, 0x5b, 0x5b, 0xb9, 0xe6, 0x23, 0x45, 0xc0,
    0x9f, 0x2d, 0xc0, 0xdd, 0xc9, 0x70, 0x98, 0x48, 0x7b, 0xbe, 0x09, 0xc9,
    0x06, 0x58, 0xd3, 0x44, 0xdd, 0xf3, 0x81, 0xef, 0xe5, 0x15, 0x03, 0x83,
    0x99, 0x08, 0x51, 0x78, 0x44, 0x40, 0xf8, 0xad, 0xe3, 0xc5, 0x89, 0x8a,
    0xe3, 0xa8, 0xe6, 0xa7, 0xde, 0x08, 0xb9, 0x27, 0x0c, 0x88, 0x5a, 0x9d,
    0x1a, 0x83, 0x64, 0x42, 0x7f, 0xa8, 0x27, 0xe2, 0xb5, 0xf9, 0x19, 0x44,
    0x8c, 0x0c, 0x6f, 0xab, 0xe0, 0xe3, 0x39, 0x59, 0x35, 0x63, 0xf4, 0x07,
    0xdb, 0x79, 0x03, 0xcb, 0x61, 0xff, 0x7e, 0xc8, 0x52, 0x3e, 0x6f, 0xbf,
    0x12, 0x98, 0x5e, 0x96, 0x9c, 0x91, 0x81, 0xa3, 0x8f, 0xd8, 0x9c, 0x0d,
    0x47, 0x2d, 0x68, 0xb2, 0xa4, 0xe7, 0xa6, 0xe5, 0xd9, 0xb8, 0x72, 0x80,
    0xb7, 0x6e, 0x7d, 0xef, 0x51, 0x90, 0x04, 0x0f, 0x82, 0x24, 0x94, 0xe4,
    0x6b, 0xce, 0x19, 0x5f, 0x54, 0xef, 0x84, 0x00, 0x92, 0x56, 0x08, 0x97,
    0xc6, 0x0b, 0x8a, 0x4f, 0xfd, 0xea, 0x4b, 0x21, 0xb6, 0x59, 0x7f, 0xfb,
    0x4c, 0x3d, 0xc4, 0x61, 0x53, 0xea, 0x2d, 0x31, 0x03, 0x5a, 0x6f, 0x03,
    0xec, 0x9b, 0x8b, 0x69, 0x83, 0x00, 0x5a, 0x3d, 0x8c, 0xc1, 0x67, 0xbf,
    0x8a, 0x21, 0x60, 0x47, 0xc8, 0xb7, 0xe6, 0x5c, 0x8a, 0x85, 0x77, 0x0d,
    0xc1, 0x8a, 0x42, 0xa0, 0x47, 0x81, 0x55, 0xee, 0x79, 0x98, 0x09, 0x7f,
    0xd7, 0xae, 0x12, 0x55, 0x0c, 0x01, 0xbb, 0xb5, 0xfa, 0x93, 0x6a, 0xed,
    0x90, 0x58, 0x63, 0xb0, 0x5f, 0x6b, 0xbe, 0x92, 0x66, 0x26, 0x6a, 0x1d,
    0xb7, 0x66, 0x7c, 0x06, 0x7c, 0x31, 0xed, 0x56, 0x85, 0x2c, 0x50, 0x3b,
    0x6a, 0x3f, 0x06, 0xbd, 0x0d, 0xd4, 0xca, 0x1b, 0xf1, 0xaf, 0x85, 0x35,
    0xe4, 0x0f, 0x41, 0x64, 0x6a, 0xa0, 0xad, 0xbc, 0x5a, 0xdf, 0x03, 0x6e,
    0x6d, 0x66, 0xcb, 0xe2, 0x34, 0x2d, 0x7e, 0x42, 0xc7, 0x1c, 0x68, 0xf2,
    0x09, 0x54, 0x07, 0xe1, 0x71, 0xc6, 0x7f, 0xc2, 0x48, 0x85, 0x26, 0x28,
    0xcf, 0x2e, 0xbe, 0x02, 0x83, 0xc6, 0x8d, 0xff, 0xc4, 0x3a, 0x3f, 0xb0,
    0x2f, 0xf2, 0x0d, 0x6f, 0x4a, 0x16, 0xab, 0xfe, 0x7e, 0x43, 0x05, 0x21,
    0x65, 0xe0, 0x83, 0x7b, 0x99, 0x3f, 0x74, 0xca, 0x55, 0x97, 0xe0, 0x26,
    0x6f, 0x01, 0x41, 0x24, 0x1a, 0x20, 0xd0, 0x8a, 0x51, 0xb6, 0x8d, 0x35,
    0xd4, 0x61, 0xca, 0xac, 0x96, 0x75, 0xb4, 0x32, 0x0b, 0x52, 0x6a, 0x73,
    0x79, 0xa1, 0x58, 0x41, 0x00, 0x03, 0xfb, 0x4d, 0x93, 0x6c, 0xfb, 0x83,
    0x26, 0xf1, 0x32, 0x82, 0x37, 0xf7, 0xcc, 0x03, 0xde, 0xc4, 0x7d, 0xf5,
    0xe3, 0xcb, 0x33, 0x6f, 0xd7, 0x9a, 0x02, 0xd4, 0xb1, 0x48, 0x49, 0x6c,
    0x58, 0x44, 0x4e, 0xcf, 0xae, 0x0e, 0xef, 0x59, 0x5b, 0x0d, 0x01, 0x8d,
    0x77, 0xc8, 0xcc, 0x03, 0x64, 0xd5, 0x63, 0x63, 0x2f, 0x8c, 0x2e, 0xb5,
    0x19, 0xac, 0x62, 0xfb, 0x8b, 0x63, 0x6d, 0x38, 0x52, 0x2c, 0x26, 0x72,
    0xd0, 0x04, 0x37, 0x9c, 0x7c, 0xfc, 0x78, 0xfa, 0xca, 0xda, 0xbd, 0x2a,
    0xfc, 0x42, 0x9b, 0xbb, 0x6d, 0x57, 0xce, 0xda, 0x5c, 0x64, 0x3d, 0x46,
    0x43, 0x8b, 0x8d, 0xac, 0x01, 0xc1, 0x67, 0x1d, 0x01, 0x20, 0x06, 0x46,
    0x11, 0x39, 0xc6, 0x2d, 0xbd, 0x6e, 0xe5, 0xd9, 0xad, 0xe8, 0x16, 0x5b,
    0xb9, 0xf6, 0x56, 0x2b, 0xef, 0x54, 0xcc, 0x8b, 0x7a, 0x13, 0x26, 0x10,
    0xc6, 0xc3, 0x9a, 0xf8, 0x9d, 0x0a, 0x8f, 0x51, 0x6f, 0xc2, 0xc4, 0xcc,
    0x78, 0x58, 0x13, 0xaf, 0xde, 0x7d, 0x24, 0x82, 0xe6, 0x57, 0x34, 0xaf,
    0xb7, 0x02, 0x51, 0x36, 0x1e, 0xd6, 0x02, 0xba, 0x96, 0xcf, 0xc3, 0x88,
    0x6e, 0x5d, 0x7b, 0x3b, 0xaa, 0xc5, 0xfd, 0x4b, 0x6f, 0xee, 0x51, 0x5b,
    0xba, 0xf8, 0xfe, 0xfd, 0xfb, 0x4f, 0xc4, 0x04, 0xd1, 0xb0, 0x4f, 0xc6,
    0xb6, 0xc0, 0x1a, 0xf7, 0xe0, 0x12, 0xf2, 0xe2, 0x6c, 0xd7, 0xe9, 0xa8,
    0x30, 0xd4, 0x2b, 0x14, 0x12, 0x57, 0x71, 0xc3, 0xe0, 0x50, 0x91, 0x5f,
    0x7e, 0xfe, 0x0f, 0xb2, 0xe6, 0x59, 0xb2, 0x5e, 0x90, 0x37, 0x2f, 0xde,
    0xa9, 0x27, 0x96, 0xbf, 0x78, 0x7a, 0xca, 0x97, 0x4b, 0x2a, 0x44, 0xac,
    0x34, 0xf6, 0xc8, 0xc1, 0xa8, 0x11, 0x99, 0x19, 0x60, 0x0f, 0x3c, 0x45,
    0xc4, 0xa3, 0x4e, 0x73, 0x39, 0x80, 0x79, 0x98, 0x24, 0xa0, 0x39, 0x42,
    0x9a, 0x3e, 0xcb, 0xd5, 0xdb, 0x35, 0xb8, 0x24, 0x47, 0xbf, 0xfc, 0xfc,
    0xef, 0xbd, 0x43, 0x90, 0xcf, 0xa6, 0x3c, 0x16, 0x01, 0x29, 0x5b, 0x7d,
    0x71, 0x36, 0x80, 0xd2, 0x04, 0xa2, 0x1c, 0x90, 0x13, 0x78, 0xc9, 0xda,
    0xc8, 0xfe, 0x3a, 0x5a, 0xce, 0xd7, 0xf9, 0x33, 0xfe, 0x77, 0xb2, 0x8f,
    0x99, 0x58, 0xb5, 0x31, 0x71, 0xd5, 0x42, 0x35, 0x69, 0xdf, 0x84, 0x7c,
    0x02, 0x11, 0x11, 0x01, 0xe4, 0xb8, 0x96, 0x34, 0xf7, 0x91, 0x97, 0x5b,
    0x0b, 0x4a, 0x04, 0xcd, 0xc2, 0x1c, 0xec, 0x1f, 0xb3, 0xf4, 0x9a, 0xe6,
    0x43, 0xe4, 0xf6, 0x40, 0x6d, 0x07, 0x72, 0xb9, 0x2b, 0x16, 0x53, 0x01,
    0xd3, 0x3b, 0x4b, 0x5f, 0x07, 0xe4, 0x3d, 0x70, 0x7e, 0x6a, 0x54, 0x1a,
    0x43, 0x9a, 0x5b, 0xec, 0x61, 0xd0, 0xdb, 0xd7, 0x43, 0x22, 0xd7, 0x4c,
    0x2e, 0x19, 0x27, 0xbd, 0x23, 0xa0, 0xde, 0x6b, 0x49, 0x35, 0x93, 0xd8,
    0x5a, 0xca, 0xd3, 0x39, 0x51, 0x58, 0x95, 0xcc, 0x61, 0xb1, 0x7d, 0x1c,
    0xcc, 0x2b, 0x94, 0xe2, 0x90, 0xd3, 0xb3, 0x80, 0x7c, 0xc6, 0x48, 0xc1,
    0x6b, 0xce, 0x7e, 0x5a, 0x53, 0xfb, 0x3e, 0x4e, 0x14, 0xe1, 0xf8, 0xf1,
    0xd3, 0xa7, 0x33, 0x92, 0x72, 0x8c, 0xee, 0x96, 0xd5, 0x59, 0xe3, 0x5a,
    0xc4, 0x9e, 0xd6, 0x75, 0xaf, 0x16, 0xa2, 0xc7, 0x8a, 0xcc, 0xf3, 0x25,
    0x02, 0xae, 0x22, 0x96, 0xfc, 0x7a, 0xfa, 0x8d, 0xcc, 0x6e, 0x58, 0x89,
    0x78, 0xfa, 0x63, 0x1d, 0xc2, 0x43, 0x8d, 0x41, 0x1f, 0x92, 0xfb, 0x88,
    0xf9, 0x56, 0x71, 0x9e, 0x0e, 0xf5, 0x32, 0x7e, 0x8d, 0x7f, 0x49, 0x4e,
    0xe3, 0x10, 0xa3, 0x7b, 0xa9, 0xe1, 0x56, 0x93, 0xaa, 0x71, 0x84, 0x96,
    0x1e, 0xde, 0x19, 0x9f, 0xa2, 0x96, 0xd6, 0x54, 0xb0, 0xb0, 0x89, 0x8a,
    0x74, 0xa2, 0x4f, 0x3f, 0x54, 0x71, 0x40, 0x14, 0x41, 0x33, 0x39, 0x6a,
    0x87, 0x85, 0x69, 0xdf, 0xca, 0xb3, 0x86, 0x6c, 0x4f, 0xcd, 0x56, 0x90,
    0x74, 0xc5, 0x64, 0x89, 0x72, 0x44, 0x40, 0x76, 0x1d, 0x0b, 0x64, 0x71,
    0xcd, 0xba, 0xf8, 0xa4, 0x87, 0xb9, 0xcf, 0x31, 0x75, 0x1e, 0x42, 0xb0,
    0xbe, 0x5b, 0xd8, 0x39, 0x2a, 0x31, 0xea, 0x5f, 0xbf, 0x5b, 0x56, 0x93,
    0x29, 0x51, 0xf2, 0x2e, 0x05, 0x18, 0xdb, 0x37, 0x33, 0x66, 0xe1, 0x82,
    0xa7, 0xe0, 0xb1, 0xfe, 0x9b, 0xdc, 0x2b, 0x3f, 0xae, 0x33, 0x58, 0x2b,
    0xb3, 0xb1, 0xaf, 0xaa, 0xd6, 0xed, 0x0d, 0xb5, 0x76, 0xcd, 0x8e, 0xbc,
    0x32, 0xfe, 0xa0, 0x3e, 0x48, 0x92, 0x2e, 0x9a, 0x57, 0xb8, 0x0c, 0xae,
    0x5f, 0xf1, 0x08, 0xc3, 0xac, 0x8c, 0xab, 0x78, 0x27, 0x26, 0xec, 0xcb,
    0xc9, 0x7e, 0x96, 0x53, 0x7b, 0x8e, 0xfb, 0x4a, 0x1f, 0x61, 0x2e, 0x42,
    0xa8, 0xba, 0x37, 0x6f, 0x10, 0xd6, 0xb5, 0x53, 0xb5, 0x5b, 0xa6, 0x81,
    0x38, 0xa5, 0xe5, 0xda, 0x87, 0xb0, 0x0f, 0xe3, 0xff, 0x0f, 0x45, 0xe7,
    0x51, 0xda, 0x22, 0x9f, 0x00, 0x00
};
static const size_t WEB_INDEX_GZIP_LENGTH = sizeof(WEB_INDEX_GZIP);

// =============================================================================
// Module: src/platform/Security.h
// =============================================================================
#include <mbedtls/sha256.h>



namespace advatek {

inline void sha256Hex(const uint8_t *data, size_t length, char output[65]) {
  uint8_t digest[32];
  mbedtls_sha256(data, length, digest, 0);
  for (uint8_t i = 0; i < 32; ++i) snprintf(output + i * 2, 3, "%02x", digest[i]);
  output[64] = '\0';
}

inline String sha256Base64Url(const char *value) {
  static constexpr char alphabet[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  uint8_t digest[32];
  mbedtls_sha256(
      reinterpret_cast<const uint8_t *>(value ? value : ""),
      value ? strlen(value) : 0,
      digest,
      0);
  String encoded;
  encoded.reserve(43);
  uint32_t buffer = 0;
  uint8_t bits = 0;
  for (uint8_t byte : digest) {
    buffer = (buffer << 8U) | byte;
    bits += 8;
    while (bits >= 6) {
      bits -= 6;
      encoded += alphabet[(buffer >> bits) & 0x3F];
    }
  }
  if (bits) encoded += alphabet[(buffer << (6 - bits)) & 0x3F];
  return encoded;
}

inline void setWebPassword(SecurityConfig &security, const char *password) {
  if (!password || !*password) {
    memset(&security, 0, sizeof(security));
    return;
  }
  uint32_t randomA = esp_random();
  uint32_t randomB = esp_random();
  snprintf(
      security.webPasswordSalt,
      sizeof(security.webPasswordSalt),
      "%08lx%08lx",
      static_cast<unsigned long>(randomA),
      static_cast<unsigned long>(randomB));
  String salted = String(security.webPasswordSalt) + password;
  sha256Hex(
      reinterpret_cast<const uint8_t *>(salted.c_str()),
      salted.length(),
      security.webPasswordHash);
}

inline bool verifyWebPassword(const SecurityConfig &security, const char *password) {
  if (!security.webPasswordHash[0]) return true;
  String salted = String(security.webPasswordSalt) + (password ? password : "");
  char hash[65];
  sha256Hex(reinterpret_cast<const uint8_t *>(salted.c_str()), salted.length(), hash);
  uint8_t difference = 0;
  for (uint8_t i = 0; i < 64; ++i) difference |= hash[i] ^ security.webPasswordHash[i];
  return difference == 0;
}

}  // namespace advatek

// =============================================================================
// Module: src/platform/MemoryResources.h
// =============================================================================
#include <Arduino.h>
#include <esp_heap_caps.h>




namespace advatek {

inline constexpr size_t CONFIG_REQUEST_LIMIT = 12U * 1024U;
inline constexpr size_t PIXLITE_RESPONSE_LIMIT = 32U * 1024U;
inline constexpr size_t CONFIG_TOKEN_CAPACITY = 768;
inline constexpr uint8_t DIAGNOSTIC_CAPACITY = 48;

// Large, long-lived collections live in external PSRAM. If any required
// allocation fails we release the partial set rather than consuming internal
// networking heap as an invisible fallback.
class MemoryResources {
 public:
  bool begin() {
    if (!psramFound()) return false;
    devices = allocate<DiscoveredDevice>(MAX_DISCOVERED_DEVICES);
    // Each managed PixLite owns a bounded media slice and status record.
    // Keeping the 16-device cache in PSRAM avoids consuming networking SRAM.
    media = allocate<MediaFile>(MAX_PIXLITES * MAX_MEDIA_FILES);
    mediaCounts = allocate<uint8_t>(MAX_PIXLITES);
    pixliteStatuses = allocate<PixLiteStatus>(MAX_PIXLITES);
    logs = allocate<LogEntry>(DIAGNOSTIC_CAPACITY);
    tokens = allocate<JsonToken>(CONFIG_TOKEN_CAPACITY);
    pixliteResponse = static_cast<char *>(heap_caps_calloc(
        PIXLITE_RESPONSE_LIMIT + 1,
        1,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    ready = selectMemoryMode(
                true,
                devices && media && mediaCounts && pixliteStatuses &&
                    logs && tokens && pixliteResponse) ==
            MemoryMode::Normal;
    if (!ready) release();
    return ready;
  }

  ~MemoryResources() { release(); }

  bool ready = false;
  DiscoveredDevice *devices = nullptr;
  MediaFile *media = nullptr;
  uint8_t *mediaCounts = nullptr;
  PixLiteStatus *pixliteStatuses = nullptr;
  LogEntry *logs = nullptr;
  JsonToken *tokens = nullptr;
  char *pixliteResponse = nullptr;

 private:
  template <typename T>
  static T *allocate(size_t count) {
    return static_cast<T *>(
        heap_caps_calloc(count, sizeof(T), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  }

  void release() {
    heap_caps_free(devices);
    heap_caps_free(media);
    heap_caps_free(mediaCounts);
    heap_caps_free(pixliteStatuses);
    heap_caps_free(logs);
    heap_caps_free(tokens);
    heap_caps_free(pixliteResponse);
    devices = nullptr;
    media = nullptr;
    mediaCounts = nullptr;
    pixliteStatuses = nullptr;
    logs = nullptr;
    tokens = nullptr;
    pixliteResponse = nullptr;
    ready = false;
  }
};

}  // namespace advatek

// =============================================================================
// Module: src/platform/Diagnostics.h
// =============================================================================
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

// =============================================================================
// Module: src/platform/ConfigStore.h
// =============================================================================
#include <Preferences.h>
#include <stdlib.h>







namespace advatek {

class ConfigStore {
 public:
  bool begin() { return preferences_.begin("adv-trigger", false); }

  bool load(AppConfig &config, const BoardProfile &board, const char *hostname) {
    AppConfig *a = static_cast<AppConfig *>(calloc(1, sizeof(AppConfig)));
    AppConfig *b = static_cast<AppConfig *>(calloc(1, sizeof(AppConfig)));
    if (!a || !b) {
      free(a);
      free(b);
      setDefaultConfig(config, board, hostname);
      return false;
    }
    bool migratedA = false;
    bool migratedB = false;
    const bool validA = readSlot("configA", *a, board, migratedA);
    const bool validB = readSlot("configB", *b, board, migratedB);
    if (!validA && !validB) {
      free(a);
      free(b);
      setDefaultConfig(config, board, hostname);
      return false;
    }
    const bool selectA = validA && (!validB || a->sequence >= b->sequence);
    config = selectA ? *a : *b;
    const bool migrated = selectA ? migratedA : migratedB;
    free(a);
    free(b);
    if (migrated) save(config);
    return true;
  }

  bool save(AppConfig &config) {
    const uint32_t previousSequence = config.sequence;
    config.magic = MAGIC;
    config.schemaVersion = CONFIG_SCHEMA_VERSION;
    ++config.sequence;
    config.crc32 = 0;
    config.crc32 = crc32Bytes(reinterpret_cast<const uint8_t *>(&config), sizeof(config));
    const char *slot = (config.sequence & 1U) ? "configA" : "configB";
    if (preferences_.putBytes(slot, &config, sizeof(config)) == sizeof(config)) {
      return true;
    }
    // Repeated bench upgrades can leave insufficient contiguous NVS space for
    // the next large blob. This is the inactive slot, so removing and retrying
    // it preserves the last CRC-valid record throughout the operation.
    preferences_.remove(slot);
    if (preferences_.putBytes(slot, &config, sizeof(config)) == sizeof(config)) {
      return true;
    }
    // Keep the parity pointing at the last valid slot. A later save will retry
    // the same inactive slot instead of risking the only known-good record.
    config.sequence = previousSequence;
    config.crc32 = 0;
    return false;
  }

  void clearAll() { preferences_.clear(); }

  void clearAuthentication(AppConfig &config) {
    memset(config.network.accessPointPassword, 0, sizeof(config.network.accessPointPassword));
    memset(config.security.webPasswordSalt, 0, sizeof(config.security.webPasswordSalt));
    memset(config.security.webPasswordHash, 0, sizeof(config.security.webPasswordHash));
    save(config);
  }

 private:
  static constexpr uint32_t MAGIC = 0x41544354UL;

  bool readSlot(
      const char *key,
      AppConfig &config,
      const BoardProfile &board,
      bool &migrated) {
    const size_t storedLength = preferences_.getBytesLength(key);
    size_t maximumLength = sizeof(AppConfig);
    if (sizeof(AppConfigV1) > maximumLength) maximumLength = sizeof(AppConfigV1);
    if (sizeof(AppConfigV2) > maximumLength) maximumLength = sizeof(AppConfigV2);
    if (sizeof(AppConfigV3) > maximumLength) maximumLength = sizeof(AppConfigV3);
    if (sizeof(AppConfigV4) > maximumLength) maximumLength = sizeof(AppConfigV4);
    if (storedLength < offsetof(AppConfig, hardware) || storedLength > maximumLength) return false;
    uint8_t *raw = static_cast<uint8_t *>(calloc(1, storedLength));
    if (!raw) return false;
    if (preferences_.getBytes(key, raw, storedLength) != storedLength) {
      free(raw);
      return false;
    }
    const AppConfig *header = reinterpret_cast<const AppConfig *>(raw);
    if (header->magic != MAGIC) {
      free(raw);
      return false;
    }
    const uint32_t expected = header->crc32;
    reinterpret_cast<AppConfig *>(raw)->crc32 = 0;
    const bool crcValid = expected == crc32Bytes(raw, storedLength);
    bool decoded = false;
    if (crcValid) {
      decoded = migrateStoredConfig(
          raw,
          storedLength,
          config,
          board.id,
          board.profileVersion,
          board.allowedInputPins,
          board.allowedInputPinCount,
          migrated);
    }
    free(raw);
    return decoded;
  }

  Preferences preferences_;
};

}  // namespace advatek

// =============================================================================
// Module: src/platform/ActionMailbox.h
// =============================================================================
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

// =============================================================================
// Module: src/platform/NetworkManager.h
// =============================================================================
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ETH.h>
#include <WiFi.h>
#include <apps/dhcpserver/dhcpserver.h>
#include <esp_netif_net_stack.h>
#include <lwip/netif.h>







namespace advatek {

class NetworkManager {
 public:
  NetworkManager(const BoardProfile &board, AppConfig &config, Diagnostics &log)
      : board_(board), config_(config), log_(log) {}

  void begin(const char *apName) {
    copyText(apName_, sizeof(apName_), apName);
    if (config_.network.uplink == UplinkMode::Ethernet) {
      WiFi.mode(WIFI_MODE_NULL);
      beginEthernet();
    } else {
      WiFi.mode(WIFI_MODE_STA);
      beginWifiStation();
    }
  }

  void loop() {
    if (apRunning_ || ethernetRecoveryRunning_) {
      dns_.processNextRequest();
      if (apStopAt_ && dueAt(millis(), apStopAt_)) stopAccessPoint();
      if (ethernetRecoveryStopAt_ &&
          dueAt(millis(), ethernetRecoveryStopAt_)) {
        stopEthernetRecovery();
      }
    }
    const bool nowOnline = uplinkConnected();
    if (nowOnline != wasOnline_) {
      wasOnline_ = nowOnline;
      if (nowOnline) {
        activeInterface().setDefault();
        startMdns();
        log_.addf(LogLevel::Info, "Operational uplink connected at %s", localIP().toString().c_str());
      } else {
        log_.add(LogLevel::Warning, "Operational uplink disconnected");
      }
    }
  }

  bool uplinkConnected() const {
    return config_.network.uplink == UplinkMode::Ethernet
               ? ETH.linkUp() && ETH.localIP() != IPAddress()
               : WiFi.STA.status() == WL_CONNECTED;
  }

  NetworkInterface &activeInterface() {
    return config_.network.uplink == UplinkMode::Ethernet
               ? static_cast<NetworkInterface &>(ETH)
               : static_cast<NetworkInterface &>(WiFi.STA);
  }

  IPAddress localIP() const {
    return config_.network.uplink == UplinkMode::Ethernet ? ETH.localIP() : WiFi.STA.localIP();
  }

  IPAddress netmask() const {
    return config_.network.uplink == UplinkMode::Ethernet
               ? ETH.subnetMask()
               : WiFi.STA.subnetMask();
  }

  IPAddress broadcastIP() const {
    const uint32_t ip = static_cast<uint32_t>(localIP());
    const uint32_t mask = static_cast<uint32_t>(netmask());
    return IPAddress(ip | ~mask);
  }

  bool accessPointRunning() const { return apRunning_; }
  bool ethernetRecoveryRunning() const { return ethernetRecoveryRunning_; }
  const char *accessPointName() const { return apName_; }
  bool ethernetInitialized() const { return ethernetInitialized_; }
  uint8_t ethernetInitAttempts() const { return ethernetInitAttempts_; }
  const char *ethernetFailureReason() const { return ethernetFailureReason_; }
  bool ethernetLinkUp() const { return ethernetInitialized_ && ETH.linkUp(); }
  uint32_t ethernetLinkSpeedMbps() const {
    return ethernetInitialized_ && ETH.linkUp() ? ETH.linkSpeed() : 0;
  }
  bool ethernetFullDuplex() const {
    return ethernetInitialized_ && ETH.linkUp() && ETH.fullDuplex();
  }

  bool openRecoveryNetwork() {
    config_.network.accessPointPassword[0] = '\0';
    if (usesEthernetRecovery(config_.network)) {
      return beginEthernetRecovery();
    }
    beginAccessPoint(900000UL);
    log_.add(LogLevel::Warning, "Open recovery access point enabled for 15 minutes");
    return true;
  }

 private:
  void applyStatic(NetworkInterface &interface, const IPv4Config &settings) {
    if (settings.mode != IpMode::Static) return;
    IPAddress ip, gateway, mask, dns;
    if (parseIPv4(settings.address, ip) && parseIPv4(settings.gateway, gateway) &&
        parseIPv4(settings.netmask, mask) && parseIPv4(settings.dns, dns)) {
      interface.config(ip, gateway, mask, dns);
    }
  }

  void beginEthernet() {
    log_.add(LogLevel::Info, "Starting explicit Ethernet uplink");
    ethernetInitialized_ = false;
    ethernetInitAttempts_ = 0;
    copyText(
        ethernetFailureReason_,
        sizeof(ethernetFailureReason_),
        "W5500 initialization did not complete");
    BoundedRetryState retry{3, 0, false};
    while (retry.canAttempt()) {
      const bool initialized = board_.beginEthernet(board_);
      retry.record(initialized);
      ethernetInitAttempts_ = retry.attempts;
      log_.addf(
          LogLevel::Info,
          "W5500 reset/initialization attempt %u of 3",
          retry.attempts);
      if (retry.succeeded) {
        ethernetInitialized_ = true;
        ethernetFailureReason_[0] = '\0';
        break;
      }
      if (retry.canAttempt()) delay(750);
    }
    if (!ethernetInitialized_) {
      log_.add(
          LogLevel::Error,
          "Ethernet initialization failed after 3 attempts; Wi-Fi fallback is disabled");
      return;
    }
    applyStatic(ETH, config_.network.ethernet);
    log_.addf(
        LogLevel::Info,
        "W5500 initialized for %s addressing",
        config_.network.ethernet.mode == IpMode::Static ? "static" : "DHCP");
  }

  void beginWifiStation() {
    log_.add(LogLevel::Info, "Starting explicit Wi-Fi station uplink");
    applyStatic(WiFi.STA, config_.network.wifi);
    WiFi.STA.setHostname(config_.network.hostname);
    WiFi.begin(config_.network.wifiSsid, config_.network.wifiPassword);
  }

  void beginAccessPoint(uint32_t lifetimeMs) {
    WiFi.mode(
        config_.network.uplink == UplinkMode::WifiStation
            ? WIFI_MODE_APSTA
            : WIFI_MODE_AP);
    WiFi.AP.begin();
    IPAddress ip(192, 168, 4, 1);
    WiFi.AP.config(ip, ip, IPAddress(255, 255, 255, 0));
    const char *password = config_.network.accessPointPassword;
    if (password[0] && strlen(password) >= 8) WiFi.AP.create(apName_, password);
    else WiFi.AP.create(apName_);
    dns_.start(53, "*", ip);
    startMdns();
    apRunning_ = true;
    apStopAt_ = lifetimeMs ? millis() + lifetimeMs : 0;
    log_.addf(LogLevel::Info, "Recovery access point %s started", apName_);
  }

  bool beginEthernetRecovery() {
    // Starting a DHCP server on an installed LAN could disrupt every device on
    // that segment. Require no physical link when BOOT recovery is released;
    // the installer plugs the computer in only after recovery mode is armed.
    if (!ethernetInitialized_) beginEthernet();
    if (!ethernetInitialized_) {
      log_.add(
          LogLevel::Error,
          "Direct Ethernet recovery unavailable because W5500 initialization failed");
      return false;
    }
    // A Wi-Fi-operated unit may not have initialized W5500 before recovery.
    // Give an already-connected PHY time to report link before making the
    // safety decision; a disconnected direct cable simply adds this delay.
    const uint32_t linkCheckUntil = millis() + 3000UL;
    while (!ETH.linkUp() && !dueAt(millis(), linkCheckUntil)) delay(25);
    if (!directEthernetRecoveryAllowed(ETH.linkUp())) {
      log_.add(
          LogLevel::Error,
          "Direct Ethernet recovery refused: disconnect the Ethernet cable before BOOT recovery");
      return false;
    }

    const IPAddress address(192, 168, 4, 1);
    const IPAddress mask(255, 255, 255, 0);
    if (!ETH.config(address, address, mask, address)) {
      log_.add(LogLevel::Error, "Direct Ethernet recovery could not set 192.168.4.1");
      return false;
    }
    struct netif *lwipNetif =
        static_cast<struct netif *>(esp_netif_get_netif_impl(ETH.netif()));
    ethernetDhcpServer_ = dhcps_new();
    ip4_addr_t serverAddress{};
    serverAddress.addr = static_cast<uint32_t>(address);
    if (!lwipNetif || !ethernetDhcpServer_ ||
        dhcps_start(ethernetDhcpServer_, lwipNetif, serverAddress) != ERR_OK) {
      if (ethernetDhcpServer_) {
        dhcps_delete(ethernetDhcpServer_);
        ethernetDhcpServer_ = nullptr;
      }
      log_.add(LogLevel::Error, "Direct Ethernet recovery DHCP server failed to start");
      return false;
    }

    ETH.setDefault();
    dns_.start(53, "*", address);
    ethernetRecoveryRunning_ = true;
    ethernetRecoveryStopAt_ = millis() + 900000UL;
    log_.add(
        LogLevel::Warning,
        "Direct Ethernet recovery armed at 192.168.4.1 for 15 minutes; connect one computer directly");
    return true;
  }

  void stopEthernetRecovery() {
    dns_.stop();
    if (ethernetDhcpServer_) {
      struct netif *lwipNetif =
          static_cast<struct netif *>(esp_netif_get_netif_impl(ETH.netif()));
      if (lwipNetif) dhcps_stop(ethernetDhcpServer_, lwipNetif);
      dhcps_delete(ethernetDhcpServer_);
      ethernetDhcpServer_ = nullptr;
    }
    ethernetRecoveryRunning_ = false;
    ethernetRecoveryStopAt_ = 0;
    log_.add(LogLevel::Info, "Direct Ethernet recovery expired; restarting normal uplink");
    delay(50);
    ESP.restart();
  }

  void stopAccessPoint() {
    dns_.stop();
    WiFi.AP.end();
    apRunning_ = false;
    apStopAt_ = 0;
    log_.add(LogLevel::Info, "Recovery access point stopped");
  }

  void startMdns() {
    if (!mdnsStarted_ && MDNS.begin(config_.network.hostname)) {
      MDNS.addService("http", "tcp", 80);
      mdnsStarted_ = true;
      log_.addf(
          LogLevel::Info,
          "Local address http://%s.local/",
          config_.network.hostname);
    } else if (!mdnsStarted_) {
      log_.addf(
          LogLevel::Warning,
          "Could not claim local hostname %s.local; use the displayed IP address",
          config_.network.hostname);
    }
  }

  const BoardProfile &board_;
  AppConfig &config_;
  Diagnostics &log_;
  DNSServer dns_;
  bool apRunning_ = false;
  bool ethernetRecoveryRunning_ = false;
  bool wasOnline_ = false;
  bool mdnsStarted_ = false;
  uint32_t apStopAt_ = 0;
  uint32_t ethernetRecoveryStopAt_ = 0;
  dhcps_t *ethernetDhcpServer_ = nullptr;
  char apName_[40]{};
  bool ethernetInitialized_ = false;
  uint8_t ethernetInitAttempts_ = 0;
  char ethernetFailureReason_[96]{};
};

}  // namespace advatek

// =============================================================================
// Module: src/platform/AdarDiscovery.h
// =============================================================================
#include <NetworkUdp.h>





namespace advatek {

class AdarDiscovery {
 public:
  AdarDiscovery(NetworkManager &network, Diagnostics &log, MemoryResources &memory)
      : network_(network), log_(log), memory_(memory) {}

  uint8_t count() const { return count_; }
  bool truncated() const { return truncated_; }
  const DiscoveredDevice &device(uint8_t index) const {
    return memory_.devices[index];
  }

  void discover() {
    count_ = 0;
    truncated_ = false;
    if (!memory_.devices) {
      log_.add(LogLevel::Error, "ADAR discovery disabled: PSRAM workspace unavailable");
      return;
    }
    if (!network_.uplinkConnected()) {
      log_.add(LogLevel::Warning, "Discovery skipped because the operational uplink is offline");
      return;
    }
    IPAddress listening;
    listening.fromString(ADAR_CLIENT_MULTICAST);
    if (!udp_.beginMulticast(listening, 0)) {
      log_.add(LogLevel::Error, "Could not open ADAR discovery socket");
      return;
    }
    runCycle(false, 3);
    runCycle(true, 2);
    udp_.stop();
    log_.addf(
        LogLevel::Info,
        "ADAR discovery completed with %u PixLite Mk3 device(s)%s",
        count_,
        truncated_ ? " (list truncated)" : "");
  }

  int16_t findByMac(const char *mac) const {
    for (uint8_t i = 0; i < count_; ++i) {
      if (strcasecmp(memory_.devices[i].macAddress, mac) == 0) return i;
    }
    return -1;
  }

 private:
  void runCycle(bool broadcast, uint8_t attempts) {
    for (uint8_t attempt = 0; attempt < attempts; ++attempt) {
      sendRequest(broadcast);
      const uint32_t deadline = millis() + 500;
      while (!dueAt(millis(), deadline)) {
        const int packetLength = udp_.parsePacket();
        if (packetLength > 0 && packetLength <= static_cast<int>(sizeof(packet_))) {
          const int read = udp_.read(packet_, packetLength);
          DiscoveredDevice candidate{};
          if (read == packetLength &&
              parseAdarDiscoveryResponse(packet_, read, broadcast, candidate)) {
            addOrUpdate(candidate);
          }
        }
        delay(2);
      }
    }
  }

  void sendRequest(bool broadcast) {
    char exclusions[50][13]{};
    const uint8_t excludedCount = count_ < 50 ? count_ : 50;
    for (uint8_t i = 0; i < excludedCount; ++i) {
      copyText(exclusions[i], sizeof(exclusions[i]), memory_.devices[i].macAddress);
    }
    const size_t length =
        buildAdarDiscoveryRequest(packet_, sizeof(packet_), exclusions, excludedCount);
    IPAddress destination;
    if (broadcast) destination = network_.broadcastIP();
    else destination.fromString(ADAR_DEVICE_MULTICAST);
    udp_.beginPacket(destination, ADAR_PORT);
    udp_.write(packet_, length);
    udp_.endPacket();
  }

  void addOrUpdate(const DiscoveredDevice &candidate) {
    const int16_t existing = findByMac(candidate.macAddress);
    if (existing >= 0) {
      memory_.devices[existing] = candidate;
      return;
    }
    if (count_ >= MAX_DISCOVERED_DEVICES) {
      truncated_ = true;
      return;
    }
    memory_.devices[count_++] = candidate;
  }

  NetworkManager &network_;
  Diagnostics &log_;
  MemoryResources &memory_;
  NetworkUDP udp_;
  uint8_t packet_[1024]{};
  uint8_t count_ = 0;
  bool truncated_ = false;
};

}  // namespace advatek

// =============================================================================
// Module: src/platform/PixLiteClient.h
// =============================================================================
#include <HTTPClient.h>











namespace advatek {

// HTTP remains deliberately serialized through one client and one reusable
// response workspace. A controller may manage 16 PixLites, but simultaneous
// 32 KB response allocations would needlessly fragment memory. Per-controller
// status and media caches live in PSRAM and actions identify their target.
class PixLiteClient {
 public:
  PixLiteClient(AppConfig &config, Diagnostics &log, MemoryResources &memory)
      : config_(config), log_(log), memory_(memory) {}

  bool connectAndLoad(uint8_t index) {
    if (!validIndex(index)) return targetFailed("PixLite target is not configured");
    PixLiteConfig &target = config_.pixlites[index];
    const int code = get(index, "/ver");
    if (code != 200) {
      setOffline(index, code, "PixLite /ver request failed");
      return false;
    }
    if (!strstr(response(), "PixLite") ||
        (!strstr(response(), "Mk3") && !strstr(response(), "Pix3"))) {
      setOffline(index, code, "Selected device did not identify as PixLite Mk3");
      return false;
    }
    const int minor = highestSupportedMinor(response());
    if (minor < 0) {
      setOffline(index, code, "PixLite does not advertise a supported v1 API");
      return false;
    }
    snprintf(target.apiVersion, sizeof(target.apiVersion), "v1.%d", minor);
    PixLiteStatus &current = status(index);
    current.online = true;
    current.lastError[0] = '\0';
    clientError_[0] = '\0';
    failureLogged_[index] = false;
    log_.addf(
        LogLevel::Info,
        "%s connected using PixLite API %s",
        target.displayName,
        target.apiVersion);
    return refreshMedia(index);
  }

  bool refreshMedia(uint8_t index) {
    if (!validIndex(index) || !memory_.media || !memory_.mediaCounts ||
        !memory_.tokens || !memory_.pixliteResponse) {
      return requestFailed(index, RESPONSE_MEMORY_UNAVAILABLE, "", "file list");
    }
    const int code = post(index, pixLiteFileListRequest(nextId()));
    if (!responseAccepted(index, code, "file list")) return false;
    uint8_t &count = memory_.mediaCounts[index];
    count = 0;
    JsonDocument document(response(), memory_.tokens, CONFIG_TOKEN_CAPACITY);
    if (!document.parse()) {
      return requestFailed(index, code, response(), "file list JSON");
    }
    MediaFile *targetMedia = mediaSlice(index);
    for (int16_t i = 0; i < document.count() && count < MAX_MEDIA_FILES; ++i) {
      if (document.token(i).type != JsonTokenType::String) continue;
      char name[64];
      if (!document.stringValue(i, name, sizeof(name))) continue;
      if (!mediaNameIsScene(name) && !mediaNameIsPlaylist(name)) continue;
      bool duplicate = false;
      for (uint8_t existing = 0; existing < count; ++existing) {
        duplicate |= strcmp(targetMedia[existing].name, name) == 0;
      }
      if (!duplicate) {
        copyText(targetMedia[count].name, sizeof(targetMedia[count].name), name);
        targetMedia[count].playlist = mediaNameIsPlaylist(name);
        ++count;
      }
    }
    // PixLite Mk3 presents media alphabetically. Cache the same deterministic
    // order so the SPA, action choices, and GPIO scene stepping all agree.
    sortMediaFiles(targetMedia, count);
    return true;
  }

  bool execute(const ActionConfig &action) {
    const int8_t index = pixLiteIndexById(config_, action.targetId);
    if (index < 0) return targetFailed("Action target is not configured");
    return execute(static_cast<uint8_t>(index), action);
  }

  bool execute(uint8_t index, const ActionConfig &action) {
    if (!validIndex(index)) return targetFailed("Action target is not configured");
    if (action.kind == ActionKind::None) return true;
    String request;
    char selectedScene[64]{};
    switch (action.kind) {
      case ActionKind::PlayScene:
        copyText(selectedScene, sizeof(selectedScene), action.mediaName);
        request = pixLitePlaybackRequest(action, nextId());
        break;
      case ActionKind::PlayPlaylist:
      case ActionKind::BlankOutputs:
        request = pixLitePlaybackRequest(action, nextId());
        break;
      case ActionKind::NextScene:
      case ActionKind::PreviousScene: {
        ActionConfig playback = action;
        const int16_t selected = sceneStepIndex(
            mediaSlice(index),
            mediaCount(index),
            sceneStepReference(
                status(index).currentFile,
                status(index).lastScene),
            action.kind == ActionKind::PreviousScene);
        if (selected < 0) {
          char message[128];
          snprintf(
              message,
              sizeof(message),
              "%s scene step ignored: no .scn files are cached",
              config_.pixlites[index].displayName);
          return targetFailed(message);
        }
        playback.kind = ActionKind::PlayScene;
        copyText(
            playback.mediaName,
            sizeof(playback.mediaName),
            media(index, static_cast<uint8_t>(selected)).name);
        copyText(selectedScene, sizeof(selectedScene), playback.mediaName);
        log_.addf(
            LogLevel::Info,
            "%s scene step selected %s",
            config_.pixlites[index].displayName,
            playback.mediaName);
        request = pixLitePlaybackRequest(playback, nextId());
        break;
      }
      case ActionKind::StopPlayback:
        request = pixLiteStopRequest(nextId());
        break;
      case ActionKind::TestColor:
      case ActionKind::TestColorFade:
        request = pixLiteTestColorRequest(action, nextId());
        break;
      case ActionKind::IntensityBrighter:
      case ActionKind::IntensityDarker:
      case ActionKind::SetIntensity:
      case ActionKind::ReleaseIntensityOverride:
        request = pixLiteIntensityRequest(
            action,
            nextId(),
            configuredAuxMask(index));
        break;
      default:
        return true;
    }
    const int code = post(index, request);
    if (!responseAccepted(index, code, "action")) return false;
    PixLiteStatus &current = status(index);
    current.online = true;
    current.httpStatus = code;
    current.lastError[0] = '\0';
    clientError_[0] = '\0';
    if (strstr(response(), "\"status\"")) parseStatus(index, response());
    if (selectedScene[0]) {
      copyText(current.lastScene, sizeof(current.lastScene), selectedScene);
    }
    return true;
  }

  bool pollStatus(uint8_t index) {
    if (!validIndex(index)) return false;
    const int code = post(index, pixLiteStatusRequest(nextId()));
    if (!responseAccepted(index, code, "status")) return false;
    PixLiteStatus &current = status(index);
    current.online = true;
    current.httpStatus = code;
    current.lastError[0] = '\0';
    parseStatus(index, response());
    return true;
  }

  uint8_t mediaCount(uint8_t index) const {
    return validIndex(index) && memory_.mediaCounts ? memory_.mediaCounts[index] : 0;
  }

  const MediaFile &media(uint8_t index, uint8_t item) const {
    return memory_.media[index * MAX_MEDIA_FILES + item];
  }

  PixLiteStatus &status(uint8_t index) {
    return memory_.pixliteStatuses && index < MAX_PIXLITES
               ? memory_.pixliteStatuses[index]
               : emptyStatus_;
  }

  const PixLiteStatus &status(uint8_t index) const {
    return memory_.pixliteStatuses && index < MAX_PIXLITES
               ? memory_.pixliteStatuses[index]
               : emptyStatus_;
  }

  const PixLiteStatus &primaryStatus() const {
    const uint8_t index =
        config_.primaryPixliteIndex < config_.pixliteCount
            ? config_.primaryPixliteIndex
            : 0;
    return status(index);
  }

  const PixLiteStatus &statusForAction(const ActionConfig &action) const {
    const int8_t index = pixLiteIndexById(config_, action.targetId);
    return index >= 0 ? status(static_cast<uint8_t>(index)) : emptyStatus_;
  }

  const char *lastError() const {
    return clientError_[0] ? clientError_ : "PixLite request failed";
  }

 private:
  // Keep client-side failures outside HTTPClient's negative error range. This
  // prevents a transport error such as SEND_HEADER_FAILED from being reported
  // as one of our bounded-buffer or degraded-memory conditions.
  static constexpr int RESPONSE_TOO_LARGE = -1002;
  static constexpr int RESPONSE_INCOMPLETE = -1003;
  static constexpr int RESPONSE_MEMORY_UNAVAILABLE = -1004;

  bool validIndex(uint8_t index) const {
    return index < config_.pixliteCount && index < MAX_PIXLITES &&
           config_.pixlites[index].enabled && config_.pixlites[index].host[0];
  }

  MediaFile *mediaSlice(uint8_t index) const {
    return memory_.media + index * MAX_MEDIA_FILES;
  }

  int highestSupportedMinor(const char *body) {
    int highest = -1;
    for (int minor = 9; minor >= 0; --minor) {
      char needle[8];
      snprintf(needle, sizeof(needle), "v1.%d", minor);
      if (strstr(body, needle)) {
        highest = minor;
        break;
      }
    }
    if (highest < 0 && strstr(body, "\"v1\"")) highest = 9;
    return highest;
  }

  String baseUrl(uint8_t index) const {
    const PixLiteConfig &target = config_.pixlites[index];
    return "http://" + String(target.host) + ":" + String(target.port);
  }

  String apiUrl(uint8_t index) const {
    const PixLiteConfig &target = config_.pixlites[index];
    String url = baseUrl(index) + "/" + target.apiVersion;
    if (target.username[0]) {
      url += "?user=" + String(target.username);
      if (target.password[0]) {
        url += "&auth=" + sha256Base64Url(target.password);
      }
    }
    return url;
  }

  int get(uint8_t index, const char *path) {
    if (!memory_.pixliteResponse) return RESPONSE_MEMORY_UNAVAILABLE;
    // A failed HTTP transaction may not call readResponse(). Clear the shared
    // workspace here so diagnostics can never quote a previous successful
    // PixLite Mk3 response as if it belonged to the current failure.
    memory_.pixliteResponse[0] = '\0';
    HTTPClient http;
    http.setConnectTimeout(1500);
    http.setTimeout(1800);
    if (!http.begin(baseUrl(index) + path)) return -1;
    const uint32_t started = millis();
    const int code = http.GET();
    const int result = code > 0 ? readResponse(http, code) : code;
    const uint32_t elapsed = millis() - started;
    status(index).latencyMs =
        static_cast<uint16_t>(elapsed < 65535 ? elapsed : 65535);
    http.end();
    return result;
  }

  int post(uint8_t index, const String &body) {
    if (!memory_.pixliteResponse) return RESPONSE_MEMORY_UNAVAILABLE;
    memory_.pixliteResponse[0] = '\0';
    HTTPClient http;
    http.setConnectTimeout(1500);
    http.setTimeout(1800);
    if (!http.begin(apiUrl(index))) return -1;
    http.addHeader("Content-Type", "application/json");
    const uint32_t started = millis();
    const int code = http.POST(body);
    const int result = code > 0 ? readResponse(http, code) : code;
    const uint32_t elapsed = millis() - started;
    status(index).latencyMs =
        static_cast<uint16_t>(elapsed < 65535 ? elapsed : 65535);
    http.end();
    return result;
  }

  int readResponse(HTTPClient &http, int code) {
    memory_.pixliteResponse[0] = '\0';
    const int declared = http.getSize();
    if (declared > static_cast<int>(PIXLITE_RESPONSE_LIMIT)) {
      return RESPONSE_TOO_LARGE;
    }
    NetworkClient *stream = http.getStreamPtr();
    size_t used = 0;
    uint32_t lastDataAt = millis();
    while (http.connected() &&
           (declared < 0 || used < static_cast<size_t>(declared))) {
      const size_t available = stream->available();
      if (available) {
        const size_t room = PIXLITE_RESPONSE_LIMIT - used;
        if (!room) return RESPONSE_TOO_LARGE;
        const size_t requested = available < room ? available : room;
        const int read = stream->readBytes(
            reinterpret_cast<uint8_t *>(memory_.pixliteResponse + used),
            requested);
        if (read > 0) {
          used += static_cast<size_t>(read);
          lastDataAt = millis();
        }
      } else {
        if (millis() - lastDataAt > 1800) break;
        delay(1);
      }
    }
    memory_.pixliteResponse[used] = '\0';
    if (declared >= 0 && used != static_cast<size_t>(declared)) {
      return RESPONSE_INCOMPLETE;
    }
    return code;
  }

  const char *response() const {
    return memory_.pixliteResponse ? memory_.pixliteResponse : "";
  }

  bool targetFailed(const char *message) {
    copyText(clientError_, sizeof(clientError_), message);
    log_.add(LogLevel::Warning, message);
    return false;
  }

  bool requestFailed(
      uint8_t index,
      int code,
      const char *responseBody,
      const char *operation) {
    PixLiteStatus &current = status(index);
    current.online = false;
    current.httpStatus = code > 0 ? code : 0;
    String error = String("PixLite ") + operation + " failed";
    if (code == RESPONSE_TOO_LARGE) {
      error += ": response exceeded 32 KB";
    } else if (code == RESPONSE_INCOMPLETE) {
      error += ": incomplete response";
    } else if (code == RESPONSE_MEMORY_UNAVAILABLE) {
      error += ": degraded memory mode";
    } else if (code < 0) {
      error += ": transport error ";
      error += String(code);
    } else if (code != 200) {
      error += ": HTTP ";
      error += String(code);
    }
    if (responseBody && responseBody[0]) {
      char excerpt[91];
      strlcpy(excerpt, responseBody, sizeof(excerpt));
      error += ": " + String(excerpt);
    }
    copyText(current.lastError, sizeof(current.lastError), error);
    copyText(clientError_, sizeof(clientError_), error.c_str());
    logFailure(index, current.lastError);
    return false;
  }

  bool responseAccepted(uint8_t index, int code, const char *operation) {
    if (code != 200) return requestFailed(index, code, response(), operation);
    if (!memory_.tokens) return requestFailed(index, -4, "", operation);

    PixLiteApiError apiError;
    if (!inspectPixLiteResponse(
            response(), memory_.tokens, CONFIG_TOKEN_CAPACITY, apiError)) {
      return requestFailed(index, code, "invalid JSON response", operation);
    }
    if (!apiError.present) {
      failureLogged_[index] = false;
      return true;
    }

    PixLiteStatus &current = status(index);
    current.online = true;
    current.httpStatus = code;
    String message = String("PixLite ") + operation + " rejected";
    if (apiError.code) message += " (API error " + String(apiError.code) + ")";
    if (apiError.message[0]) message += ": " + String(apiError.message);
    copyText(current.lastError, sizeof(current.lastError), message);
    copyText(clientError_, sizeof(clientError_), message.c_str());
    logFailure(index, current.lastError);
    return false;
  }

  void setOffline(uint8_t index, int code, const char *message) {
    PixLiteStatus &current = status(index);
    current.online = false;
    current.httpStatus = code > 0 ? code : 0;
    copyText(current.lastError, sizeof(current.lastError), message);
    copyText(clientError_, sizeof(clientError_), message);
    logFailure(index, message);
  }

  void logFailure(uint8_t index, const char *message) {
    if (index >= MAX_PIXLITES) return;
    const uint32_t now = millis();
    // A disconnected controller otherwise adds a warning after every
    // two-second status poll and quickly hides more useful diagnostics.
    if (!failureLogged_[index] ||
        now - failureLoggedAt_[index] >= MEDIA_RETRY_MS) {
      log_.add(LogLevel::Warning, message);
      failureLogged_[index] = true;
      failureLoggedAt_[index] = now;
    }
  }

  void parseStatus(uint8_t index, const char *responseBody) {
    if (!memory_.tokens) return;
    JsonDocument document(responseBody, memory_.tokens, CONFIG_TOKEN_CAPACITY);
    if (!document.parse()) return;
    PixLiteStatus &current = status(index);
    current.outputCount = 0;
    for (int16_t i = 0; i + 1 < document.count(); ++i) {
      if (document.equals(i, "mode") &&
          document.token(i + 1).type == JsonTokenType::String) {
        document.stringValue(i + 1, current.mode, sizeof(current.mode));
      } else if ((document.equals(i, "file") || document.equals(i, "name")) &&
                 document.token(i + 1).type == JsonTokenType::String) {
        char candidate[64];
        document.stringValue(i + 1, candidate, sizeof(candidate));
        if (mediaNameIsScene(candidate) || mediaNameIsPlaylist(candidate)) {
          copyText(current.currentFile, sizeof(current.currentFile), candidate);
          if (mediaNameIsScene(candidate)) {
            copyText(current.lastScene, sizeof(current.lastScene), candidate);
          }
        }
      } else if (document.equals(i, "out") &&
                 document.token(i + 1).type == JsonTokenType::String &&
                 current.outputCount < MAX_OUTPUTS) {
        OutputIntensity &output = current.outputs[current.outputCount];
        document.stringValue(i + 1, output.output, sizeof(output.output));
        if (strcmp(output.output, "Pix") != 0 &&
            strncmp(output.output, "Aux:", 4) != 0) {
          continue;
        }
        output.highPriority = false;
        output.programmedFactor = 255;
        const int16_t parent = document.token(i).parent;
        for (int16_t field = i + 2; field < document.count(); ++field) {
          if (document.token(field).parent != parent) continue;
          if (document.equals(field, "progPri")) {
            char priority[8];
            output.highPriority =
                document.stringValue(field + 1, priority, sizeof(priority))
                    ? programPriorityIsHigh(priority)
                    : document.booleanValue(field + 1, false);
          } else if (document.equals(field, "fc")) {
            const int16_t programmed = document.arrayItem(field + 1, 1);
            if (programmed >= 0) {
              output.programmedFactor =
                  constrain(document.integerValue(programmed, 255), 0, 255);
            }
          }
        }
        ++current.outputCount;
      }
    }
    if (strcasecmp(current.mode, "playback") != 0) {
      current.currentFile[0] = '\0';
    }
  }

  uint8_t configuredAuxMask(uint8_t index) const {
    uint8_t mask = 0;
    const PixLiteStatus &current = status(index);
    for (uint8_t i = 0; i < current.outputCount; ++i) {
      if (strncmp(current.outputs[i].output, "Aux:", 4) != 0) continue;
      const int aux = atoi(current.outputs[i].output + 4);
      if (aux >= 1 && aux <= 8) mask |= 1U << (aux - 1);
    }
    return mask;
  }

  uint32_t nextId() { return ++requestId_; }

  AppConfig &config_;
  Diagnostics &log_;
  MemoryResources &memory_;
  PixLiteStatus emptyStatus_{};
  char clientError_[128]{};
  bool failureLogged_[MAX_PIXLITES]{};
  uint32_t failureLoggedAt_[MAX_PIXLITES]{};
  uint32_t requestId_ = 0;
};

}  // namespace advatek

// =============================================================================
// Module: src/platform/WebApi.h
// =============================================================================
#include <WebServer.h>












namespace advatek {

class WebApiDelegate {
 public:
  virtual String stateJson() = 0;
  virtual String inputStateJson() = 0;
  virtual void configurationChanged() = 0;
  virtual void requestFactoryReset() = 0;
};

class WebApi {
 public:
  WebApi(
      AppConfig &config,
      const BoardProfile &board,
      ConfigStore &store,
      AdarDiscovery &discovery,
      PixLiteClient &pixlite,
      Diagnostics &log,
      MemoryResources &memory,
      WebApiDelegate &delegate)
      : config_(config),
        board_(board),
        store_(store),
        discovery_(discovery),
        pixlite_(pixlite),
        log_(log),
        memory_(memory),
        delegate_(delegate),
        server_(80) {}

  void begin() {
    server_.on("/", HTTP_GET, [this]() { serveUi(); });
    server_.on("/generate_204", HTTP_ANY, [this]() { serveUi(); });
    server_.on("/hotspot-detect.html", HTTP_ANY, [this]() { serveUi(); });
    server_.on("/api/state", HTTP_GET, [this]() {
      if (authorize()) sendJson(200, delegate_.stateJson());
    });
    server_.on("/api/inputs", HTTP_GET, [this]() {
      if (authorize()) sendJson(200, delegate_.inputStateJson());
    });
    server_.on("/api/config", HTTP_GET, [this]() {
      if (authorize()) streamConfig();
    });
    server_.on("/api/config/export", HTTP_GET, [this]() {
      if (!authorize()) return;
      server_.sendHeader(
          "Content-Disposition",
          "attachment; filename=\"advatek-trigger-backup.json\"");
      streamConfig();
    });
    server_.on("/api/devices", HTTP_GET, [this]() {
      if (authorize()) streamDevices();
    });
    server_.on("/api/media", HTTP_GET, [this]() {
      if (authorize()) streamMedia();
    });
    server_.on("/api/log", HTTP_GET, [this]() {
      if (authorize()) streamLog();
    });
    server_.on("/api/discover", HTTP_POST, [this]() {
      if (!authorize()) return;
      if (!memory_.ready) {
        sendError(503, "Discovery disabled in degraded memory mode");
        return;
      }
      discovery_.discover();
      bool addressChanged = false;
      for (uint8_t i = 0; i < config_.pixliteCount; ++i) {
        PixLiteConfig &target = config_.pixlites[i];
        const int16_t found = discovery_.findByMac(target.macAddress);
        if (found >= 0) {
          const DiscoveredDevice &device = discovery_.device(found);
          if (strcmp(target.host, device.ipAddress) != 0) {
            copyText(target.host, sizeof(target.host), device.ipAddress);
            addressChanged = true;
          }
          if (genericPixLiteName(target.displayName) && device.nickname[0]) {
            copyText(
                target.displayName,
                sizeof(target.displayName),
                device.nickname);
            addressChanged = true;
          }
        }
      }
      if (addressChanged) {
        store_.save(config_);
        log_.add(
            LogLevel::Info,
            "Updated saved PixLite addresses from their MAC identities");
      }
      streamDevices();
    });
    server_.on("/api/connect", HTTP_POST, [this]() {
      if (!authorize()) return;
      if (!memory_.ready) {
        sendError(503, "PixLite connection disabled in degraded memory mode");
        return;
      }
      applyConnectRequest();
    });
    server_.on("/api/pixlites/remove", HTTP_POST, [this]() {
      if (authorize()) removePixLite();
    });
    server_.on("/api/action/test", HTTP_POST, [this]() {
      if (!authorize()) return;
      if (!memory_.ready) {
        sendError(503, "PixLite actions disabled in degraded memory mode");
        return;
      }
      testAction();
    });
    server_.on("/api/config", HTTP_POST, [this]() {
      if (authorize()) applyConfiguration();
    });
    server_.on("/api/config/import", HTTP_POST, [this]() {
      if (authorize()) applyConfiguration();
    });
    server_.on("/api/network/apply", HTTP_POST, [this]() {
      if (!authorize()) return;
      applyConfiguration(true);
    });
    server_.on("/api/factory-reset", HTTP_POST, [this]() {
      if (!authorize()) return;
      if (server_.arg("plain").indexOf("FACTORY RESET") < 0) {
        sendError(400, "Confirmation phrase is required");
        return;
      }
      sendJson(202, "{\"restarting\":true}");
      delegate_.requestFactoryReset();
    });
    server_.onNotFound([this]() {
      if (server_.uri().startsWith("/api/")) sendError(404, "Unknown API endpoint");
      else serveUi();
    });
    server_.begin();
    log_.add(LogLevel::Info, "Local HTTP interface started (HTTP only; no TLS)");
  }

  void loop() { server_.handleClient(); }

 private:
  bool authorize() {
    if (!config_.security.webPasswordHash[0]) return true;
    if (verifyWebPassword(config_.security, server_.header("X-Advatek-Password").c_str())) return true;
    server_.sendHeader("Cache-Control", "no-store");
    sendError(401, "Password required");
    return false;
  }

  void serveUi() {
    server_.sendHeader("Content-Encoding", "gzip");
    server_.sendHeader("Cache-Control", "no-cache");
    server_.send_P(
        200,
        "text/html; charset=utf-8",
        reinterpret_cast<const char *>(WEB_INDEX_GZIP),
        WEB_INDEX_GZIP_LENGTH);
  }

  void sendJson(int code, const String &json) {
    server_.sendHeader("Cache-Control", "no-store");
    server_.send(code, "application/json", json);
  }

  void sendError(int code, const char *message) {
    sendJson(code, "{\"error\":\"" + jsonEscape(message) + "\"}");
  }

  void beginJsonStream(int code = 200) {
    server_.sendHeader("Cache-Control", "no-store");
    server_.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server_.send(code, "application/json", "");
  }

  void endJsonStream() { server_.sendContent(""); }

  void streamConfig() {
    beginJsonStream();
    String chunk;
    chunk.reserve(1200);
    chunk = "{\"schemaVersion\":" + String(config_.schemaVersion) +
            ",\"boardId\":\"" + jsonEscape(config_.hardware.boardId) +
            "\",\"hardwareBinding\":{\"boardId\":\"" +
            jsonEscape(config_.hardware.boardId) + "\",\"profileVersion\":\"" +
            jsonEscape(config_.hardware.profileVersion) + "\",\"inputGpios\":[";
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      if (i) chunk += ',';
      if (config_.hardware.inputGpios[i] == PIN_UNASSIGNED) chunk += "null";
      else chunk += String(config_.hardware.inputGpios[i]);
    }
    chunk += "]},\"boardCapabilities\":{\"displayName\":\"" +
             jsonEscape(board_.displayName) + "\",\"inputHardware\":\"";
    chunk += board_.inputHardwareKind == InputHardwareKind::IsolatedDigitalInput
                 ? "isolated"
                 : "direct";
    chunk += "\",\"allowedInputPins\":[";
    for (uint8_t i = 0; i < board_.allowedInputPinCount; ++i) {
      if (i) chunk += ',';
      chunk += String(board_.allowedInputPins[i]);
    }
    chunk += "],\"cameraSharedPins\":[";
    for (uint8_t i = 0; i < board_.cameraSharedPinCount; ++i) {
      if (i) chunk += ',';
      chunk += String(board_.cameraSharedPins[i]);
    }
    chunk += "]},\"statusLed\":{\"enabled\":";
    chunk += config_.statusLed.enabled ? "true" : "false";
    chunk += ",\"brightnessPercent\":" +
             String(config_.statusLed.brightnessPercent) +
             "},\"network\":{\"uplink\":\"";
    chunk += config_.network.uplink == UplinkMode::Ethernet ? "ethernet" : "wifi";
    chunk += "\",\"hostname\":\"" + jsonEscape(config_.network.hostname) +
             "\",\"wifiSsid\":\"" + jsonEscape(config_.network.wifiSsid) + "\",";
    appendIpChunk(chunk, "ethernet", config_.network.ethernet);
    appendIpChunk(chunk, "wifi", config_.network.wifi);
    chunk += "\"recoveryConnection\":\"";
    chunk += usesEthernetRecovery(config_.network) ? "ethernet" : "wifi";
    chunk += "\"},\"pixlites\":[";
    server_.sendContent(chunk);
    for (uint8_t i = 0; i < config_.pixliteCount && i < MAX_PIXLITES; ++i) {
      const PixLiteConfig &target = config_.pixlites[i];
      chunk = i ? "," : "";
      chunk += "{\"id\":\"" + jsonEscape(target.id) +
               "\",\"name\":\"" + jsonEscape(target.displayName) +
               "\",\"mac\":\"" + jsonEscape(target.macAddress) +
               "\",\"host\":\"" + jsonEscape(target.host) +
               "\",\"port\":" + String(target.port) +
               ",\"apiVersion\":\"" + jsonEscape(target.apiVersion) +
               "\",\"username\":\"" + jsonEscape(target.username) + "\"}";
      server_.sendContent(chunk);
    }
    chunk = "],\"primaryPixliteId\":\"";
    if (config_.pixliteCount) {
      const uint8_t primary =
          config_.primaryPixliteIndex < config_.pixliteCount
              ? config_.primaryPixliteIndex
              : 0;
      chunk += jsonEscape(config_.pixlites[primary].id);
    }
    chunk += "\",\"maxPixlites\":" + String(MAX_PIXLITES) + ",\"inputs\":[";
    server_.sendContent(chunk);
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      const InputConfig &input = config_.inputs[i];
      chunk = i ? "," : "";
      chunk += "{\"enabled\":" + String(input.enabled ? "true" : "false") +
               ",\"name\":\"" + jsonEscape(input.name) + "\",\"gpio\":";
      if (config_.hardware.inputGpios[i] == PIN_UNASSIGNED) chunk += "null";
      else chunk += String(config_.hardware.inputGpios[i]);
      chunk += ",\"normallyClosed\":" +
               String(input.normallyClosed ? "true" : "false") +
               ",\"mode\":\"" +
               String(input.mode == InputMode::Momentary ? "momentary" : "maintained") +
               "\",\"debounceMs\":" + String(input.debounceMs) + ",\"onAction\":";
      appendActionJson(chunk, input.onAction);
      chunk += ",\"offAction\":";
      appendActionJson(chunk, input.offAction);
      chunk += '}';
      server_.sendContent(chunk);
    }
    server_.sendContent("],\"secretsOmitted\":true}");
    endJsonStream();
  }

  static void appendIpChunk(String &chunk, const char *name, const IPv4Config &ip) {
    chunk += "\"" + String(name) + "\":{\"mode\":\"" +
             String(ip.mode == IpMode::Static ? "static" : "dhcp") +
             "\",\"address\":\"" + jsonEscape(ip.address) +
             "\",\"netmask\":\"" + jsonEscape(ip.netmask) +
             "\",\"gateway\":\"" + jsonEscape(ip.gateway) +
             "\",\"dns\":\"" + jsonEscape(ip.dns) + "\"},";
  }

  void streamDevices() {
    beginJsonStream();
    server_.sendContent(
        String("{\"truncated\":") +
        (discovery_.truncated() ? "true" : "false") +
        ",\"devices\":[");
    for (uint8_t i = 0; i < discovery_.count(); ++i) {
      const DiscoveredDevice &device = discovery_.device(i);
      const int8_t configured = pixLiteIndexByMac(config_, device.macAddress);
      String chunk = i ? "," : "";
      chunk += "{\"name\":\"" + jsonEscape(device.productName) +
               "\",\"nickname\":\"" + jsonEscape(device.nickname) +
               "\",\"mac\":\"" + jsonEscape(device.macAddress) +
               "\",\"ip\":\"" + jsonEscape(device.ipAddress) +
               "\",\"firmware\":\"" + jsonEscape(device.firmwareVersion) +
               "\",\"authRequired\":" +
               String(device.authRequired ? "true" : "false") +
               ",\"configuredId\":\"" +
               jsonEscape(configured >= 0 ? config_.pixlites[configured].id : "") +
               "\",\"via\":\"" +
               String(device.viaBroadcast ? "broadcast" : "multicast") + "\"}";
      server_.sendContent(chunk);
    }
    server_.sendContent("]}");
    endJsonStream();
  }

  void streamMedia() {
    const String requestedId = server_.arg("targetId");
    const int8_t resolved =
        requestedId.length()
            ? pixLiteIndexById(config_, requestedId.c_str())
            : (config_.pixliteCount
                   ? static_cast<int8_t>(
                         config_.primaryPixliteIndex < config_.pixliteCount
                             ? config_.primaryPixliteIndex
                             : 0)
                   : -1);
    if (resolved < 0) {
      sendError(404, "PixLite target is not configured");
      return;
    }
    const uint8_t targetIndex = static_cast<uint8_t>(resolved);
    beginJsonStream();
    server_.sendContent(
        "{\"targetId\":\"" +
        jsonEscape(config_.pixlites[targetIndex].id) +
        "\",\"scenes\":[");
    bool first = true;
    for (uint8_t i = 0; i < pixlite_.mediaCount(targetIndex); ++i) {
      const MediaFile &media = pixlite_.media(targetIndex, i);
      if (media.playlist) continue;
      server_.sendContent(
          String(first ? "" : ",") + "\"" + jsonEscape(media.name) + "\"");
      first = false;
    }
    server_.sendContent("],\"playlists\":[");
    first = true;
    for (uint8_t i = 0; i < pixlite_.mediaCount(targetIndex); ++i) {
      const MediaFile &media = pixlite_.media(targetIndex, i);
      if (!media.playlist) continue;
      server_.sendContent(
          String(first ? "" : ",") + "\"" + jsonEscape(media.name) + "\"");
      first = false;
    }
    server_.sendContent("]}");
    endJsonStream();
  }

  void streamLog() {
    beginJsonStream();
    server_.sendContent("[");
    LogEntry entry{};
    const uint8_t count = log_.count();
    for (uint8_t i = 0; i < count; ++i) {
      if (!log_.snapshot(i, entry)) continue;
      String chunk = i ? "," : "";
      chunk += "{\"timeMs\":" + String(entry.timestampMs) +
               ",\"level\":\"" + Diagnostics::levelName(entry.level) +
               "\",\"message\":\"" + jsonEscape(entry.message) + "\"}";
      server_.sendContent(chunk);
    }
    server_.sendContent("]");
    endJsonStream();
  }

  void applyConnectRequest() {
    const String body = server_.arg("plain");
    JsonToken tokens[96];
    JsonDocument doc(body.c_str(), tokens, 96);
    if (!doc.parse()) {
      sendError(400, "Invalid JSON");
      return;
    }
    char requestedId[16]{};
    char requestedMac[13]{};
    char requestedHost[64]{};
    updateString(doc, doc.root(), "targetId", requestedId, sizeof(requestedId));
    updateString(doc, doc.root(), "mac", requestedMac, sizeof(requestedMac));
    updateString(doc, doc.root(), "host", requestedHost, sizeof(requestedHost));
    int8_t targetIndex = pixLiteIndexById(config_, requestedId);
    if (targetIndex < 0) targetIndex = pixLiteIndexByMac(config_, requestedMac);
    if (targetIndex < 0) targetIndex = pixLiteIndexByHost(config_, requestedHost);
    const bool adding = targetIndex < 0;
    if (adding) {
      if (config_.pixliteCount >= MAX_PIXLITES) {
        sendError(409, "The 16 PixLite limit has been reached");
        return;
      }
      targetIndex = static_cast<int8_t>(config_.pixliteCount);
      initializeJsonPixLite(config_.pixlites[targetIndex], targetIndex);
      // Array indexes are not identities: removing a middle entry can leave a
      // gap, so choose the first unused stable ID instead of reusing count+1.
      for (uint8_t suffix = 1; suffix <= MAX_PIXLITES + 1; ++suffix) {
        char candidate[16];
        snprintf(candidate, sizeof(candidate), "pixlite-%u", suffix);
        if (pixLiteIndexById(config_, candidate) < 0) {
          copyText(
              config_.pixlites[targetIndex].id,
              sizeof(config_.pixlites[targetIndex].id),
              candidate);
          break;
        }
      }
      ++config_.pixliteCount;
    }
    PixLiteConfig &target = config_.pixlites[targetIndex];
    if (requestedId[0]) copyText(target.id, sizeof(target.id), requestedId);
    updateString(doc, doc.root(), "name", target.displayName, sizeof(target.displayName));
    updateString(doc, doc.root(), "mac", target.macAddress, sizeof(target.macAddress));
    updateString(doc, doc.root(), "host", target.host, sizeof(target.host));
    updateString(doc, doc.root(), "username", target.username, sizeof(target.username));
    // A blank password in the edit form means "keep the stored secret". This
    // prevents a harmless name/IP edit from silently breaking authentication.
    char requestedPassword[65]{};
    if (updateString(
            doc,
            doc.root(),
            "password",
            requestedPassword,
            sizeof(requestedPassword)) &&
        requestedPassword[0]) {
      copyText(target.password, sizeof(target.password), requestedPassword);
    }
    target.enabled = true;
    const int16_t deviceIndex = discovery_.findByMac(target.macAddress);
    if (deviceIndex >= 0) {
      copyText(
          target.host,
          sizeof(target.host),
          discovery_.device(deviceIndex).ipAddress);
      if (!target.displayName[0] ||
          genericPixLiteName(target.displayName)) {
        copyText(
            target.displayName,
            sizeof(target.displayName),
            discovery_.device(deviceIndex).nickname);
      }
    }
    if (!target.host[0]) {
      if (adding) --config_.pixliteCount;
      sendError(400, "PixLite IP address is required");
      return;
    }
    if (pixlite_.connectAndLoad(static_cast<uint8_t>(targetIndex))) {
      store_.save(config_);
      sendJson(
          200,
          "{\"ok\":true,\"targetId\":\"" +
              jsonEscape(target.id) + "\"}");
    } else {
      if (adding) {
        --config_.pixliteCount;
        memset(&config_.pixlites[config_.pixliteCount], 0, sizeof(PixLiteConfig));
      }
      sendError(502, pixlite_.lastError());
    }
  }

  static bool genericPixLiteName(const char *name) {
    if (!name || !name[0] || strcmp(name, "Primary PixLite") == 0) return true;
    if (strncmp(name, "PixLite ", 8) != 0 || !name[8]) return false;
    for (const char *cursor = name + 8; *cursor; ++cursor) {
      if (*cursor < '0' || *cursor > '9') return false;
    }
    return true;
  }

  void removePixLite() {
    const String body = server_.arg("plain");
    JsonToken tokens[32];
    JsonDocument doc(body.c_str(), tokens, 32);
    char targetId[16]{};
    if (!doc.parse() ||
        !updateString(doc, doc.root(), "targetId", targetId, sizeof(targetId))) {
      sendError(400, "PixLite targetId is required");
      return;
    }
    const int8_t resolved = pixLiteIndexById(config_, targetId);
    if (resolved < 0) {
      sendError(404, "PixLite target is not configured");
      return;
    }
    const uint8_t removing = static_cast<uint8_t>(resolved);
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      if (!config_.inputs[i].enabled) continue;
      const ActionConfig *actions[] = {
          &config_.inputs[i].onAction,
          &config_.inputs[i].offAction};
      for (const ActionConfig *action : actions) {
        if (action->kind != ActionKind::None &&
            pixLiteIndexById(config_, action->targetId) == resolved) {
          sendError(
              409,
              "Remove or retarget input actions before forgetting this PixLite");
          return;
        }
      }
    }
    for (uint8_t i = removing; i + 1 < config_.pixliteCount; ++i) {
      config_.pixlites[i] = config_.pixlites[i + 1];
      if (memory_.pixliteStatuses) {
        memory_.pixliteStatuses[i] = memory_.pixliteStatuses[i + 1];
      }
      if (memory_.mediaCounts) memory_.mediaCounts[i] = memory_.mediaCounts[i + 1];
      if (memory_.media) {
        memcpy(
            memory_.media + i * MAX_MEDIA_FILES,
            memory_.media + (i + 1) * MAX_MEDIA_FILES,
            sizeof(MediaFile) * MAX_MEDIA_FILES);
      }
    }
    --config_.pixliteCount;
    memset(&config_.pixlites[config_.pixliteCount], 0, sizeof(PixLiteConfig));
    if (memory_.pixliteStatuses) {
      memset(
          &memory_.pixliteStatuses[config_.pixliteCount],
          0,
          sizeof(PixLiteStatus));
    }
    if (memory_.mediaCounts) memory_.mediaCounts[config_.pixliteCount] = 0;
    if (config_.pixliteCount == 0 ||
        config_.primaryPixliteIndex == removing) {
      config_.primaryPixliteIndex = 0;
    } else if (config_.primaryPixliteIndex > removing) {
      --config_.primaryPixliteIndex;
    }
    store_.save(config_);
    sendJson(200, "{\"ok\":true}");
  }

  void testAction() {
    const String body = server_.arg("plain");
    ActionConfig action = defaultAction();
    JsonToken tokens[96];
    JsonDocument doc(body.c_str(), tokens, 96);
    if (!doc.parse()) {
      sendError(400, "Invalid action JSON");
      return;
    }
    parseAction(doc, doc.root(), action);
    if (action.kind == ActionKind::None) {
      sendJson(200, "{\"ok\":true}");
      return;
    }
    if (pixlite_.execute(action)) sendJson(200, "{\"ok\":true}");
    else sendError(502, pixlite_.lastError());
  }

  void applyConfiguration(bool restart = false) {
    const String requestBody = server_.arg("plain");
    if (!payloadWithinLimit(requestBody.length(), CONFIG_REQUEST_LIMIT)) {
      sendError(413, "Configuration request exceeds 12 KB");
      return;
    }
    if (!memory_.tokens) {
      sendError(503, "Configuration changes disabled in degraded memory mode");
      return;
    }
    AppConfig candidate = config_;
    char error[128];
    if (!applyConfigJson(
            requestBody.c_str(),
            candidate,
            board_,
            memory_.tokens,
            CONFIG_TOKEN_CAPACITY,
            error,
            sizeof(error))) {
      if (server_.uri() == "/api/config/import") {
        String response = "{\"error\":\"" + jsonEscape(error) +
                          "\",\"remapRequired\":true}";
        sendJson(409, response);
      } else {
        sendError(400, error);
      }
      return;
    }
    JsonDocument passwordDocument(
        requestBody.c_str(), memory_.tokens, CONFIG_TOKEN_CAPACITY);
    if (passwordDocument.parse()) {
      const int16_t network = passwordDocument.objectValue(passwordDocument.root(), "network");
      const int16_t passwordToken =
          network >= 0 ? passwordDocument.objectValue(network, "apPassword") : -1;
      if (passwordToken >= 0) {
        char password[64];
        if (passwordDocument.stringValue(passwordToken, password, sizeof(password))) {
          setWebPassword(candidate.security, password);
        }
      }
    }
    config_ = candidate;
    store_.save(config_);
    delegate_.configurationChanged();
    sendJson(200, "{\"ok\":true,\"remapRequired\":false}");
    if (restart) {
      delay(100);
      ESP.restart();
    }
  }

  AppConfig &config_;
  const BoardProfile &board_;
  ConfigStore &store_;
  AdarDiscovery &discovery_;
  PixLiteClient &pixlite_;
  Diagnostics &log_;
  MemoryResources &memory_;
  WebApiDelegate &delegate_;
  WebServer server_;
};

}  // namespace advatek

// =============================================================================
// Module: src/platform/App.h
// =============================================================================
#include <esp_heap_caps.h>
#include <esp_system.h>
















namespace advatek {

class App : public WebApiDelegate {
 public:
  App()
      : board_(activeBoardProfile()),
        log_(memory_),
        network_(board_, config_, log_),
        discovery_(network_, log_, memory_),
        pixlite_(config_, log_, memory_),
        web_(config_, board_, store_, discovery_, pixlite_, log_, memory_, *this) {}

  void begin() {
    Serial.begin(115200);
    delay(100);
    const bool memoryReady = memory_.begin();
    log_.begin();
    log_.addf(
        LogLevel::Info,
        "Boot firmware=%s source=%s board=%s profile=%s",
        FIRMWARE_VERSION,
        SOURCE_REVISION,
        board_.id,
        board_.profileVersion);
    log_.addf(
        LogLevel::Info,
        "Hardware flash=%lu PSRAM=%lu freePSRAM=%lu mode=%s",
        static_cast<unsigned long>(ESP.getFlashChipSize()),
        static_cast<unsigned long>(ESP.getPsramSize()),
        static_cast<unsigned long>(ESP.getFreePsram()),
        memoryReady ? "normal" : "degraded-recovery");
    log_.addf(
        LogLevel::Info,
        "Reset reason=%s (%d)",
        resetReasonName(esp_reset_reason()),
        static_cast<int>(esp_reset_reason()));
    char suffix[7];
    const uint64_t chip = ESP.getEfuseMac();
    snprintf(suffix, sizeof(suffix), "%06llX", chip & 0xFFFFFFULL);
    // Ethernet commissioning uses a predictable first-run mDNS name. The
    // recovery AP keeps a unique SSID so installers can identify its unit.
    const char *hostname = "advatrigger";
    char apName[40];
    snprintf(apName, sizeof(apName), "Advatek-Trigger-%s", suffix);

    store_.begin();
    const bool loaded = store_.load(config_, board_, hostname);
    showStatusOrange();
    log_.addf(
        LogLevel::Info,
        "%s configuration schema %u (%s)",
        loaded ? "Loaded" : "Created default",
        config_.schemaVersion,
        board_.id);
    if (!memoryReady) {
      log_.add(
          LogLevel::Error,
          "PSRAM workspace unavailable; recovery UI active and discovery/config imports disabled");
    }
    if (ESP.getFlashChipSize() <
        static_cast<uint32_t>(board_.minimumFlashMb) * 1024U * 1024U) {
      log_.addf(
          LogLevel::Error,
          "Expected at least %u MB flash was not detected",
          board_.minimumFlashMb);
    }
    if (board_.requiresPsram &&
        ESP.getPsramSize() <
            static_cast<uint32_t>(board_.minimumPsramMb) * 1024U * 1024U) {
      log_.addf(
          LogLevel::Error,
          "Expected at least %u MB PSRAM was not detected",
          board_.minimumPsramMb);
    }
    sampleMemory(true);
    configurePins();
    pinMode(board_.recoveryButtonPin, INPUT_PULLUP);
    network_.begin(apName);
    web_.begin();
    // Live industrial-board testing showed the combined HTTP, ADAR and PixLite
    // call path could leave less than 200 bytes free in the original 12 KB
    // task. The extra 8 KB remains well inside the internal-heap budget and
    // gives network failures enough headroom to unwind safely.
    xTaskCreatePinnedToCore(networkTaskEntry, "network", 20480, this, 1, &networkTask_, 0);
  }

  void loop() {
    scanInputs();
    scanRecoveryButton();
    const uint32_t now = millis();
    updateStatusLed(now);
    if (factoryResetRequested_ && dueAt(now, factoryResetDueAt_)) {
      performFactoryReset();
    }
    delay(2);
  }

  String stateJson() override {
    const PixLiteStatus &status = pixlite_.primaryStatus();
    const PixLiteConfig *primary =
        config_.pixliteCount
            ? &config_.pixlites[
                  config_.primaryPixliteIndex < config_.pixliteCount
                      ? config_.primaryPixliteIndex
                      : 0]
            : nullptr;
    String json = "{\"firmwareVersion\":\"" + String(FIRMWARE_VERSION) + "\",";
    json += "\"webVersion\":\"" + String(WEB_UI_VERSION) + "\",";
    json += "\"sourceRevision\":\"" + String(SOURCE_REVISION) + "\",";
    json += "\"boardId\":\"" + jsonEscape(board_.id) + "\",";
    json += "\"boardProfileVersion\":\"" + jsonEscape(board_.profileVersion) + "\",";
    json += "\"hostname\":\"" + jsonEscape(config_.network.hostname) + "\",";
    json += "\"uplink\":\"" +
            String(config_.network.uplink == UplinkMode::Ethernet ? "ethernet" : "wifi") + "\",";
    json += "\"uplinkOnline\":" + String(network_.uplinkConnected() ? "true" : "false") + ",";
    json += "\"ip\":\"" + network_.localIP().toString() + "\",";
    json += "\"apRunning\":" + String(network_.accessPointRunning() ? "true" : "false") + ",";
    json += "\"ethernetRecoveryRunning\":" +
            String(network_.ethernetRecoveryRunning() ? "true" : "false") + ",";
    json += "\"statusLed\":{\"enabled\":" +
            String(config_.statusLed.enabled ? "true" : "false") +
            ",\"brightnessPercent\":" +
            String(config_.statusLed.brightnessPercent) + "},";
    json += "\"pinRemappingRequired\":" +
            String(configurationNeedsRemap(config_, board_) ? "true" : "false") + ",";
    json += "\"degradedMode\":" + String(memory_.ready ? "false" : "true") + ",";
    json += "\"memory\":{\"flashBytes\":" + String(ESP.getFlashChipSize()) + ",";
    json += "\"internalFree\":" + String(memorySnapshot_.internalFree) + ",";
    json += "\"internalMinimum\":" + String(memorySnapshot_.internalMinimum) + ",";
    json += "\"internalLargestBlock\":" + String(memorySnapshot_.internalLargestBlock) + ",";
    json += "\"psramTotal\":" + String(ESP.getPsramSize()) + ",";
    json += "\"psramFree\":" + String(ESP.getFreePsram()) + ",";
    json += "\"networkTaskStackWatermarkBytes\":" +
            String(memorySnapshot_.networkStackWatermarkBytes) + ",";
    json += "\"resetReason\":\"" + String(resetReasonName(esp_reset_reason())) + "\",";
    json += "\"resetReasonCode\":" + String(static_cast<int>(esp_reset_reason())) + ",";
    json += "\"uptimeMs\":" + String(millis()) + "},";
    json += "\"ethernet\":{\"initialized\":" +
            String(network_.ethernetInitialized() ? "true" : "false") + ",";
    json += "\"initAttempts\":" + String(network_.ethernetInitAttempts()) + ",";
    json += "\"linkUp\":" + String(network_.ethernetLinkUp() ? "true" : "false") + ",";
    json += "\"addressing\":\"" +
            String(config_.network.ethernet.mode == IpMode::Static ? "static" : "dhcp") + "\",";
    json += "\"linkSpeedMbps\":" + String(network_.ethernetLinkSpeedMbps()) + ",";
    json += "\"fullDuplex\":" +
            String(network_.ethernetFullDuplex() ? "true" : "false") + ",";
    json += "\"failureReason\":\"" +
            jsonEscape(network_.ethernetFailureReason()) + "\"},";
    json += "\"pixlite\":{\"online\":" + String(status.online ? "true" : "false") + ",";
    json += "\"host\":\"" + jsonEscape(primary ? primary->host : "") + "\",";
    json += "\"mode\":\"" + jsonEscape(status.mode) + "\",";
    json += "\"currentFile\":\"" + jsonEscape(status.currentFile) + "\",";
    json += "\"latencyMs\":" + String(status.latencyMs) + ",";
    json += "\"error\":\"" + jsonEscape(status.lastError) + "\"},\"pixlites\":[";
    for (uint8_t i = 0; i < config_.pixliteCount && i < MAX_PIXLITES; ++i) {
      if (i) json += ',';
      const PixLiteConfig &target = config_.pixlites[i];
      const PixLiteStatus &targetStatus = pixlite_.status(i);
      json += "{\"id\":\"" + jsonEscape(target.id) +
              "\",\"name\":\"" + jsonEscape(target.displayName) +
              "\",\"mac\":\"" + jsonEscape(target.macAddress) +
              "\",\"host\":\"" + jsonEscape(target.host) +
              "\",\"online\":" + String(targetStatus.online ? "true" : "false") +
              ",\"mode\":\"" + jsonEscape(targetStatus.mode) +
              "\",\"currentFile\":\"" + jsonEscape(targetStatus.currentFile) +
              "\",\"latencyMs\":" + String(targetStatus.latencyMs) +
              ",\"error\":\"" + jsonEscape(targetStatus.lastError) + "\"}";
    }
    json += "],\"inputs\":[";
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      if (i) json += ',';
      json += "{\"active\":" + String(runtime_[i].stableActive ? "true" : "false") +
              ",\"ramping\":" + String(runtime_[i].rampActive ? "true" : "false") +
              ",\"eventSequence\":" + String(runtime_[i].eventSequence) + "}";
    }
    json += "]}";
    return json;
  }

  String inputStateJson() override {
    // This intentionally small response is polled more often than /api/state.
    // eventSequence prevents a complete press/release between polls being lost.
    String json = "{\"inputs\":[";
    json.reserve(420);
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      if (i) json += ',';
      json += "{\"active\":" + String(runtime_[i].stableActive ? "true" : "false") +
              ",\"eventSequence\":" + String(runtime_[i].eventSequence) + "}";
    }
    json += "]}";
    return json;
  }

  void configurationChanged() override {
    configurePins();
    triggerFlashActive_ = false;
    showStatusOrange();
    log_.add(LogLevel::Info, "Configuration saved");
  }

  void requestFactoryReset() override {
    factoryResetRequested_ = true;
    factoryResetDueAt_ = millis() + 750;
    recoveryLedIntent_ = RecoveryIntent::FactoryReset;
  }

 private:
  static void networkTaskEntry(void *context) {
    static_cast<App *>(context)->networkTask();
  }

  void networkTask() {
    // Each target owns a two-second due time. Poll at most one per task pass so
    // action mailboxes are serviced between HTTP requests; when many targets
    // are due, the loop naturally uses all available network throughput.
    uint32_t pollDue[MAX_PIXLITES];
    uint32_t mediaRefreshDue[MAX_PIXLITES]{};
    uint32_t mediaPrimedIdentity[MAX_PIXLITES]{};
    uint32_t mediaAttemptedIdentity[MAX_PIXLITES]{};
    bool lastStatusPollOnline[MAX_PIXLITES]{};
    const uint32_t pollStartedAt = millis();
    for (uint8_t i = 0; i < MAX_PIXLITES; ++i) {
      pollDue[i] = pollStartedAt + (i * (STATUS_POLL_MS / MAX_PIXLITES));
    }
    uint32_t memoryDue = millis() + 5000;
    uint8_t pollIndex = 0;
    uint8_t mediaIndex = 0;
    for (;;) {
      network_.loop();
      web_.loop();
      const bool uplinkConnected = network_.uplinkConnected();
      if (!uplinkConnected) {
        // Ensure that restoring Ethernet/Wi-Fi also refreshes every saved
        // controller after its first successful status poll.
        memset(lastStatusPollOnline, 0, sizeof(lastStatusPollOnline));
      }
      PendingAction pending{};
      if (uplinkConnected) {
        for (uint8_t target = 0; target < MAX_PIXLITES; ++target) {
          if (mailboxes_[target].take(pending, millis())) {
            pixlite_.execute(target, pending.action);
          }
        }
      }
      bool mediaRequestAttempted = false;
      if (uplinkConnected && config_.pixliteCount) {
        const uint32_t mediaNow = millis();
        for (uint8_t checked = 0;
             checked < config_.pixliteCount && checked < MAX_PIXLITES;
             ++checked) {
          const uint8_t candidate =
              (mediaIndex + checked) % config_.pixliteCount;
          const uint32_t identity =
              pixliteTargetIdentity(config_.pixlites[candidate]);
          if (mediaAttemptedIdentity[candidate] != identity) {
            // A changed target should load immediately, while a failed load
            // of the same target remains bounded by the retry interval.
            mediaAttemptedIdentity[candidate] = identity;
            mediaRefreshDue[candidate] = mediaNow;
          }
          const bool identityChanged =
              mediaPrimedIdentity[candidate] != identity;
          if (config_.pixlites[candidate].enabled &&
              config_.pixlites[candidate].host[0] &&
              dueAt(mediaNow, mediaRefreshDue[candidate])) {
            mediaIndex = (candidate + 1) % config_.pixliteCount;
            mediaRequestAttempted = true;
            const bool loaded = identityChanged
                ? pixlite_.connectAndLoad(candidate)
                : pixlite_.refreshMedia(candidate);
            if (loaded) {
              mediaPrimedIdentity[candidate] = identity;
              // A successful version/media exchange already proves this
              // target is reachable; do not make the first status poll cause
              // a duplicate media refresh on a normal boot.
              lastStatusPollOnline[candidate] = true;
            }
            mediaRefreshDue[candidate] =
                mediaNow + (loaded ? MEDIA_REFRESH_MS : MEDIA_RETRY_MS);
            break;
          }
        }
      }
      // Do not issue a status poll in the same pass as version/media loading;
      // action mailboxes get another scheduling opportunity between requests.
      if (!mediaRequestAttempted && uplinkConnected && config_.pixliteCount) {
        const uint32_t pollNow = millis();
        for (uint8_t checked = 0;
             checked < config_.pixliteCount && checked < MAX_PIXLITES;
             ++checked) {
          const uint8_t candidate =
              (pollIndex + checked) % config_.pixliteCount;
          if (config_.pixlites[candidate].enabled &&
              config_.pixlites[candidate].host[0] &&
              dueAt(pollNow, pollDue[candidate])) {
            pollIndex = (candidate + 1) % config_.pixliteCount;
            pollDue[candidate] = pollNow + STATUS_POLL_MS;
            const bool previouslyOnline = lastStatusPollOnline[candidate];
            const bool pollSucceeded = pixlite_.pollStatus(candidate);
            lastStatusPollOnline[candidate] = pollSucceeded;
            if (mediaRefreshRequiredAfterPoll(
                    previouslyOnline, pollSucceeded)) {
              // The next task pass performs the serialized fileList request,
              // leaving this pass free to return to trigger action handling.
              mediaRefreshDue[candidate] = pollNow;
              log_.addf(
                  LogLevel::Info,
                  "%s reconnected; refreshing media",
                  config_.pixlites[candidate].displayName);
            }
            break;
          }
        }
      }
      if (dueAt(millis(), memoryDue)) {
        memoryDue = millis() + 5000;
        sampleMemory(false);
      }
      vTaskDelay(pdMS_TO_TICKS(2));
    }
  }

  static uint32_t pixliteTargetIdentity(const PixLiteConfig &target) {
    // A small stable fingerprint detects target changes without retaining
    // another copy of every hostname on the already-bounded network stack.
    uint32_t hash = 2166136261UL;
    const char *parts[] = {target.id, target.macAddress, target.host};
    for (const char *part : parts) {
      while (*part) {
        hash ^= static_cast<uint8_t>(*part++);
        hash *= 16777619UL;
      }
      hash ^= 0xFF;
      hash *= 16777619UL;
    }
    return hash ? hash : 1;
  }

  void configurePins() {
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      runtime_[i].initialized = false;
      const uint8_t gpio = config_.hardware.inputGpios[i];
      if (config_.inputs[i].enabled && gpio != PIN_UNASSIGNED && pinAllowed(board_, gpio)) {
        pinMode(gpio, INPUT_PULLUP);
      }
    }
  }

  void scanInputs() {
    const uint32_t now = millis();
    for (uint8_t i = 0; i < MAX_INPUTS; ++i) {
      const InputConfig &input = config_.inputs[i];
      InputRuntime &runtime = runtime_[i];
      if (!input.enabled) {
        runtime.rampActive = false;
        continue;
      }
      const uint8_t gpio = config_.hardware.inputGpios[i];
      if (gpio == PIN_UNASSIGNED || !pinAllowed(board_, gpio)) {
        runtime.rampActive = false;
        continue;
      }
      const bool pinLow = digitalRead(gpio) == LOW;
      const bool active = input.normallyClosed ? !pinLow : pinLow;
      const InputEvent event = InputEngine::update(runtime, active, input.debounceMs, now);
      if (event.occurred) {
        flashStatusLed(now);
        // An inverse edge always stops a held ramp before its own action.
        runtime.rampActive = false;
        const ActionConfig &configured = event.active ? input.onAction : input.offAction;
        ActionConfig action = configured;
        prepareIntensityAction(runtime, action, event.active, now);
        publishAction(i, event.active, action, now);
      }
      if (runtime.rampActive && dueAt(now, runtime.rampDueAt)) {
        ActionConfig repeated = input.onAction;
        applyRampStep(runtime, repeated);
        runtime.rampDueAt = now + repeated.repeatIntervalMs;
        publishAction(i, true, repeated, now);
      }
    }
  }

  void prepareIntensityAction(
      InputRuntime &runtime,
      ActionConfig &action,
      bool activeEdge,
      uint32_t now) {
    if (!activeEdge ||
        (action.kind != ActionKind::IntensityBrighter &&
         action.kind != ActionKind::IntensityDarker)) {
      return;
    }
    const PixLiteStatus &status = pixlite_.statusForAction(action);
    runtime.rampPercent =
        status.outputCount ? factorToPercent(status.outputs[0].programmedFactor) : action.intensityPercent;
    applyRampStep(runtime, action);
    runtime.rampActive = true;
    runtime.rampDueAt = now + action.holdDelayMs;
  }

  void applyRampStep(InputRuntime &runtime, ActionConfig &action) {
    runtime.rampPercent = nextRampPercent(
        runtime.rampPercent,
        action.intensityStepPercent,
        action.kind == ActionKind::IntensityDarker);
    action.intensityPercent = runtime.rampPercent;
  }

  void publishAction(
      uint8_t inputIndex,
      bool activeEdge,
      const ActionConfig &action,
      uint32_t now) {
    if (action.kind == ActionKind::None) return;
    const int8_t target = pixLiteIndexById(config_, action.targetId);
    if (target < 0) {
      log_.addf(
          LogLevel::Warning,
          "Input %u action ignored because its PixLite target is unavailable",
          inputIndex + 1);
      return;
    }
    // Latest-event-wins is maintained independently for every PixLite so two
    // contacts aimed at different controllers cannot discard one another.
    mailboxes_[target].publish(inputIndex, activeEdge, action, now);
  }

  void scanRecoveryButton() {
    const bool pressed = digitalRead(board_.recoveryButtonPin) == LOW;
    const uint32_t now = millis();
    if (pressed && !recoveryPressed_) {
      recoveryPressed_ = true;
      recoveryStartedAt_ = now;
    } else if (pressed) {
      recoveryLedIntent_ = recoveryIntentForDuration(now - recoveryStartedAt_);
    } else if (!pressed && recoveryPressed_) {
      recoveryPressed_ = false;
      const uint32_t duration = now - recoveryStartedAt_;
      const RecoveryIntent intent = recoveryIntentForDuration(duration);
      if (intent == RecoveryIntent::FactoryReset) {
        requestFactoryReset();
      } else if (intent == RecoveryIntent::ClearAuthentication) {
        store_.clearAuthentication(config_);
        const bool recoveryStarted = network_.openRecoveryNetwork();
        recoveryLedIntent_ =
            recoveryStarted
                ? RecoveryIntent::ClearAuthentication
                : RecoveryIntent::FactoryReset;
        recoveryFeedbackUntil_ = now + 5000;
        log_.add(
            LogLevel::Warning,
            recoveryStarted
                ? "BOOT recovery cleared local authentication and opened the selected recovery connection"
                : "BOOT recovery cleared local authentication but the selected recovery connection was refused");
      } else {
        recoveryLedIntent_ = RecoveryIntent::None;
        if (intent == RecoveryIntent::Cancelled) {
          log_.add(LogLevel::Info, "BOOT recovery cancelled after 20-second hold");
        }
        showStatusOrange();
      }
    }
  }

  void performFactoryReset() {
    log_.add(LogLevel::Warning, "Factory reset requested");
    store_.clearAll();
    delay(200);
    ESP.restart();
  }

  void showStatusOrange() {
    if (board_.statusLedPin < 0) return;
    if (!config_.statusLed.enabled) {
      writeStatusLed(0, 0, 0);
      return;
    }
    // Tuned on the fitted WS2812 so the status color reads as orange rather
    // than pink through the board and enclosure.
    writeStatusLed(
        scaleStatusLedChannel(255),
        scaleStatusLedChannel(48),
        0);
  }

  void flashStatusLed(uint32_t now) {
    if (board_.statusLedPin < 0 || !config_.statusLed.enabled) return;
    const uint8_t white = scaleStatusLedChannel(255);
    writeStatusLed(white, white, white);
    triggerFlashActive_ = true;
    triggerFlashUntil_ = now + 120;
  }

  void updateStatusLed(uint32_t now) {
    if (board_.statusLedPin < 0) return;
    // Recovery feedback overrides the user brightness/off preference so a
    // person holding BOOT can safely see which release zone is armed.
    if (recoveryLedIntent_ == RecoveryIntent::FactoryReset) {
      writeStatusLed(255, 0, 0);
      return;
    }
    if (recoveryLedIntent_ == RecoveryIntent::ClearAuthentication) {
      if (!recoveryPressed_ && dueAt(now, recoveryFeedbackUntil_)) {
        recoveryLedIntent_ = RecoveryIntent::None;
        showStatusOrange();
      } else if (((now / 250U) & 1U) == 0) {
        writeStatusLed(255, 48, 0);
      } else {
        writeStatusLed(255, 255, 255);
      }
      return;
    }
    if (recoveryPressed_ &&
        recoveryLedIntent_ == RecoveryIntent::Cancelled) {
      showStatusOrange();
      return;
    }
    if (triggerFlashActive_ && dueAt(now, triggerFlashUntil_)) {
      triggerFlashActive_ = false;
      showStatusOrange();
    }
  }

  void writeStatusLed(uint8_t red, uint8_t green, uint8_t blue) {
    if (board_.statusLedPin < 0) return;
    const rgb_led_color_order_t order =
        board_.statusLedColorOrder == StatusLedColorOrder::Rgb
            ? LED_COLOR_ORDER_RGB
            : LED_COLOR_ORDER_GRB;
    rgbLedWriteOrdered(board_.statusLedPin, order, red, green, blue);
  }

  uint8_t scaleStatusLedChannel(uint8_t channel) const {
    const uint8_t brightness =
        config_.statusLed.brightnessPercent > 100
            ? 100
            : config_.statusLed.brightnessPercent;
    return static_cast<uint8_t>(
        (static_cast<uint16_t>(channel) * brightness + 50U) / 100U);
  }

  void sampleMemory(bool initial) {
    memorySnapshot_.internalFree =
        heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    memorySnapshot_.internalMinimum =
        heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    memorySnapshot_.internalLargestBlock =
        heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    // ESP-IDF reports this value in bytes, unlike upstream FreeRTOS where the
    // value is expressed in StackType_t words.
    memorySnapshot_.networkStackWatermarkBytes =
        networkTask_ ? uxTaskGetStackHighWaterMark(networkTask_) : 0;
    if ((initial || memorySnapshot_.internalFree < 64U * 1024U) && !warnedFreeHeap_) {
      if (memorySnapshot_.internalFree < 64U * 1024U) {
        log_.add(LogLevel::Warning, "Internal free heap crossed below 64 KB");
        warnedFreeHeap_ = true;
      }
    }
    if (memorySnapshot_.internalLargestBlock < 32U * 1024U && !warnedLargestBlock_) {
      log_.add(LogLevel::Warning, "Largest internal heap block crossed below 32 KB");
      warnedLargestBlock_ = true;
    }
    if (networkTask_ && memorySnapshot_.networkStackWatermarkBytes < 3U * 1024U &&
        !warnedNetworkStack_) {
      log_.add(LogLevel::Warning, "Network task stack watermark crossed below 3 KB");
      warnedNetworkStack_ = true;
    }
  }

  static const char *resetReasonName(esp_reset_reason_t reason) {
    switch (reason) {
      case ESP_RST_POWERON: return "power-on";
      case ESP_RST_EXT: return "external";
      case ESP_RST_SW: return "software";
      case ESP_RST_PANIC: return "panic";
      case ESP_RST_INT_WDT: return "interrupt-watchdog";
      case ESP_RST_TASK_WDT: return "task-watchdog";
      case ESP_RST_WDT: return "watchdog";
      case ESP_RST_DEEPSLEEP: return "deep-sleep";
      case ESP_RST_BROWNOUT: return "brownout";
      case ESP_RST_SDIO: return "sdio";
      default: return "unknown";
    }
  }

  struct MemorySnapshot {
    size_t internalFree = 0;
    size_t internalMinimum = 0;
    size_t internalLargestBlock = 0;
    size_t networkStackWatermarkBytes = 0;
  };

  const BoardProfile &board_;
  AppConfig config_{};
  InputRuntime runtime_[MAX_INPUTS]{};
  MemoryResources memory_;
  Diagnostics log_;
  ConfigStore store_;
  NetworkManager network_;
  AdarDiscovery discovery_;
  PixLiteClient pixlite_;
  ActionMailbox mailboxes_[MAX_PIXLITES];
  WebApi web_;
  TaskHandle_t networkTask_ = nullptr;
  bool recoveryPressed_ = false;
  bool factoryResetRequested_ = false;
  uint32_t recoveryStartedAt_ = 0;
  uint32_t recoveryFeedbackUntil_ = 0;
  uint32_t factoryResetDueAt_ = 0;
  RecoveryIntent recoveryLedIntent_ = RecoveryIntent::None;
  uint32_t triggerFlashUntil_ = 0;
  bool triggerFlashActive_ = false;
  MemorySnapshot memorySnapshot_{};
  bool warnedFreeHeap_ = false;
  bool warnedLargestBlock_ = false;
  bool warnedNetworkStack_ = false;
};

}  // namespace advatek

// =============================================================================
// Module: AdvatekTrigger.ino
// =============================================================================
/*
  Advatek PixLite Mk3 Contact Closure Trigger

  Canonical modular firmware. End users may instead download the generated
  board-specific single-file sketch from `generated/`.

  SPDX-License-Identifier: MIT
*/


advatek::App application;

void setup() {
  application.begin();
}

void loop() {
  application.loop();
}
