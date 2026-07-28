/*
  Advatek PixLite Mk3 Contact Closure Trigger
  Generated board artifact: Waveshare ESP32-S3-ETH-8DI-8RO
  Board profile: waveshare-esp32-s3-eth-8di-8ro
  Arduino-ESP32 core: >=3.3.10 <4
  Source revision: development

  This file is generated from the canonical modular source. Changes made here
  will be overwritten. See the repository README and PORTING.md.

  SPDX-License-Identifier: MIT
*/

#define ADVATEK_BOARD_WAVESHARE_ESP32_S3_ETH_8DI_8RO 1
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
      if (tokens_[index].parent != objectIndex) {
        index = skip(index);
        continue;
      }
      // Object children are an ordered key/value sequence. Advance over the
      // complete value after each key instead of inspecting string values as
      // potential keys. Without this distinction, {"uplink":"ethernet",
      // "ethernet":{...}} resolves the value "ethernet" as the key and skips
      // the real nested IPv4 object.
      const int16_t keyIndex = index;
      const int16_t valueIndex = keyIndex + 1;
      if (!valid(valueIndex) ||
          tokens_[valueIndex].parent != objectIndex) {
        return -1;
      }
      if (tokens_[keyIndex].type == JsonTokenType::String &&
          equals(keyIndex, key)) {
        return valueIndex;
      }
      index = skip(valueIndex);
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

inline bool validOperationalWifi(const NetworkConfig &network) {
  return network.uplink != UplinkMode::WifiStation || network.wifiSsid[0];
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
  if (!validOperationalWifi(config.network)) {
    validationMessage(
        result,
        "Operational Wi-Fi requires a network name (SSID).");
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
    const bool clearWifiPassword = doc.booleanValue(
        doc.objectValue(network, "clearWifiPassword"), false);
    char wifiPassword[64]{};
    if (clearWifiPassword) {
      config.network.wifiPassword[0] = '\0';
    } else if (
        updateString(
            doc, network, "wifiPassword", wifiPassword, sizeof(wifiPassword)) &&
        wifiPassword[0]) {
      // A blank password means "keep the stored password", matching the SPA
      // placeholder and allowing redacted configuration updates.
      copyText(
          config.network.wifiPassword,
          sizeof(config.network.wifiPassword),
          wifiPassword);
    }
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
    0xdb, 0x72, 0xdb, 0x48, 0x96, 0xe0, 0xfb, 0x7c, 0x45, 0x0a, 0x76, 0xc9,
    0xc0, 0x16, 0x08, 0x91, 0x94, 0x64, 0xcb, 0xa0, 0x40, 0x8e, 0xcb, 0x76,
    0x55, 0xab, 0xdb, 0x17, 0x8d, 0x65, 0x77, 0xcf, 0x8c, 0x5a, 0x3d, 0x4c,
    0x01, 0x49, 0x32, 0x4b, 0x60, 0x02, 0x85, 0x4c, 0xea, 0xd2, 0x14, 0x22,
    0xfa, 0x13, 0x36, 0x62, 0x1e, 0xf6, 0x65, 0x9f, 0xe7, 0x17, 0x26, 0x62,
    0x1f, 0xe7, 0x53, 0xea, 0x4b, 0x36, 0x4e, 0x5e, 0x80, 0x04, 0x40, 0xca,
    0x72, 0x75, 0x97, 0x23, 0x4a, 0x44, 0x22, 0xef, 0x79, 0xf2, 0xdc, 0xcf,
    0xc1, 0xf1, 0x4e, 0x92, 0xc5, 0xe2, 0x2e, 0x27, 0x68, 0x21, 0x96, 0xe9,
    0xf8, 0x18, 0xfe, 0x8f, 0x52, 0xcc, 0xe6, 0x11, 0x61, 0xe3, 0xe3, 0x05,
    0xc1, 0xc9, 0xf8, 0x78, 0x49, 0x04, 0x46, 0xf1, 0x02, 0x17, 0x9c, 0x88,
    0xe8, 0xcb, 0xe7, 0x1f, 0x7b, 0x47, 0xba, 0x8c, 0xe1, 0x25, 0x89, 0xae,
    0x29, 0xb9, 0xc9, 0xb3, 0x42, 0xa0, 0x38, 0x63, 0x82, 0x30, 0x11, 0x39,
    0x37, 0x34, 0x11, 0x8b, 0x28, 0x21, 0xd7, 0x34, 0x26, 0x3d, 0xf9, 0xe0,
    0x53, 0x46, 0x05, 0xc5, 0x69, 0x8f, 0xc7, 0x38, 0x25, 0xd1, 0xc0, 0xb1,
    0x3b, 0x10, 0x0b, 0xb2, 0x24, 0xbd, 0x38, 0x4b, 0xb3, 0xa2, 0xee, 0xe3,
    0xc9, 0xb0, 0x0f, 0xff, 0x9c, 0xf1, 0x71, 0x4a, 0xd9, 0x15, 0x2a, 0x48,
    0x1a, 0xd1, 0x38, 0x63, 0x68, 0x51, 0x90, 0x59, 0xe4, 0x24, 0x58, 0xe0,
    0x90, 0x2e, 0xf1, 0x9c, 0xec, 0xf1, 0xeb, 0xf9, 0xf7, 0xb7, 0xcb, 0xd4,
    0xff, 0x6e, 0xff, 0x35, 0xbf, 0x9e, 0xa3, 0xdb, 0x65, 0xca, 0x78, 0xf4,
    0x6c, 0x21, 0x44, 0x1e, 0xee, 0xed, 0xdd, 0xdc, 0xdc, 0x04, 0x37, 0xfb,
    0x41, 0x56, 0xcc, 0xf7, 0x86, 0xfd, 0x7e, 0x1f, 0x2a, 0x3f, 0x43, 0x30,
    0xe1, 0x1f, 0xb2, 0xdb, 0xe8, 0x59, 0x1f, 0xf5, 0xd1, 0x00, 0x0d, 0x9e,
    0x7d, 0xb7, 0xff, 0xf6, 0xbb, 0xfd, 0xd7, 0x39, 0x16, 0x0b, 0x34, 0xa3,
    0x69, 0x1a, 0x3d, 0xfb, 0x6e, 0xb8, 0x3f, 0x1b, 0x1c, 0xe2, 0x61, 0xfc,
    0x0c, 0x25, 0xd1, 0xb3, 0xf7, 0x7d, 0xd4, 0xbf, 0x1e, 0xa4, 0x83, 0x5e,
    0x70, 0xf8, 0xd7, 0x67, 0x7b, 0xaa, 0x32, 0xf4, 0xf4, 0xdd, 0xfe, 0x5b,
    0x67, 0x7c, 0x2c, 0xa8, 0x48, 0xc9, 0xf8, 0x55, 0x72, 0x8d, 0x05, 0xb9,
    0x42, 0xef, 0xf0, 0x25, 0x47, 0xff, 0xf3, 0xdf, 0xe8, 0x75, 0xc6, 0x04,
    0x8e, 0x05, 0xfa, 0x5c, 0xd0, 0xf9, 0x9c, 0x14, 0xc7, 0x7b, 0xaa, 0xda,
    0x31, 0x8f, 0x0b, 0x9a, 0x0b, 0x04, 0x1b, 0x1e, 0x39, 0xcb, 0x2c, 0x59,
    0xa5, 0xc4, 0x19, 0xbb, 0xb3, 0x15, 0x8b, 0x05, 0xcd, 0x98, 0xeb, 0xad,
    0xe3, 0x8c, 0x71, 0x81, 0x48, 0x94, 0x64, 0xf1, 0x6a, 0x49, 0x98, 0x08,
    0xe2, 0x82, 0x60, 0x41, 0xde, 0xa6, 0x04, 0x9e, 0x5c, 0x07, 0x76, 0xc3,
    0xf1, 0x82, 0x82, 0xa4, 0xef, 0x28, 0x17, 0x23, 0x3a, 0x73, 0xc9, 0xee,
    0x2e, 0x09, 0xf8, 0x2a, 0x87, 0x33, 0xe0, 0xf6, 0x6f, 0x57, 0x0f, 0x90,
    0x17, 0x24, 0xcd, 0x70, 0xe2, 0x78, 0x5e, 0x41, 0xc4, 0xaa, 0x60, 0xa3,
    0x59, 0x56, 0xb8, 0x6a, 0x1c, 0x8e, 0xb2, 0x19, 0xaa, 0x86, 0xfa, 0x65,
    0x45, 0x8a, 0xbb, 0x33, 0x92, 0x92, 0x58, 0x64, 0xc5, 0xab, 0x34, 0x75,
    0x9f, 0xc1, 0x68, 0xe7, 0xb0, 0xf7, 0xad, 0xae, 0x2e, 0x9e, 0x79, 0x1e,
    0x75, 0xb9, 0x37, 0x62, 0xe4, 0x06, 0xbd, 0x5f, 0x09, 0x0c, 0xb3, 0xff,
    0x78, 0xc9, 0x49, 0x71, 0x4d, 0x0a, 0x97, 0x47, 0xe3, 0x75, 0x3d, 0x46,
    0x0c, 0x63, 0x70, 0x8f, 0xce, 0xdc, 0x38, 0x90, 0xeb, 0x8e, 0x22, 0x27,
    0x5e, 0xd0, 0x34, 0x81, 0x05, 0x38, 0x5e, 0x5d, 0x31, 0x87, 0x8a, 0x71,
    0x80, 0x93, 0x84, 0x24, 0x1f, 0xb2, 0x84, 0x70, 0x2f, 0x0f, 0x04, 0x9e,
    0x7f, 0x00, 0x18, 0x89, 0x22, 0xe7, 0xdd, 0xc9, 0x87, 0x3f, 0x38, 0xbb,
    0xbb, 0x39, 0xac, 0x1d, 0x9e, 0x9b, 0x33, 0xda, 0xdd, 0xa5, 0x6e, 0xee,
    0x95, 0x5e, 0x90, 0xa9, 0x59, 0xb8, 0x66, 0x55, 0xfe, 0xba, 0x1a, 0x2c,
    0xdc, 0xe9, 0xfb, 0x7c, 0x75, 0x29, 0x0a, 0x42, 0xc2, 0x9d, 0x7e, 0xe9,
    0x8d, 0xcc, 0xbe, 0x23, 0xe6, 0x72, 0xb3, 0xf5, 0x71, 0xb4, 0x2e, 0x47,
    0x6a, 0xa3, 0x10, 0x0f, 0x28, 0x13, 0x64, 0x5e, 0x50, 0x71, 0xb7, 0xbb,
    0xeb, 0xc6, 0xf5, 0x53, 0x64, 0xbd, 0xf1, 0x7c, 0x1e, 0x14, 0x64, 0x46,
    0x8a, 0x82, 0x14, 0xa7, 0x59, 0x4a, 0x63, 0x55, 0xb7, 0x59, 0x14, 0xb5,
    0xeb, 0x40, 0xab, 0xb8, 0xc8, 0x38, 0xff, 0x58, 0xd0, 0x39, 0x65, 0xb0,
    0xa0, 0x15, 0x27, 0xbd, 0xb8, 0x20, 0x09, 0x61, 0x70, 0x55, 0xb8, 0x33,
    0x89, 0x03, 0xeb, 0x31, 0x72, 0x28, 0x8b, 0xd3, 0x55, 0x42, 0x9c, 0xb0,
    0xd3, 0x12, 0xb3, 0x8c, 0xdd, 0x2d, 0xb3, 0x55, 0xb7, 0x4d, 0xb6, 0xa4,
    0xc2, 0x09, 0x5b, 0x85, 0x1c, 0x2f, 0x49, 0x2f, 0x93, 0xad, 0x1d, 0x3f,
    0x2e, 0xab, 0x5d, 0x80, 0x13, 0x5d, 0xd3, 0x99, 0xcb, 0x03, 0x92, 0x1b,
    0x58, 0x81, 0xdf, 0xd1, 0x4e, 0x7f, 0x64, 0x36, 0x07, 0x76, 0x6a, 0x34,
    0x23, 0x22, 0x5e, 0xb8, 0x3c, 0x80, 0xcb, 0xe8, 0xc7, 0x5e, 0x59, 0x7a,
    0xae, 0x37, 0x8a, 0x53, 0xcc, 0x39, 0xfa, 0x03, 0x22, 0xb7, 0x82, 0xb0,
    0x84, 0xa3, 0xb7, 0x45, 0x91, 0x15, 0x6a, 0x53, 0x8b, 0x15, 0x40, 0x94,
    0x4b, 0x7c, 0xe6, 0xad, 0xf9, 0x2a, 0x27, 0x85, 0x4b, 0x3c, 0x5f, 0x2c,
    0x28, 0x0f, 0x72, 0x7c, 0x07, 0xc7, 0x17, 0xb1, 0xb2, 0x4c, 0x89, 0x40,
    0x99, 0x9f, 0x46, 0xeb, 0xd2, 0x3f, 0x89, 0x38, 0xe1, 0x9c, 0x66, 0xec,
    0x4c, 0x64, 0x05, 0x9e, 0x93, 0x60, 0x4e, 0xc4, 0x89, 0x20, 0x4b, 0xd7,
    0xc1, 0xea, 0x9e, 0x9d, 0x62, 0xce, 0x6f, 0xb2, 0x22, 0x71, 0xbc, 0xc9,
    0xc4, 0x71, 0xfc, 0xb3, 0xe8, 0x7c, 0xe0, 0x0f, 0xfd, 0xc1, 0xa1, 0x3f,
    0x78, 0xee, 0x0f, 0x8e, 0xfc, 0xfd, 0x23, 0x7f, 0xff, 0xa5, 0x7f, 0xd0,
    0xbf, 0xf0, 0xaf, 0xa3, 0x9d, 0x81, 0x7f, 0xe3, 0x7f, 0x8c, 0xfa, 0xfe,
    0x2b, 0xf8, 0xf9, 0x3a, 0xda, 0x19, 0xe8, 0xb5, 0xfc, 0x3e, 0x3a, 0xbf,
    0xf0, 0xaf, 0x22, 0x09, 0xc1, 0x38, 0xf7, 0xff, 0x58, 0xfd, 0x22, 0x24,
    0x3a, 0x3f, 0x77, 0x58, 0xc6, 0x88, 0xe3, 0x3b, 0x1f, 0x32, 0x84, 0xe5,
    0xee, 0x38, 0x17, 0xfe, 0xb9, 0x93, 0xa7, 0xf8, 0xee, 0x2c, 0x26, 0xf2,
    0xcd, 0x69, 0x8a, 0xef, 0x10, 0x97, 0x0f, 0xe6, 0x15, 0x14, 0xa5, 0x00,
    0xd5, 0xfa, 0x6d, 0x6e, 0x9e, 0xa1, 0x02, 0x23, 0xb7, 0xc2, 0xb4, 0xfd,
    0x40, 0x6e, 0x85, 0xdd, 0xb6, 0x20, 0xd7, 0x34, 0x5b, 0xf1, 0xaa, 0x6b,
    0xfd, 0x6c, 0x55, 0xe1, 0x22, 0xcb, 0x1d, 0xdf, 0x79, 0x47, 0xaf, 0x09,
    0x5a, 0x66, 0x09, 0x41, 0x2e, 0x94, 0xc8, 0x11, 0x2e, 0x71, 0x7c, 0xe5,
    0xc9, 0x4a, 0x97, 0x29, 0x66, 0x57, 0x8e, 0xef, 0xfc, 0x00, 0x7f, 0x51,
    0xb6, 0x12, 0xf9, 0x4a, 0x70, 0xf9, 0x46, 0x10, 0x2e, 0x5e, 0x03, 0x86,
    0x75, 0x7c, 0xe7, 0x33, 0xe1, 0x42, 0xf5, 0xf1, 0x3f, 0xff, 0x8d, 0x78,
    0x96, 0xd2, 0x04, 0x01, 0xf2, 0x5d, 0x15, 0xcd, 0x9a, 0x3f, 0xe2, 0x84,
    0xb4, 0x6b, 0x7f, 0xfa, 0xe9, 0x07, 0x5d, 0x17, 0xcd, 0xe0, 0xb5, 0x1c,
    0xb4, 0xa0, 0xf3, 0x85, 0x20, 0xd0, 0xf3, 0x09, 0x20, 0x6f, 0x4e, 0xc5,
    0x1d, 0xaa, 0x0a, 0xa1, 0x46, 0x82, 0x8b, 0xab, 0xd6, 0x7b, 0x5d, 0x24,
    0x57, 0x46, 0x44, 0x55, 0xee, 0xf8, 0xce, 0x19, 0x11, 0x88, 0x56, 0xcf,
    0x50, 0xa1, 0x20, 0x29, 0xc1, 0x9c, 0xd8, 0x95, 0x3e, 0xa9, 0x22, 0x94,
    0x5d, 0x93, 0xa2, 0xa0, 0x30, 0x91, 0x0b, 0x1f, 0x47, 0x22, 0x1a, 0x57,
    0xb8, 0x6c, 0x4e, 0x84, 0xc6, 0x99, 0x3f, 0xdc, 0x9d, 0x24, 0xae, 0xf0,
    0xfc, 0x02, 0xde, 0x63, 0x57, 0x78, 0xc1, 0x35, 0x4e, 0x57, 0xc4, 0x5f,
    0x45, 0xae, 0xf0, 0x89, 0x67, 0x97, 0x45, 0x67, 0xa2, 0xa0, 0x6c, 0xee,
    0x12, 0x80, 0x2a, 0xcf, 0xff, 0xb9, 0x6a, 0x11, 0x2f, 0x48, 0x7c, 0x45,
    0x12, 0x3f, 0x81, 0x12, 0x5d, 0x49, 0xc8, 0x4a, 0x41, 0x41, 0xf2, 0x14,
    0xc7, 0xc4, 0xdd, 0x3b, 0xdf, 0x3d, 0x1e, 0x3b, 0xcf, 0x2e, 0xf6, 0xe6,
    0x3e, 0x89, 0xc6, 0xee, 0xda, 0xd9, 0x75, 0x42, 0x67, 0x17, 0x2f, 0xf3,
    0x91, 0xe3, 0x3b, 0xc7, 0xf0, 0x3b, 0x15, 0xf0, 0x73, 0x0c, 0x3f, 0xe7,
    0xf0, 0xf3, 0x99, 0xf3, 0x2c, 0x74, 0x76, 0x7f, 0x59, 0x65, 0xb2, 0xfc,
    0x19, 0x94, 0x3f, 0xd9, 0x7f, 0x39, 0x72, 0x4a, 0xef, 0x9c, 0x5c, 0x78,
    0xa3, 0x6a, 0x29, 0x38, 0x49, 0xde, 0x5e, 0x13, 0x26, 0x00, 0x85, 0x11,
    0x46, 0x0a, 0xd7, 0xc9, 0x33, 0xd8, 0xa1, 0x22, 0xc9, 0x6e, 0x98, 0xe3,
    0x8b, 0x68, 0xdc, 0xa5, 0x1a, 0x00, 0xb3, 0xd7, 0x86, 0x6a, 0x8c, 0x08,
    0xa2, 0x8c, 0x0b, 0xcc, 0x62, 0x92, 0xcd, 0xd0, 0xef, 0x3e, 0xbf, 0x7f,
    0x77, 0xc2, 0xf2, 0x95, 0xd9, 0x1f, 0x20, 0x19, 0x15, 0x5e, 0x96, 0x20,
    0xb2, 0xbb, 0x2b, 0x02, 0x81, 0x8b, 0x39, 0x11, 0x3b, 0x51, 0x24, 0xc9,
    0xcb, 0x65, 0xba, 0x2a, 0x5c, 0xaf, 0xf4, 0x77, 0xfa, 0x9e, 0xbe, 0x3d,
    0x9f, 0x23, 0x7b, 0xdc, 0xb3, 0x80, 0xb2, 0x84, 0xdc, 0x7e, 0x9c, 0xb9,
    0xc2, 0xfb, 0x7e, 0x60, 0xd0, 0xe7, 0xf5, 0xee, 0x2e, 0x19, 0xf7, 0x27,
    0xd3, 0x37, 0x27, 0x4f, 0xd7, 0xa4, 0x44, 0xee, 0x4f, 0xa7, 0x27, 0x1f,
    0xd1, 0xd3, 0xb5, 0x28, 0xbd, 0x69, 0x38, 0xad, 0x1e, 0xa6, 0xe5, 0x08,
    0xf3, 0x3b, 0x16, 0xa3, 0x0a, 0x0f, 0xcd, 0xe0, 0x60, 0xa2, 0x75, 0x69,
    0x50, 0x32, 0x93, 0x57, 0xf3, 0x77, 0x04, 0x27, 0xa4, 0xe0, 0x2e, 0x09,
    0x16, 0xea, 0x97, 0x37, 0x3a, 0xd9, 0xdd, 0x65, 0x01, 0x27, 0xc2, 0x75,
    0xfe, 0xb5, 0xa7, 0x29, 0x70, 0xaf, 0x42, 0x0d, 0xfe, 0x89, 0xe7, 0x93,
    0xe0, 0x32, 0x4b, 0xee, 0xaa, 0x5a, 0xaf, 0x15, 0x77, 0xd1, 0xfb, 0x7c,
    0x97, 0x03, 0x68, 0xe3, 0x3c, 0x4f, 0x69, 0x2c, 0x69, 0xd7, 0xde, 0xcf,
    0x3c, 0x63, 0x8e, 0x59, 0x1c, 0x8d, 0xf0, 0x0d, 0xa6, 0x02, 0x29, 0x3c,
    0x27, 0xfc, 0x75, 0x10, 0x04, 0xc4, 0xd7, 0xc3, 0x86, 0xac, 0xf4, 0x80,
    0xee, 0xd2, 0x80, 0x0b, 0x2c, 0x56, 0x3c, 0x8a, 0xa2, 0x83, 0xfe, 0xa0,
    0x26, 0x1f, 0x79, 0x91, 0x2d, 0x73, 0xe1, 0x3a, 0x86, 0x25, 0xd0, 0x2c,
    0x00, 0xca, 0x2b, 0x9c, 0x05, 0xad, 0xe3, 0x28, 0x8a, 0xd8, 0x2a, 0x4d,
    0x3d, 0xb1, 0x28, 0xb2, 0x1b, 0x04, 0x0b, 0x94, 0x08, 0xd3, 0x75, 0xcc,
    0x02, 0x50, 0x41, 0x7e, 0x59, 0xd1, 0x82, 0x40, 0x03, 0xbd, 0xa1, 0x27,
    0x51, 0xec, 0xb7, 0x30, 0x23, 0xdf, 0x86, 0x19, 0x61, 0xf9, 0x72, 0x23,
    0xbd, 0x52, 0xd3, 0x7a, 0xbd, 0x28, 0x1a, 0xc0, 0x5a, 0x5d, 0x39, 0x8b,
    0x1d, 0x1a, 0x64, 0x57, 0xd6, 0x14, 0xfe, 0x00, 0xb8, 0x1f, 0xa6, 0x71,
    0x7f, 0x3f, 0xfd, 0x44, 0x7e, 0x59, 0xc1, 0xdd, 0x9f, 0x61, 0x9a, 0x92,
    0x04, 0xb9, 0x4f, 0xd7, 0x66, 0xc5, 0xa5, 0x37, 0xf5, 0x79, 0x35, 0x29,
    0x5e, 0x13, 0x90, 0xa5, 0x3c, 0xb8, 0x9d, 0x41, 0x7d, 0x70, 0xd8, 0x75,
    0x44, 0x86, 0x81, 0xce, 0x8f, 0x58, 0x20, 0xc8, 0xad, 0xd0, 0x47, 0x10,
    0x09, 0x1b, 0x24, 0xe5, 0xca, 0x27, 0x22, 0x58, 0x12, 0xce, 0xf1, 0x9c,
    0x84, 0xe6, 0x86, 0x79, 0x3e, 0x0b, 0xb8, 0xb8, 0x4b, 0xe1, 0x18, 0x8b,
    0x84, 0x14, 0xef, 0xc8, 0x4c, 0xe1, 0xa6, 0x88, 0x4c, 0x9c, 0x27, 0xc9,
    0xf3, 0xc3, 0xfe, 0xfe, 0xa5, 0x13, 0x3a, 0x4f, 0x14, 0xb3, 0xe6, 0xf8,
    0x2c, 0x90, 0x44, 0x08, 0x2e, 0x0a, 0xdc, 0x1a, 0xd7, 0xe1, 0x8b, 0xec,
    0xc6, 0xf1, 0x7c, 0x4e, 0xc4, 0x67, 0xba, 0x24, 0xd9, 0x4a, 0xb8, 0xae,
    0x17, 0x8d, 0xed, 0x6a, 0x05, 0x59, 0x66, 0xd7, 0xa4, 0xaa, 0xb9, 0x7f,
    0xd8, 0xef, 0x7b, 0xf5, 0x82, 0x7e, 0xe7, 0x0a, 0x6f, 0xad, 0xd7, 0x29,
    0x02, 0x51, 0xd0, 0xa5, 0xeb, 0x05, 0x22, 0x7b, 0x97, 0xdd, 0x90, 0xe2,
    0x35, 0xe6, 0xc4, 0xb5, 0xee, 0xff, 0x5f, 0x80, 0xe7, 0xe4, 0x93, 0xf0,
    0xcf, 0x7b, 0x7f, 0xde, 0xdb, 0xf3, 0x1b, 0xa8, 0xe1, 0xcf, 0x41, 0x9a,
    0xc5, 0x38, 0xfd, 0xf3, 0xde, 0xe4, 0x69, 0xeb, 0xcd, 0xf9, 0x5f, 0x70,
    0xef, 0xaf, 0xfd, 0xde, 0xcb, 0xde, 0xc5, 0xf7, 0x7b, 0x73, 0xdf, 0xe9,
    0xd9, 0xef, 0x7a, 0xdd, 0xa2, 0xbf, 0xf4, 0xee, 0x7b, 0x4f, 0xa1, 0xd4,
    0xf1, 0x02, 0x9e, 0xd2, 0x98, 0xb8, 0x7d, 0x7f, 0x7f, 0x60, 0xb7, 0x51,
    0x03, 0xd4, 0x2b, 0x78, 0xe7, 0x9a, 0x05, 0xac, 0xaf, 0x28, 0x4b, 0x42,
    0x4d, 0xd7, 0xd4, 0xf5, 0x3e, 0x49, 0xc2, 0x6c, 0x12, 0xe4, 0x05, 0x5d,
    0xe2, 0xe2, 0xee, 0x94, 0xde, 0xa6, 0x54, 0x90, 0x93, 0xe4, 0xfe, 0x1e,
    0x0a, 0xd5, 0x13, 0x9f, 0x04, 0xe7, 0xfd, 0x8b, 0x49, 0x40, 0x93, 0xfb,
    0x7b, 0x47, 0x57, 0x74, 0xfc, 0x25, 0x49, 0x28, 0x06, 0xbe, 0x2c, 0x74,
    0x1c, 0xbf, 0x20, 0x39, 0xc1, 0x22, 0x74, 0x66, 0x59, 0x41, 0xae, 0x01,
    0xcd, 0x2b, 0xba, 0x13, 0x3a, 0x39, 0xbd, 0x25, 0x29, 0x77, 0x7c, 0xbc,
    0xba, 0xfd, 0xa8, 0x8a, 0x06, 0x7e, 0x85, 0xd9, 0x4f, 0x49, 0x11, 0x13,
    0x26, 0xc2, 0x41, 0xbf, 0xef, 0x73, 0x41, 0x72, 0xf3, 0x7c, 0xe8, 0x2f,
    0xb2, 0x34, 0x79, 0x43, 0x52, 0x7c, 0xf7, 0x9e, 0x87, 0x07, 0xfd, 0xbe,
    0xee, 0x1f, 0xb0, 0x7f, 0x71, 0x8d, 0xd3, 0xf7, 0x3c, 0x1c, 0x1e, 0xf6,
    0x7d, 0xa0, 0x3e, 0xfa, 0x67, 0x45, 0xb0, 0x00, 0x10, 0x66, 0xfd, 0x7e,
    0xbf, 0xef, 0x94, 0xf5, 0xfa, 0x05, 0xa9, 0x8f, 0x70, 0x47, 0xdc, 0xdf,
    0x0b, 0x40, 0x73, 0xa7, 0x6a, 0x21, 0xe8, 0x94, 0xde, 0xbe, 0xa3, 0x82,
    0x38, 0x9b, 0x8b, 0xd1, 0xfb, 0xab, 0x7d, 0xe7, 0xfe, 0x7e, 0xef, 0x2f,
    0xfa, 0xd9, 0x9d, 0x84, 0x50, 0xe4, 0x4d, 0xd0, 0x9f, 0x93, 0xef, 0x9f,
    0xee, 0x05, 0x30, 0xb0, 0x2b, 0xac, 0xbd, 0x66, 0x72, 0x2c, 0x83, 0x13,
    0xf7, 0xfe, 0xf2, 0xe4, 0xbc, 0xdf, 0x7b, 0x89, 0x7b, 0xb3, 0x8b, 0xf5,
    0xf3, 0xf2, 0xe9, 0x1e, 0x35, 0x0d, 0x26, 0xa2, 0x9e, 0xa9, 0xbe, 0x44,
    0x6b, 0x78, 0xf5, 0x89, 0x24, 0xe1, 0x87, 0xd5, 0xf2, 0x92, 0x14, 0x41,
    0x0e, 0xb2, 0xd6, 0x09, 0x13, 0x2e, 0xd1, 0xc7, 0x3c, 0xf0, 0xf7, 0x3d,
    0x7f, 0xf0, 0xdc, 0x93, 0xab, 0xfd, 0xa9, 0x20, 0x84, 0x6d, 0xad, 0xba,
    0xef, 0x1f, 0xd6, 0x55, 0x7f, 0x48, 0x57, 0x64, 0x6b, 0xcd, 0x43, 0xff,
    0x85, 0xac, 0x69, 0x6d, 0x17, 0xb5, 0xb6, 0xcb, 0xcd, 0x2a, 0x28, 0xb8,
    0xbf, 0x3f, 0xbf, 0xf0, 0x82, 0x25, 0xce, 0x5d, 0x16, 0x8d, 0xa7, 0xc7,
    0x59, 0x2e, 0x2b, 0x2b, 0x9a, 0xe9, 0x3c, 0x5d, 0x27, 0x2e, 0x0b, 0x68,
    0xe2, 0x95, 0x0e, 0x20, 0x74, 0x40, 0x6c, 0x01, 0x4d, 0x26, 0x0e, 0x97,
    0x92, 0x04, 0x49, 0x9c, 0xd0, 0x71, 0xca, 0xb1, 0xaa, 0x05, 0x02, 0xdf,
    0xfd, 0x3d, 0x0b, 0x16, 0x19, 0x17, 0x5e, 0x79, 0xbc, 0xa7, 0x7a, 0x1a,
    0x4f, 0xbd, 0xe0, 0xe7, 0x8c, 0x32, 0xd7, 0x71, 0xbc, 0xfb, 0xfb, 0x67,
    0xad, 0xfe, 0x9d, 0xf1, 0xab, 0x24, 0x41, 0xd8, 0x3e, 0x16, 0x34, 0xa3,
    0x05, 0x17, 0x55, 0xf3, 0x67, 0xf5, 0x02, 0x30, 0xa9, 0x84, 0x28, 0x11,
    0xb9, 0x69, 0x6b, 0x05, 0x33, 0x9a, 0x0a, 0x52, 0xb8, 0x79, 0x34, 0xce,
    0x83, 0x8c, 0xa5, 0x94, 0x11, 0x2f, 0x48, 0x09, 0x9b, 0x8b, 0x85, 0x4f,
    0x22, 0x37, 0x0b, 0x28, 0xd0, 0xc5, 0x0d, 0x75, 0x09, 0xc3, 0x97, 0x29,
    0x49, 0xaa, 0xca, 0x2c, 0x4a, 0xe5, 0x12, 0x60, 0x39, 0x93, 0xa9, 0x16,
    0x36, 0x9f, 0xae, 0xeb, 0xc2, 0x52, 0xdd, 0xf8, 0xbd, 0x69, 0xe8, 0x7c,
    0x61, 0xf8, 0x1a, 0xd3, 0x14, 0x3a, 0x70, 0x46, 0xd8, 0x75, 0xe2, 0x8c,
    0x31, 0xa2, 0x58, 0x4a, 0x2f, 0xa0, 0x8c, 0x91, 0x02, 0x28, 0x72, 0x34,
    0x3d, 0xa6, 0xe3, 0xe3, 0x3d, 0x2a, 0xf7, 0x29, 0x0d, 0x56, 0x39, 0x88,
    0x5e, 0x1f, 0xe5, 0x14, 0x27, 0x53, 0xe8, 0x59, 0x95, 0x94, 0xc0, 0x85,
    0xc1, 0x23, 0xcd, 0xcb, 0x69, 0x68, 0xbd, 0xb8, 0xbf, 0x77, 0x3e, 0x10,
    0x71, 0x93, 0x15, 0x57, 0x4e, 0x89, 0xb2, 0xd9, 0x0c, 0x1a, 0x4e, 0xbd,
    0x72, 0xea, 0xb7, 0x47, 0x94, 0xa8, 0x4f, 0xca, 0x54, 0xd3, 0xba, 0x1c,
    0xd5, 0x1d, 0xe9, 0x31, 0x1d, 0xb5, 0x3d, 0x4e, 0xe8, 0xe8, 0xce, 0x9c,
    0x72, 0x5a, 0x51, 0xc7, 0xf3, 0x73, 0xe7, 0x8d, 0x94, 0xee, 0xd1, 0xc9,
    0xa9, 0xe3, 0xb7, 0x5a, 0xc2, 0xdc, 0x9a, 0xab, 0x6e, 0xd7, 0xb0, 0x37,
    0x8c, 0xe6, 0x25, 0x6c, 0x92, 0x64, 0xf4, 0xde, 0xc1, 0x96, 0x21, 0x9c,
    0x24, 0x05, 0xe1, 0xdc, 0xf1, 0x99, 0xff, 0xf8, 0x4d, 0x96, 0xed, 0x6d,
    0x00, 0x01, 0x45, 0x42, 0x91, 0xa5, 0x29, 0x29, 0xb8, 0xe3, 0x4f, 0x81,
    0xcf, 0x40, 0x6a, 0x45, 0x6a, 0x03, 0x3b, 0xb0, 0xad, 0x0e, 0xb6, 0x44,
    0x52, 0xfa, 0x9c, 0xfa, 0xaa, 0x43, 0xc9, 0x26, 0xa9, 0xf6, 0xa4, 0x84,
    0x2e, 0x67, 0x74, 0xbe, 0x2a, 0x48, 0x02, 0x93, 0xc8, 0x29, 0xfb, 0x44,
    0x96, 0x38, 0xcf, 0x29, 0x9b, 0x7f, 0xd2, 0x94, 0x7a, 0xe2, 0x40, 0xef,
    0x05, 0x14, 0xd7, 0xd4, 0x1b, 0xa0, 0xdf, 0x74, 0xf8, 0x56, 0x2c, 0x48,
    0xc1, 0x88, 0x80, 0x2d, 0x21, 0xfa, 0xf7, 0x24, 0x80, 0xad, 0xf9, 0x92,
    0xab, 0x63, 0x36, 0xa5, 0xb2, 0xf0, 0x2c, 0x27, 0x24, 0x79, 0x7f, 0x99,
    0xf3, 0x12, 0xc1, 0xff, 0x51, 0xa3, 0xc2, 0x6c, 0x95, 0xa6, 0x6f, 0x56,
    0x79, 0x4a, 0x6e, 0x27, 0x0e, 0xfc, 0x46, 0x89, 0x7c, 0x70, 0x42, 0x67,
    0x81, 0xd3, 0x99, 0x79, 0x2a, 0xa7, 0xa1, 0x3d, 0x14, 0x10, 0xf3, 0x55,
    0x41, 0x3e, 0x11, 0xcc, 0x33, 0x76, 0x7f, 0xef, 0xbc, 0x03, 0xe5, 0x8a,
    0xe2, 0x23, 0xd5, 0x0c, 0xdf, 0x93, 0x65, 0x06, 0x28, 0x3e, 0x0d, 0x12,
    0x32, 0x2f, 0x70, 0x42, 0x92, 0xf7, 0x59, 0x42, 0x26, 0xce, 0x1b, 0xfd,
    0x84, 0x0a, 0x12, 0x03, 0xd7, 0x7d, 0x27, 0x65, 0x02, 0x07, 0x40, 0xf0,
    0x3d, 0x16, 0x8b, 0xa0, 0xc8, 0x56, 0x2c, 0x71, 0xdd, 0x34, 0x58, 0xca,
    0x0e, 0x26, 0x52, 0x3e, 0x2e, 0x18, 0x4e, 0x7f, 0x2c, 0x08, 0xb9, 0xbf,
    0xef, 0x7b, 0x7b, 0x83, 0xfe, 0xf0, 0xc0, 0x2b, 0xd1, 0x1f, 0x7e, 0x40,
    0xe6, 0x15, 0x9a, 0x15, 0x84, 0x98, 0xad, 0xf9, 0x91, 0x16, 0xcb, 0x1b,
    0x5c, 0x48, 0x68, 0x99, 0xe9, 0xdf, 0x7f, 0x24, 0x05, 0xf0, 0x3a, 0xf7,
    0xf7, 0x4d, 0x70, 0x52, 0x0d, 0xbe, 0xe4, 0x82, 0x2e, 0x89, 0x3c, 0x1c,
    0x39, 0x83, 0x59, 0x9a, 0x65, 0x85, 0x3d, 0x83, 0x95, 0xac, 0xf0, 0x9e,
    0xcb, 0xd1, 0x9f, 0x13, 0x18, 0x7c, 0x49, 0xd9, 0x4a, 0x10, 0x2e, 0xc7,
    0xbc, 0x80, 0xcb, 0xa8, 0x18, 0x9a, 0xde, 0xbc, 0xa0, 0x49, 0xe3, 0x36,
    0x52, 0x89, 0xec, 0xdc, 0xf3, 0xdc, 0x5f, 0xf8, 0xa7, 0x17, 0x1e, 0x20,
    0xbd, 0x84, 0x5e, 0x23, 0x79, 0x79, 0x22, 0xd3, 0x2a, 0xc6, 0x45, 0xe2,
    0x8c, 0x8f, 0xf9, 0x12, 0xa7, 0xa9, 0xbc, 0xb7, 0x39, 0xa0, 0x34, 0xf3,
    0x78, 0x3a, 0x99, 0x1e, 0x63, 0xad, 0xaf, 0x82, 0x97, 0xa7, 0x80, 0x1f,
    0xa5, 0x1a, 0x28, 0x72, 0x3e, 0xe6, 0x04, 0xee, 0x5c, 0xe2, 0x2e, 0xbc,
    0xd2, 0x19, 0xeb, 0x1f, 0xc7, 0x7b, 0x78, 0x3c, 0x0d, 0xa7, 0xc7, 0x5c,
    0x14, 0x19, 0x9b, 0x9b, 0xaa, 0xdd, 0x5a, 0xea, 0xfd, 0x78, 0x5a, 0x1e,
    0xef, 0x25, 0xf4, 0xda, 0xc6, 0x9d, 0x23, 0xc3, 0xe9, 0x9d, 0x9f, 0x03,
    0xaf, 0x0b, 0x97, 0x1b, 0x89, 0x05, 0x41, 0x4c, 0x63, 0x06, 0x7f, 0x67,
    0xa7, 0x79, 0x15, 0x7d, 0xe7, 0x89, 0x79, 0x07, 0x5b, 0xda, 0xc1, 0xb3,
    0xd0, 0x22, 0xb3, 0x78, 0x01, 0x8d, 0xfe, 0x9c, 0x27, 0xa6, 0xa8, 0x6e,
    0xc6, 0x90, 0x44, 0x9e, 0x8e, 0x4f, 0xc6, 0x7d, 0xdf, 0x79, 0xa2, 0x30,
    0xa9, 0xde, 0xe5, 0x5f, 0x56, 0x34, 0xbe, 0xea, 0x71, 0x81, 0x0b, 0xd1,
    0xd8, 0x65, 0xde, 0xd8, 0x65, 0xff, 0x3f, 0xe4, 0x3e, 0xe3, 0x7a, 0x97,
    0x49, 0x8e, 0x9e, 0xae, 0x17, 0x13, 0x27, 0xc9, 0x24, 0x26, 0x72, 0x4a,
    0xa7, 0xda, 0xcf, 0xd3, 0xd2, 0x19, 0x1f, 0x03, 0xb6, 0x5c, 0x4c, 0x9c,
    0x5f, 0xff, 0xef, 0x7f, 0x3a, 0xe1, 0x7f, 0x7c, 0x3f, 0x28, 0x15, 0xfe,
    0xcc, 0xd5, 0x4e, 0x76, 0xb6, 0x25, 0x8e, 0xb8, 0x8d, 0xd4, 0xcf, 0x07,
    0xd5, 0xb5, 0x97, 0x90, 0x40, 0xc4, 0x2a, 0xef, 0xe5, 0x45, 0x36, 0x97,
    0xd8, 0xc7, 0x6b, 0x70, 0xab, 0xc0, 0xad, 0xef, 0x4f, 0x9c, 0x4f, 0x04,
    0x27, 0x77, 0x12, 0xe6, 0x63, 0x40, 0xb2, 0x68, 0x1f, 0xc9, 0x56, 0x08,
    0x66, 0xca, 0xa7, 0x65, 0x4b, 0x8c, 0xf9, 0xe4, 0x7a, 0x6b, 0x51, 0xdc,
    0xad, 0x53, 0x23, 0x4d, 0xb8, 0xce, 0x1e, 0xce, 0xe9, 0x1e, 0x40, 0x0f,
    0x71, 0x3c, 0xdf, 0x26, 0x4e, 0x16, 0xa5, 0x8a, 0xaa, 0x9f, 0x13, 0x89,
    0xe9, 0x26, 0xe7, 0x6b, 0x9a, 0x84, 0x35, 0xf3, 0xc5, 0x24, 0xdf, 0xb5,
    0x89, 0x57, 0xf1, 0x83, 0x20, 0xa8, 0x1a, 0x97, 0x17, 0xe1, 0xf9, 0x85,
    0xe7, 0x03, 0x3d, 0xf4, 0xff, 0xc5, 0xf5, 0xfc, 0x2f, 0x6e, 0xaa, 0xc9,
    0x9b, 0x57, 0xc6, 0x58, 0xca, 0x35, 0xde, 0x1a, 0x18, 0xf6, 0x9d, 0xbe,
    0xcd, 0x00, 0x7c, 0x71, 0x45, 0x74, 0x7e, 0xe1, 0xad, 0x45, 0x30, 0xcb,
    0x8a, 0xb7, 0x38, 0x5e, 0xb8, 0x52, 0x6d, 0x53, 0xc9, 0x7a, 0x34, 0xda,
    0x26, 0x62, 0x4f, 0xe9, 0xd3, 0x35, 0x2b, 0x7b, 0x52, 0xf2, 0xa4, 0xe2,
    0x6e, 0xea, 0x8d, 0xe8, 0xee, 0xae, 0x4b, 0x2d, 0x5e, 0x5b, 0x64, 0xf3,
    0x79, 0x4a, 0x5c, 0x47, 0x09, 0xa7, 0x00, 0x5b, 0x44, 0x0b, 0xaa, 0x9e,
    0x4f, 0x03, 0x05, 0xef, 0xa6, 0x64, 0xe2, 0x18, 0x15, 0xaa, 0xae, 0x1d,
    0x56, 0x05, 0x94, 0xe9, 0xa2, 0x1a, 0xde, 0x15, 0x87, 0xe3, 0x92, 0x80,
    0x80, 0xa0, 0x7c, 0x06, 0x92, 0x0b, 0x8b, 0x25, 0xda, 0x19, 0x8f, 0xc7,
    0x7d, 0x3f, 0x8e, 0x7e, 0x7f, 0xce, 0x2e, 0x40, 0xe0, 0x81, 0xbf, 0x11,
    0xf7, 0xe1, 0x3c, 0xaf, 0x33, 0x9a, 0xa0, 0xfe, 0xfd, 0x3d, 0x3f, 0x8e,
    0x8d, 0xb2, 0x4b, 0xeb, 0x21, 0x23, 0x89, 0x4f, 0x96, 0x94, 0xb9, 0xbc,
    0x17, 0xfb, 0x07, 0x72, 0x21, 0xb9, 0x5c, 0x8b, 0x12, 0x46, 0x38, 0x11,
    0xa7, 0x45, 0x96, 0x93, 0x42, 0xdc, 0xb9, 0x4e, 0xaf, 0x97, 0xaf, 0x52,
    0x4e, 0xb8, 0xe3, 0x6b, 0xa9, 0x25, 0xf7, 0x60, 0x35, 0x5d, 0x09, 0x63,
    0x96, 0x62, 0xbe, 0x70, 0xe0, 0x5d, 0x36, 0x9b, 0x71, 0x22, 0xfe, 0xa4,
    0x14, 0xe3, 0x6d, 0x91, 0x45, 0x57, 0xf3, 0x4a, 0xaf, 0x0d, 0x4f, 0x20,
    0x6e, 0x80, 0x7a, 0x6e, 0xe7, 0xb5, 0xb7, 0x7e, 0x0d, 0x3a, 0x39, 0x00,
    0x2e, 0xc3, 0xea, 0x34, 0x41, 0x4c, 0xdf, 0x3e, 0x6f, 0x64, 0xce, 0x3c,
    0x12, 0x16, 0x6f, 0xd3, 0x85, 0x85, 0x75, 0x39, 0xa3, 0x0c, 0xa7, 0xe9,
    0x1d, 0x74, 0x3c, 0x28, 0x2d, 0x78, 0xf8, 0x17, 0x8b, 0x9d, 0x6a, 0xd2,
    0x4c, 0xb8, 0x34, 0xfa, 0xb9, 0x17, 0x67, 0x2b, 0x26, 0x5a, 0x77, 0x06,
    0x88, 0x6e, 0x45, 0x56, 0xb3, 0x19, 0x7a, 0xba, 0xce, 0x82, 0x25, 0xbe,
    0x3d, 0xad, 0xba, 0x18, 0x3c, 0xb7, 0x69, 0xaa, 0xe4, 0x54, 0x38, 0xbe,
    0x26, 0x49, 0xaf, 0x42, 0x2e, 0x36, 0xae, 0x10, 0x12, 0x57, 0x90, 0x0a,
    0x10, 0xd9, 0x26, 0xee, 0x8e, 0x25, 0x2e, 0x8d, 0xc6, 0x34, 0xa0, 0x49,
    0x14, 0x45, 0x04, 0x78, 0xd2, 0xfb, 0xfb, 0x4a, 0x9d, 0xdb, 0x44, 0xe1,
    0x72, 0x24, 0x65, 0xa3, 0x70, 0xc6, 0xff, 0x84, 0xe4, 0x7f, 0xc7, 0x3c,
    0xc7, 0xcc, 0x46, 0xf2, 0xa4, 0x97, 0x64, 0x02, 0x3d, 0x5d, 0x33, 0xcd,
    0x2d, 0x5a, 0x6c, 0x91, 0x03, 0x08, 0x68, 0x0f, 0x1a, 0x6c, 0x6c, 0xad,
    0xad, 0x1f, 0x94, 0xcd, 0x32, 0x67, 0x7c, 0x7c, 0x29, 0xb1, 0x37, 0xd1,
    0xec, 0x2f, 0xa9, 0xd8, 0xdf, 0x4b, 0x43, 0x3d, 0x4e, 0x4e, 0x91, 0xaa,
    0xa1, 0xde, 0x00, 0x1b, 0xf1, 0xfe, 0xd5, 0x6b, 0x5d, 0xb6, 0xc4, 0x31,
    0xf0, 0xcc, 0xf2, 0x8f, 0xb3, 0x62, 0x57, 0x0c, 0xe8, 0xb5, 0x57, 0x1e,
    0x5f, 0x16, 0x63, 0x6b, 0x6a, 0x53, 0xc5, 0x61, 0x03, 0x59, 0xbe, 0xbf,
    0xd7, 0xa8, 0xca, 0xd3, 0xfc, 0x22, 0x0b, 0x52, 0x2c, 0x08, 0x8b, 0xef,
    0x24, 0x35, 0x2c, 0xd1, 0x92, 0x4f, 0x43, 0xa8, 0xac, 0x65, 0x7d, 0xe7,
    0x43, 0x26, 0xcd, 0x37, 0x4c, 0xf1, 0xeb, 0x35, 0x15, 0xfb, 0xfa, 0x02,
    0x95, 0x72, 0x94, 0xc3, 0x1a, 0x57, 0x42, 0x64, 0x0c, 0x81, 0x69, 0xa6,
    0x27, 0x05, 0x44, 0x45, 0xbb, 0x88, 0x92, 0x0c, 0xc6, 0x7f, 0xa4, 0xe4,
    0x06, 0xc9, 0xf2, 0xe3, 0x3d, 0x55, 0xb5, 0xd9, 0x84, 0x24, 0x54, 0x34,
    0x5b, 0xbc, 0x4d, 0xa8, 0xe8, 0xd4, 0x35, 0xa3, 0x63, 0x36, 0x27, 0x85,
    0xa3, 0x9a, 0xaa, 0x1b, 0xd6, 0x6c, 0xfc, 0x49, 0x96, 0xd5, 0xcd, 0xad,
    0x85, 0x68, 0xb2, 0x59, 0x36, 0x65, 0x0e, 0x0b, 0x36, 0xc8, 0x32, 0x17,
    0x77, 0x3d, 0x85, 0xa6, 0xe1, 0xec, 0x3e, 0x64, 0x68, 0x0b, 0x6b, 0x89,
    0x24, 0x18, 0xa9, 0x73, 0x84, 0xfe, 0xdf, 0x50, 0x2e, 0xb9, 0x23, 0x94,
    0x31, 0x82, 0xf0, 0x4a, 0x64, 0x4b, 0x2c, 0x68, 0x0c, 0x17, 0x0b, 0x65,
    0x05, 0xf0, 0x96, 0x88, 0x0a, 0x8e, 0x4e, 0x4e, 0x0d, 0x93, 0x1b, 0xe8,
    0x79, 0xa9, 0x29, 0x3d, 0xdb, 0x78, 0x05, 0x3a, 0x56, 0x19, 0xe7, 0xbc,
    0xde, 0xe3, 0x0b, 0xc7, 0xab, 0x10, 0x35, 0x89, 0xc6, 0x24, 0xc8, 0x58,
    0x9c, 0xd2, 0xf8, 0x2a, 0x02, 0x3d, 0xc7, 0xbf, 0xba, 0x24, 0x80, 0xaa,
    0x9c, 0x80, 0x6a, 0x25, 0xa1, 0xd8, 0xf3, 0xbe, 0x69, 0x04, 0x38, 0x92,
    0x07, 0x07, 0x78, 0x63, 0xb1, 0xd1, 0xea, 0xf6, 0x31, 0xd0, 0xae, 0xe8,
    0xdb, 0x67, 0x86, 0x86, 0x6e, 0xbc, 0x6f, 0x1c, 0x5a, 0x1d, 0xe9, 0x83,
    0x83, 0x17, 0xc4, 0x5a, 0x9e, 0xaa, 0xef, 0x59, 0xc2, 0xf8, 0x1b, 0x57,
    0x48, 0x15, 0xa2, 0x85, 0xa5, 0x66, 0x59, 0xb1, 0x74, 0xbc, 0x60, 0x41,
    0x93, 0x84, 0x30, 0xb0, 0x06, 0xb4, 0xde, 0xf5, 0x24, 0x09, 0x6a, 0xa1,
    0x31, 0x21, 0x65, 0x59, 0x80, 0xc5, 0x06, 0xad, 0x0d, 0x25, 0xd7, 0xd3,
    0xa0, 0xbe, 0x2b, 0xd9, 0x5b, 0x4f, 0xa9, 0x56, 0x7a, 0x34, 0x71, 0x7c,
    0xa1, 0xf4, 0x26, 0x8e, 0x67, 0xde, 0x2d, 0x71, 0x0c, 0xa5, 0xea, 0x26,
    0xd7, 0xc5, 0x80, 0x16, 0xa0, 0x5c, 0xa1, 0x07, 0xeb, 0x05, 0x60, 0x03,
    0x78, 0x01, 0x7f, 0x1b, 0x2f, 0x56, 0x1c, 0x34, 0x2d, 0x22, 0x80, 0xbf,
    0xba, 0x15, 0x10, 0xa4, 0xba, 0x42, 0xa5, 0x6d, 0x04, 0x75, 0x90, 0xdf,
    0xdd, 0x04, 0x1e, 0x03, 0x08, 0x9f, 0x30, 0x91, 0xc1, 0xc5, 0x74, 0xd7,
    0x97, 0x64, 0x81, 0xaf, 0x29, 0x28, 0x51, 0xf8, 0x32, 0xcb, 0xc4, 0xc2,
    0xf1, 0x2f, 0xd3, 0x2c, 0xbe, 0x0a, 0x1d, 0x46, 0x70, 0x41, 0xb8, 0x70,
    0xba, 0xf4, 0xe8, 0x5f, 0x81, 0x83, 0xa8, 0x69, 0x10, 0xa9, 0x68, 0xd0,
    0x54, 0xd2, 0x20, 0x09, 0x71, 0x13, 0xa3, 0x68, 0x8a, 0x9e, 0xae, 0x09,
    0x8b, 0xb3, 0x84, 0x7c, 0xf9, 0x74, 0xf2, 0x3a, 0x5b, 0xe6, 0x19, 0x03,
    0x73, 0xa6, 0xf0, 0xca, 0xa9, 0x37, 0xba, 0x92, 0xda, 0x59, 0x50, 0x55,
    0x8e, 0x0c, 0x6e, 0x6f, 0xc3, 0x55, 0x8d, 0xd5, 0x85, 0x07, 0x84, 0x47,
    0x76, 0xbe, 0xf1, 0xbc, 0xd8, 0xc4, 0x28, 0x19, 0x26, 0x66, 0xdb, 0x1a,
    0xa7, 0x04, 0x50, 0x08, 0x56, 0x95, 0x26, 0x75, 0x21, 0x81, 0x2a, 0x94,
    0x44, 0x86, 0x02, 0x1b, 0x9a, 0x2a, 0x51, 0x9c, 0x02, 0x32, 0x4c, 0x69,
    0x5b, 0x43, 0x91, 0x52, 0x83, 0x2c, 0x96, 0x2b, 0x40, 0x99, 0x80, 0x23,
    0x54, 0x0f, 0xb2, 0xb6, 0xbc, 0xc9, 0xc6, 0xfa, 0xd3, 0x1e, 0xa9, 0x2a,
    0xff, 0x7b, 0x06, 0xab, 0x3a, 0xa9, 0xc7, 0x53, 0x7b, 0x92, 0x63, 0x46,
    0xd2, 0x06, 0x94, 0xdf, 0xb9, 0x86, 0xe3, 0x23, 0xc0, 0xf1, 0x11, 0xc5,
    0xf1, 0xb5, 0x0e, 0x33, 0x03, 0xe6, 0x02, 0x4c, 0x70, 0x02, 0x2c, 0x64,
    0xb5, 0x51, 0x96, 0x00, 0xe9, 0x6e, 0x09, 0xcb, 0xc0, 0x83, 0x5c, 0x05,
    0x0b, 0xcc, 0x15, 0xb6, 0xf5, 0x00, 0x06, 0xd4, 0x19, 0xc2, 0xb3, 0xff,
    0xed, 0x60, 0xa0, 0x90, 0xf6, 0xd4, 0xf3, 0x7c, 0x11, 0xed, 0xf4, 0x0d,
    0x4b, 0x22, 0x76, 0x77, 0xdd, 0xa7, 0xae, 0x07, 0xf3, 0xef, 0xc0, 0x5e,
    0xd1, 0x50, 0xb5, 0x6d, 0xc7, 0x42, 0x42, 0x29, 0xdb, 0x81, 0xcb, 0x28,
    0x96, 0xee, 0xf4, 0xc7, 0x0c, 0xa6, 0x81, 0x9e, 0xae, 0x89, 0x81, 0x12,
    0xcd, 0x64, 0x97, 0x13, 0xa4, 0x84, 0x7d, 0xb4, 0xe2, 0x94, 0xcd, 0x11,
    0x15, 0x68, 0xb9, 0xe2, 0x02, 0x5d, 0x12, 0x54, 0x10, 0x35, 0x79, 0x92,
    0x28, 0xad, 0x53, 0x30, 0x55, 0x2b, 0x6e, 0x72, 0x5c, 0x66, 0x02, 0x7b,
    0x0a, 0x13, 0x39, 0xfe, 0x7a, 0x49, 0xc4, 0x22, 0x4b, 0x42, 0xe7, 0xf4,
    0xe3, 0xd9, 0x67, 0xc7, 0x07, 0x3b, 0x44, 0xf8, 0xfb, 0xb3, 0x8f, 0x1f,
    0x02, 0x2e, 0xd9, 0x44, 0x3a, 0xbb, 0x73, 0xd7, 0x95, 0x16, 0x56, 0x94,
    0x5e, 0xe9, 0xf9, 0x57, 0x41, 0x42, 0x52, 0x22, 0x35, 0x96, 0x7a, 0x13,
    0xdf, 0xbb, 0x9e, 0xbf, 0x74, 0x1b, 0x8a, 0x8d, 0x59, 0x56, 0xcc, 0x33,
    0x21, 0x08, 0x70, 0x00, 0xea, 0x54, 0x19, 0x9c, 0x2a, 0x6b, 0xf1, 0xf1,
    0x69, 0x63, 0x83, 0xae, 0x80, 0x59, 0x77, 0x8d, 0x55, 0xe7, 0x24, 0x31,
    0xca, 0x7b, 0xd7, 0x15, 0x01, 0xa8, 0x85, 0x41, 0xfb, 0xd9, 0x30, 0x60,
    0x4e, 0xc8, 0xa4, 0x86, 0xd3, 0x90, 0x4c, 0xf4, 0xf5, 0xf0, 0x6a, 0x45,
    0x20, 0xdd, 0xac, 0x08, 0xa4, 0x9e, 0xe4, 0x8b, 0xba, 0x7a, 0xbd, 0x7a,
    0x6a, 0x7f, 0x85, 0xab, 0xee, 0x33, 0x9f, 0x9a, 0xf9, 0xf1, 0x08, 0x64,
    0x07, 0x51, 0xf6, 0x40, 0xcd, 0x32, 0xdd, 0xc4, 0xad, 0x69, 0x13, 0xac,
    0x22, 0xf7, 0xea, 0x01, 0x86, 0xe3, 0xa5, 0x66, 0xdd, 0xba, 0x75, 0x35,
    0x6e, 0x00, 0x06, 0x49, 0xcb, 0xc9, 0xdb, 0x2a, 0x4a, 0xd9, 0xbf, 0x62,
    0x71, 0x52, 0x7c, 0x49, 0xd2, 0xf1, 0x2b, 0xf9, 0xea, 0x58, 0x69, 0x30,
    0x11, 0x4d, 0xd4, 0x60, 0x3d, 0xd8, 0x2a, 0xe8, 0x92, 0x10, 0x2d, 0xb8,
    0xc6, 0x7e, 0xae, 0x94, 0x03, 0xed, 0x8d, 0x88, 0xa5, 0x2e, 0x94, 0x9a,
    0xcd, 0x8d, 0xbb, 0xda, 0xd0, 0x7c, 0xa3, 0xf6, 0x13, 0x38, 0x2f, 0x59,
    0x71, 0x7c, 0xbc, 0xa7, 0xa6, 0xd2, 0x98, 0x98, 0xda, 0x80, 0x19, 0x25,
    0x69, 0x12, 0x39, 0xea, 0x30, 0x9d, 0xf1, 0x1b, 0xc2, 0x05, 0x65, 0xd2,
    0x8c, 0x65, 0x13, 0xa4, 0xce, 0xec, 0x4d, 0xfd, 0xa7, 0x6b, 0x4a, 0x5c,
    0x5a, 0xc3, 0xc2, 0xd7, 0xc6, 0x34, 0xfc, 0x70, 0x8e, 0x59, 0x6f, 0xe8,
    0x34, 0xa6, 0x20, 0x2f, 0xb7, 0x33, 0x96, 0x76, 0x6b, 0xe0, 0x70, 0x0c,
    0xc8, 0x1c, 0x4b, 0xe9, 0xa2, 0x1e, 0x5a, 0xd5, 0x6b, 0xc0, 0x49, 0x50,
    0x19, 0x11, 0x40, 0x2f, 0x02, 0x8d, 0x74, 0xdd, 0x19, 0x4d, 0x09, 0x77,
    0x90, 0x34, 0x68, 0x80, 0x2d, 0x80, 0x14, 0x91, 0xf3, 0x7a, 0x91, 0x65,
    0x5c, 0x8e, 0x20, 0x5d, 0x95, 0xb0, 0x62, 0x24, 0xc1, 0x5b, 0x87, 0x48,
    0xda, 0x89, 0xf6, 0xc6, 0xc7, 0x30, 0x2f, 0xe8, 0xa6, 0x1e, 0x55, 0xf5,
    0x34, 0x7e, 0xba, 0x4e, 0x89, 0x42, 0xad, 0xa6, 0x4a, 0x67, 0xa1, 0x36,
    0x63, 0x2b, 0x97, 0xd6, 0x5b, 0x90, 0x34, 0x47, 0x9b, 0x96, 0x2c, 0x2f,
    0xc2, 0x99, 0x20, 0xb9, 0x33, 0xfe, 0xc2, 0x09, 0x47, 0x38, 0xcd, 0x17,
    0xf8, 0x92, 0x48, 0x36, 0x4f, 0x91, 0x00, 0x24, 0x6d, 0x55, 0x08, 0xb3,
    0x04, 0xdd, 0x14, 0x38, 0xe7, 0x08, 0x0b, 0x44, 0x28, 0xa8, 0xe0, 0x10,
    0x61, 0x49, 0xd0, 0xe2, 0xa9, 0xbb, 0xe7, 0xaa, 0x0c, 0x1e, 0xce, 0x58,
    0x39, 0x18, 0x68, 0xb0, 0x51, 0x5d, 0x77, 0xce, 0xd4, 0xd4, 0x6d, 0x01,
    0xa0, 0xb1, 0xc5, 0x48, 0x18, 0x54, 0x75, 0xe0, 0x8a, 0x9b, 0xe2, 0x36,
    0x34, 0xbe, 0xcb, 0xb2, 0x1c, 0xad, 0x98, 0xa0, 0x29, 0x02, 0xaf, 0x82,
    0x1c, 0x58, 0x5a, 0x0d, 0x9b, 0xad, 0x8e, 0x33, 0x16, 0x93, 0x76, 0xaf,
    0xb2, 0xac, 0xdd, 0xe5, 0x47, 0x16, 0x93, 0xba, 0x93, 0xdf, 0x0c, 0x60,
    0xca, 0x20, 0x3d, 0x96, 0x4e, 0x08, 0xca, 0xf1, 0xc0, 0xbe, 0xc1, 0xf2,
    0x6d, 0xaf, 0xc8, 0x6e, 0x40, 0x17, 0xd4, 0x84, 0x38, 0xd5, 0x50, 0x3b,
    0x5a, 0xe9, 0x87, 0x06, 0xf8, 0x55, 0x66, 0xa3, 0xfb, 0xfb, 0xca, 0x1a,
    0x03, 0x90, 0x88, 0x0b, 0x8a, 0x7b, 0x72, 0x5e, 0x50, 0x93, 0x95, 0x48,
    0xd4, 0x63, 0x4b, 0x38, 0x53, 0xe6, 0xad, 0xd6, 0x48, 0x3d, 0xd9, 0xb7,
    0x52, 0xd2, 0x69, 0x3d, 0xc0, 0x96, 0x21, 0x02, 0x91, 0x7d, 0xc9, 0x73,
    0x63, 0x3e, 0x94, 0x36, 0x10, 0xd9, 0xa1, 0x96, 0x01, 0xb6, 0x80, 0xa2,
    0x33, 0x7e, 0x05, 0xc6, 0x6a, 0xc2, 0x91, 0xc8, 0x10, 0x4e, 0x53, 0xa4,
    0x8c, 0x6b, 0xfa, 0xd0, 0x2a, 0xaf, 0x90, 0x00, 0x69, 0x55, 0x34, 0x97,
    0x7a, 0xbf, 0xa4, 0xc6, 0x0c, 0xcf, 0x38, 0x7a, 0x95, 0x2c, 0x29, 0xa3,
    0x5c, 0x14, 0x58, 0x80, 0x30, 0x12, 0x4b, 0xa1, 0xbd, 0x96, 0x40, 0xbe,
    0x86, 0x6e, 0xd4, 0x34, 0x9d, 0xb1, 0xb2, 0xe5, 0x75, 0x20, 0xd1, 0xbc,
    0x6e, 0x01, 0x8c, 0x36, 0x02, 0x8e, 0x4f, 0xe5, 0xdf, 0x6d, 0x60, 0x85,
    0x57, 0xb7, 0x0a, 0xaa, 0x54, 0x2f, 0xd2, 0xa7, 0x69, 0x75, 0xdb, 0x01,
    0xaa, 0x57, 0xab, 0x5b, 0xed, 0xd5, 0xb2, 0xad, 0x23, 0x35, 0xdc, 0x2b,
    0x96, 0xbc, 0xea, 0xf6, 0xd8, 0x78, 0xd7, 0xee, 0x5a, 0xcd, 0x0f, 0x7d,
    0x6f, 0x69, 0x24, 0xd0, 0xab, 0xd5, 0xed, 0x63, 0x41, 0xd8, 0xde, 0x28,
    0x98, 0xb9, 0x35, 0x55, 0xf4, 0xa4, 0x0d, 0x9b, 0x72, 0xb5, 0x0a, 0x32,
    0x99, 0x54, 0x5c, 0x39, 0xa0, 0x98, 0x8e, 0x9c, 0x81, 0x83, 0x96, 0xf8,
    0x36, 0x72, 0x8e, 0x2c, 0x50, 0xa5, 0x41, 0x65, 0x3e, 0x9d, 0x4c, 0x06,
    0xa5, 0x84, 0xc0, 0xaf, 0xce, 0xa0, 0xf6, 0x9f, 0x19, 0xd7, 0x2e, 0x37,
    0xdf, 0xb5, 0xa7, 0x51, 0xd7, 0xda, 0x34, 0x99, 0xbe, 0x9e, 0xcc, 0xa0,
    0xdf, 0x6f, 0x4c, 0xa7, 0x6d, 0xc1, 0x9d, 0x4c, 0x06, 0xfd, 0xfe, 0x23,
    0xe7, 0xc5, 0x25, 0xda, 0x7c, 0xbd, 0x00, 0xe1, 0x1d, 0xe5, 0xa4, 0x90,
    0x8a, 0xd3, 0xee, 0xc4, 0x64, 0xb5, 0x87, 0x36, 0xa8, 0x3d, 0x27, 0xcb,
    0x80, 0x3c, 0x99, 0x1c, 0x76, 0x27, 0xd3, 0x60, 0x00, 0x88, 0xc0, 0x34,
    0xe5, 0x2d, 0x26, 0x00, 0xbc, 0x27, 0x58, 0x4c, 0x92, 0x26, 0xf6, 0x11,
    0x74, 0x49, 0xd9, 0x1c, 0x54, 0xfc, 0xab, 0x25, 0x68, 0x5b, 0xc7, 0xbf,
    0xcb, 0xd2, 0x44, 0x22, 0x76, 0xb0, 0x3f, 0x23, 0xf5, 0xfa, 0x78, 0xcf,
    0xbc, 0x7d, 0x1c, 0x73, 0xd1, 0x18, 0x01, 0x08, 0x9c, 0xa3, 0xba, 0xbd,
    0x24, 0x80, 0x9e, 0x91, 0xc2, 0xab, 0x68, 0xc9, 0xdb, 0xbb, 0x22, 0xab,
    0x6e, 0xda, 0x95, 0x43, 0x73, 0x54, 0x87, 0xfd, 0xd6, 0xbe, 0x58, 0xa6,
    0xf4, 0xc9, 0xe4, 0xe0, 0xd1, 0xc7, 0x44, 0xb5, 0xad, 0x1d, 0xb4, 0x28,
    0x72, 0x32, 0x44, 0x19, 0x7c, 0xf8, 0x26, 0x08, 0x92, 0x15, 0xbf, 0x6d,
    0x5a, 0x6d, 0x8b, 0xfe, 0x64, 0x32, 0x3c, 0x7c, 0xec, 0xdc, 0xa4, 0xdb,
    0xd9, 0x18, 0x7c, 0xd3, 0x36, 0xcc, 0x47, 0xbe, 0xfc, 0x0a, 0x30, 0xb7,
    0xe6, 0xa2, 0x1c, 0x09, 0xb6, 0xcc, 0x40, 0xa3, 0xe4, 0x3d, 0x0d, 0x33,
    0x50, 0x68, 0x94, 0x47, 0x15, 0x37, 0x7b, 0xd7, 0x50, 0x90, 0x2a, 0xad,
    0xaa, 0xe2, 0x0e, 0x95, 0xde, 0xdc, 0x5d, 0xcb, 0xb2, 0x90, 0xf8, 0xd2,
    0x45, 0x0a, 0xbc, 0x87, 0x2a, 0xdb, 0xb2, 0xd4, 0x69, 0xc8, 0xd7, 0x95,
    0x85, 0x19, 0x84, 0x5b, 0x59, 0xf2, 0x28, 0x9d, 0xea, 0x91, 0x32, 0xba,
    0x70, 0xa9, 0x3c, 0x55, 0xcd, 0xa4, 0xf2, 0xaa, 0x96, 0xff, 0x4c, 0xf5,
    0x71, 0x1f, 0xaa, 0xe0, 0x24, 0xe9, 0x29, 0x33, 0x8d, 0xa7, 0xb5, 0xf1,
    0xaf, 0x84, 0x28, 0xe8, 0xe5, 0x4a, 0x10, 0xd7, 0x49, 0x28, 0x97, 0x73,
    0x00, 0xbd, 0x82, 0x6e, 0x14, 0x1d, 0x79, 0x75, 0xcf, 0x52, 0x30, 0xe8,
    0xaa, 0x64, 0x37, 0x2c, 0x30, 0x1a, 0x4f, 0x37, 0x5e, 0x36, 0xbd, 0x30,
    0x30, 0x9b, 0xa9, 0x43, 0x95, 0x05, 0x8a, 0xce, 0x3a, 0x28, 0xcb, 0x89,
    0xa5, 0x6a, 0x34, 0x77, 0xca, 0xa6, 0x87, 0xaa, 0xbd, 0x7e, 0xe5, 0x6c,
    0x7a, 0xa7, 0xb9, 0xfe, 0x63, 0x2a, 0x81, 0xa2, 0x69, 0x91, 0x70, 0x9a,
    0x55, 0xeb, 0xe2, 0x22, 0x03, 0x43, 0x1b, 0x5d, 0xce, 0x9b, 0xd4, 0x5f,
    0x0a, 0x84, 0xc8, 0x54, 0x1b, 0x21, 0xa9, 0x99, 0x27, 0x1c, 0xdd, 0x2c,
    0x80, 0xc3, 0x06, 0x5d, 0xa4, 0x36, 0x48, 0x2c, 0xf1, 0x15, 0x91, 0x58,
    0xe1, 0xb2, 0x20, 0xf8, 0xca, 0x91, 0x66, 0x7c, 0x6d, 0xc5, 0x6b, 0xe8,
    0x7d, 0xa7, 0xaa, 0xc7, 0xa7, 0x6b, 0xf6, 0xfd, 0xa0, 0x9c, 0x5a, 0xa6,
    0x3c, 0x43, 0x83, 0x8d, 0xe9, 0x90, 0x04, 0xf3, 0x9c, 0x66, 0xca, 0x41,
    0x6c, 0x32, 0x55, 0xea, 0x31, 0xf4, 0x74, 0x7d, 0x3d, 0x71, 0x8c, 0x99,
    0x0d, 0x09, 0x52, 0x2c, 0x41, 0x65, 0xef, 0x84, 0x0e, 0x46, 0x39, 0x65,
    0x60, 0xe8, 0xfd, 0xec, 0xaa, 0x86, 0x46, 0xeb, 0x4b, 0xa4, 0x32, 0x58,
    0xfa, 0x4a, 0x63, 0xca, 0x04, 0xa6, 0x8c, 0x24, 0xce, 0xc4, 0x79, 0x5f,
    0x3d, 0x20, 0x7e, 0x43, 0x45, 0xbc, 0x70, 0x42, 0xe7, 0x7d, 0x06, 0x76,
    0x1c, 0x30, 0x28, 0x29, 0x85, 0xa9, 0xd3, 0x56, 0x01, 0x37, 0xf6, 0x5a,
    0xe0, 0xb9, 0x14, 0x83, 0x82, 0x84, 0x5c, 0x66, 0x2b, 0x16, 0x4b, 0x23,
    0x2b, 0x90, 0x01, 0xb4, 0xe4, 0xc8, 0x94, 0x55, 0x8c, 0x85, 0x39, 0x4a,
    0x73, 0xb4, 0x16, 0x96, 0x54, 0x27, 0x01, 0xb2, 0x71, 0x85, 0x24, 0x37,
    0x56, 0xc0, 0x9c, 0xc6, 0xdc, 0xaa, 0x52, 0x09, 0x6a, 0x20, 0x3f, 0x58,
    0x58, 0x40, 0x1d, 0xb8, 0x12, 0x09, 0x96, 0xf8, 0x56, 0x41, 0x71, 0xe4,
    0xec, 0x0f, 0x1a, 0xcc, 0xdf, 0xb6, 0xf3, 0xd8, 0x80, 0x81, 0xac, 0xa1,
    0xe4, 0xfe, 0x9f, 0xb4, 0x37, 0x5f, 0xba, 0x31, 0xca, 0xfd, 0xb7, 0x39,
    0x22, 0x35, 0x0f, 0x38, 0x8b, 0x0e, 0x43, 0xd4, 0x1b, 0x38, 0x63, 0xfb,
    0x44, 0xd5, 0x75, 0x04, 0x6f, 0x29, 0xb9, 0xe9, 0x9a, 0xdd, 0x78, 0xba,
    0x3e, 0xdb, 0x2e, 0x65, 0x53, 0x29, 0x5c, 0x1a, 0x20, 0x89, 0x68, 0x57,
    0xb8, 0xfc, 0xac, 0xe4, 0x9d, 0x6f, 0x90, 0x2f, 0xad, 0x85, 0x1a, 0x3b,
    0x1b, 0xe0, 0xd1, 0xee, 0xa2, 0xa4, 0xd9, 0xbf, 0xbd, 0xa8, 0xa5, 0x01,
    0x1f, 0x67, 0xdc, 0x86, 0xa4, 0x6d, 0x9c, 0x9a, 0x05, 0x92, 0xdb, 0x41,
    0xb5, 0xb9, 0xae, 0x0e, 0xe0, 0x7e, 0x95, 0x3b, 0xb3, 0x56, 0xf5, 0x46,
    0x83, 0x25, 0x72, 0x97, 0xdc, 0xeb, 0x80, 0x8c, 0x01, 0xda, 0xcd, 0x6c,
    0x87, 0x21, 0x1f, 0xc3, 0x26, 0xf5, 0xe8, 0xc2, 0xff, 0x83, 0x10, 0x64,
    0x60, 0x3a, 0xcf, 0x52, 0x5c, 0x48, 0xe6, 0xac, 0x0b, 0xb9, 0x71, 0x25,
    0xb2, 0x80, 0xc7, 0xf1, 0x65, 0x76, 0xab, 0x36, 0x87, 0x65, 0x05, 0x5c,
    0xc6, 0xbb, 0xd7, 0x69, 0xc6, 0xc1, 0xcd, 0x44, 0xfb, 0x23, 0xcb, 0x6d,
    0x41, 0x7b, 0x63, 0xf4, 0x41, 0xbf, 0x47, 0xb1, 0xac, 0xb0, 0x69, 0x0a,
    0x5d, 0x5c, 0xa9, 0xed, 0x18, 0xcd, 0x7b, 0x85, 0x90, 0xb1, 0xb4, 0xa8,
    0x79, 0x68, 0x74, 0x50, 0xdd, 0x7c, 0xc2, 0x85, 0xa6, 0x20, 0x0a, 0x83,
    0xd7, 0x05, 0x1a, 0x8d, 0x2b, 0x66, 0x46, 0x64, 0x52, 0x6e, 0xaa, 0xec,
    0x2f, 0x8f, 0x1f, 0x41, 0x59, 0x76, 0x90, 0x52, 0xa3, 0x35, 0x86, 0xb2,
    0x8b, 0xcc, 0x60, 0x2d, 0x33, 0x8f, 0xbd, 0xe0, 0x86, 0x98, 0xdd, 0xe0,
    0x01, 0x37, 0x2b, 0x82, 0xa4, 0xa6, 0xe0, 0xaf, 0x2e, 0xf3, 0x9d, 0x8c,
    0x29, 0xfd, 0x8f, 0xe3, 0x6f, 0x01, 0xcb, 0x77, 0xa0, 0x77, 0x03, 0x0a,
    0xf0, 0x3f, 0xff, 0x5d, 0x11, 0x01, 0xb9, 0xf3, 0xdc, 0x01, 0xcb, 0x3c,
    0xe1, 0x7c, 0xc3, 0x1b, 0x1f, 0xcc, 0x17, 0xaa, 0x63, 0xaf, 0x34, 0x23,
    0xcd, 0x66, 0x8f, 0x1b, 0x6a, 0x36, 0xb3, 0x7b, 0x04, 0x4a, 0x09, 0x43,
    0x19, 0x37, 0xf8, 0xce, 0x2b, 0x18, 0xcb, 0x74, 0xed, 0x95, 0x8d, 0xa5,
    0xdb, 0xbc, 0x70, 0xc5, 0xd8, 0x58, 0x18, 0xa2, 0x43, 0xef, 0x1f, 0x36,
    0xd1, 0xa8, 0xe3, 0xd8, 0x6a, 0xa8, 0x61, 0xd1, 0x78, 0xcd, 0x02, 0x88,
    0x6f, 0x20, 0x4c, 0xbc, 0x21, 0x33, 0xbc, 0x4a, 0x85, 0x5b, 0xbb, 0x59,
    0x57, 0x76, 0xfb, 0xa6, 0x1d, 0x47, 0x62, 0x59, 0x6f, 0x64, 0xd8, 0xa9,
    0x73, 0x7a, 0x11, 0x81, 0x0b, 0xb6, 0xf5, 0xec, 0x6b, 0x7e, 0x29, 0xdc,
    0x19, 0xf8, 0x80, 0x06, 0x43, 0x20, 0x95, 0x7e, 0x7d, 0x1d, 0xa5, 0x5f,
    0xa9, 0xd9, 0xee, 0xf0, 0x9d, 0xeb, 0xf9, 0xd5, 0x7e, 0xc0, 0x53, 0x09,
    0x4a, 0x65, 0xff, 0xd2, 0x05, 0xad, 0xeb, 0x63, 0xd7, 0x5b, 0x43, 0x7a,
    0x6b, 0xb5, 0x95, 0x41, 0xba, 0xb3, 0x1a, 0x68, 0xa2, 0xd7, 0x02, 0x5b,
    0x62, 0xb9, 0xed, 0x47, 0x10, 0x23, 0xc5, 0x03, 0x05, 0xb6, 0x51, 0x14,
    0xf5, 0x77, 0x77, 0x5d, 0xe5, 0x58, 0xa0, 0xea, 0xbc, 0xc6, 0xb9, 0x58,
    0x15, 0xc4, 0xe5, 0x81, 0x6e, 0x74, 0x92, 0x78, 0xfe, 0x07, 0x97, 0xf9,
    0x52, 0x7b, 0xef, 0x95, 0xd5, 0x0e, 0x82, 0x25, 0x4c, 0x97, 0x0f, 0x1a,
    0xa3, 0xac, 0xf2, 0x88, 0xfa, 0xd6, 0x73, 0x0c, 0xf2, 0x4d, 0xaa, 0xcb,
    0xd2, 0x8c, 0x8b, 0xaa, 0x5c, 0x8e, 0xa4, 0x5f, 0x5c, 0x91, 0x3b, 0x33,
    0xbd, 0x1d, 0xae, 0x19, 0xf5, 0xdd, 0xdd, 0x73, 0x07, 0x39, 0xbe, 0xf3,
    0x96, 0xc9, 0x80, 0x8e, 0x40, 0xc7, 0x21, 0x71, 0x97, 0x07, 0x57, 0xe4,
    0xce, 0xdb, 0xdd, 0xad, 0xe6, 0x65, 0xba, 0xa8, 0xc6, 0x86, 0xb8, 0x81,
    0x88, 0x96, 0x9e, 0x6f, 0xb9, 0x93, 0xac, 0x15, 0x9b, 0x48, 0x80, 0x4d,
    0x5c, 0x63, 0xe9, 0x36, 0x42, 0x14, 0x59, 0x99, 0x7a, 0x00, 0x37, 0x52,
    0x3a, 0x94, 0xeb, 0x5a, 0x6b, 0xed, 0xbf, 0x3c, 0x28, 0xff, 0xdc, 0xba,
    0x97, 0xd6, 0xc5, 0xb9, 0xa8, 0x7a, 0x66, 0x76, 0x7f, 0x12, 0x9d, 0x82,
    0xf6, 0xb5, 0xdd, 0xe9, 0x5b, 0xc9, 0x97, 0xfb, 0xcd, 0x8a, 0x4a, 0x17,
    0xda, 0xae, 0x1a, 0x83, 0xef, 0x4c, 0xb3, 0xa2, 0x12, 0xe9, 0x37, 0xf7,
    0x59, 0x07, 0x0f, 0x34, 0x9a, 0x48, 0xfd, 0x10, 0x78, 0xc3, 0x04, 0x19,
    0x53, 0xb8, 0xcb, 0x95, 0x41, 0x27, 0x9d, 0x3a, 0x4a, 0x87, 0x34, 0x6d,
    0x72, 0xfc, 0x54, 0x45, 0xa6, 0x34, 0xb5, 0x46, 0xbe, 0x1a, 0x50, 0x1a,
    0x0d, 0x6c, 0x5f, 0x8e, 0x4a, 0x28, 0x79, 0x2b, 0xad, 0x69, 0x15, 0x60,
    0xda, 0xea, 0x75, 0x9f, 0x46, 0x85, 0x3b, 0xb5, 0xb6, 0xc7, 0xe7, 0x55,
    0x81, 0x59, 0x9d, 0x1f, 0x47, 0xda, 0x36, 0x11, 0xd2, 0x9d, 0x28, 0xd2,
    0x3e, 0xe3, 0x52, 0xe7, 0x1a, 0x52, 0x63, 0x2b, 0x50, 0xc1, 0x4a, 0xf7,
    0xf7, 0xb4, 0x63, 0x3c, 0xf0, 0x2b, 0x1d, 0xa9, 0xaa, 0x5d, 0x47, 0x3e,
    0x99, 0xda, 0xcd, 0x78, 0x27, 0xed, 0x41, 0xde, 0x0c, 0xaf, 0x6a, 0xc5,
    0x53, 0xd9, 0xd1, 0x53, 0xcd, 0xe6, 0x16, 0x40, 0x52, 0xcf, 0x97, 0x3b,
    0xa9, 0x86, 0xb5, 0x62, 0x9e, 0xb4, 0x4b, 0x7a, 0x23, 0x58, 0xa9, 0x8a,
    0x4a, 0x6a, 0x05, 0x20, 0x75, 0xc2, 0x8d, 0x9a, 0x03, 0xe0, 0xd5, 0xed,
    0x3f, 0xa2, 0x9f, 0xdd, 0x5d, 0x6e, 0xb4, 0x5c, 0xb5, 0x53, 0xbc, 0x9a,
    0x77, 0xb3, 0x1f, 0x5e, 0x6d, 0x63, 0x35, 0xa6, 0xde, 0x45, 0x33, 0xb0,
    0x52, 0x43, 0x3c, 0x66, 0x52, 0xcd, 0x95, 0x80, 0x76, 0xe0, 0xe1, 0x9e,
    0x8d, 0x7c, 0xff, 0x70, 0x2d, 0x10, 0xa1, 0xbf, 0x79, 0xf4, 0x72, 0xb4,
    0x3d, 0xec, 0x74, 0x7a, 0xde, 0xb2, 0xf3, 0xb0, 0xd2, 0xb9, 0x40, 0xe7,
    0xb5, 0x1a, 0xe0, 0x62, 0x5a, 0x23, 0x60, 0xe9, 0x9f, 0x6d, 0x8c, 0x9d,
    0xf1, 0x79, 0x5e, 0x21, 0x5f, 0x55, 0xd3, 0xba, 0x14, 0x31, 0x51, 0xb7,
    0xc2, 0xc6, 0x2a, 0xd5, 0xcb, 0x3f, 0x29, 0xa3, 0x94, 0x57, 0xb9, 0xb9,
    0x35, 0xee, 0x8c, 0xbe, 0x22, 0xb4, 0xbe, 0xcd, 0xda, 0x6f, 0x3f, 0x08,
    0x02, 0xe6, 0xcb, 0xc0, 0x0a, 0x53, 0x41, 0x5f, 0xaa, 0xca, 0xb4, 0x67,
    0xca, 0x0d, 0x8a, 0xb1, 0x82, 0x27, 0xcc, 0x2b, 0x59, 0x32, 0xf5, 0xcc,
    0x3d, 0x30, 0xc5, 0xea, 0x71, 0xea, 0x19, 0xe8, 0x35, 0xe5, 0xd5, 0x2d,
    0xad, 0x03, 0x2b, 0x34, 0xe5, 0x31, 0x35, 0xf0, 0xea, 0x16, 0x8c, 0xa9,
    0x9d, 0x60, 0x8b, 0x56, 0xb5, 0xea, 0x3d, 0x54, 0xb6, 0x83, 0x30, 0x5a,
    0xf5, 0xe0, 0x15, 0x54, 0xb1, 0x23, 0x33, 0x5a, 0x55, 0xe0, 0x15, 0x54,
    0xe9, 0x04, 0x6b, 0x6c, 0x18, 0x12, 0xde, 0x40, 0x5d, 0x1d, 0xc3, 0xd1,
    0xaa, 0x01, 0xa5, 0xd2, 0x12, 0x5c, 0x85, 0x75, 0x70, 0xf0, 0x6b, 0x64,
    0xc4, 0xe5, 0xb6, 0x7d, 0x33, 0xa9, 0x0f, 0xd3, 0x72, 0x1a, 0xd0, 0xfc,
    0x81, 0xb8, 0xf0, 0xa9, 0xe4, 0x18, 0xc8, 0x84, 0x55, 0x4c, 0x57, 0xc8,
    0x6a, 0x9e, 0xa8, 0xf4, 0xe3, 0xc8, 0xfd, 0xa3, 0x32, 0x8a, 0x7a, 0x93,
    0xc9, 0x69, 0x91, 0x2d, 0x29, 0x27, 0x41, 0x41, 0x78, 0x96, 0x5e, 0x83,
    0x5a, 0x3e, 0x50, 0xa6, 0x55, 0x49, 0x85, 0x4a, 0x2f, 0x10, 0x0b, 0xc2,
    0x5c, 0x69, 0x82, 0x96, 0x25, 0x4d, 0xd3, 0xaf, 0x82, 0xd3, 0x3d, 0x98,
    0xf0, 0xa3, 0xcc, 0xbe, 0x54, 0x22, 0xee, 0xd1, 0x1f, 0xb5, 0xd7, 0x43,
    0xec, 0xf9, 0xb1, 0x1e, 0x2e, 0x8f, 0xc6, 0x4b, 0x37, 0x57, 0x14, 0xbe,
    0x5a, 0xe9, 0x07, 0x03, 0x99, 0xc4, 0x72, 0x15, 0x94, 0xec, 0x1e, 0x65,
    0xdc, 0x95, 0x58, 0x0e, 0x94, 0x94, 0xde, 0x4e, 0x14, 0x31, 0xc9, 0x45,
    0x74, 0x1d, 0x2e, 0x4d, 0x1d, 0x20, 0x7c, 0xa4, 0xe5, 0x2d, 0x21, 0x03,
    0x3c, 0x29, 0x9b, 0xff, 0xfa, 0xb7, 0xff, 0x42, 0x1a, 0x65, 0x39, 0xa1,
    0x63, 0x33, 0xf1, 0x8e, 0x2f, 0x37, 0x9b, 0xd9, 0x93, 0x7a, 0xea, 0x7a,
    0xeb, 0x4a, 0xdb, 0x55, 0x91, 0x76, 0x15, 0x4f, 0x29, 0x7d, 0x14, 0x2b,
    0x8d, 0xd6, 0xfd, 0xfd, 0xce, 0x43, 0x1e, 0xa3, 0x44, 0x89, 0xeb, 0x53,
    0xaf, 0xe9, 0x83, 0x59, 0x31, 0x53, 0x85, 0xa9, 0x05, 0x5c, 0xde, 0xd4,
    0xb3, 0x98, 0x42, 0xa2, 0x98, 0x42, 0xa1, 0x1c, 0x62, 0x8b, 0x66, 0x6f,
    0x3a, 0x56, 0xcb, 0x92, 0xf1, 0x09, 0xc8, 0xf8, 0x9a, 0x57, 0x3c, 0xee,
    0x4f, 0x80, 0x5f, 0x0c, 0x99, 0x0f, 0x6c, 0x47, 0xdd, 0x56, 0x31, 0x21,
    0x36, 0x1b, 0xd9, 0x9e, 0x85, 0x79, 0x35, 0xf5, 0x3c, 0x29, 0xf1, 0xf9,
    0x4d, 0xc9, 0x2c, 0xfc, 0xb9, 0x9a, 0x46, 0x0c, 0x17, 0xd7, 0x80, 0xde,
    0x9f, 0x5c, 0x62, 0x8b, 0x15, 0xa2, 0x96, 0x04, 0x2c, 0xbe, 0x54, 0x55,
    0xaa, 0x25, 0x02, 0x61, 0xf3, 0xf0, 0xa5, 0xb5, 0xf9, 0xb9, 0x1d, 0xe3,
    0x02, 0x61, 0x7b, 0x67, 0x44, 0x74, 0x5d, 0x36, 0xcc, 0xe1, 0x28, 0x55,
    0xa3, 0x74, 0x89, 0x30, 0x4a, 0x46, 0xe9, 0x46, 0x6a, 0x2b, 0x99, 0x3a,
    0x51, 0x88, 0x53, 0x29, 0x75, 0x42, 0x8c, 0x45, 0xa8, 0x4d, 0x90, 0x0f,
    0xe9, 0x9f, 0x6a, 0x25, 0xc8, 0x54, 0xfa, 0x5b, 0x08, 0xed, 0x1c, 0x22,
    0x95, 0x51, 0x9b, 0xfa, 0xb6, 0x34, 0x55, 0x94, 0x23, 0xcc, 0x39, 0x9d,
    0x83, 0x54, 0xbf, 0x04, 0x05, 0xb8, 0x58, 0x60, 0x86, 0x32, 0xb9, 0xc7,
    0xaa, 0x2f, 0x70, 0x8b, 0xd5, 0xd5, 0x7d, 0x5b, 0xe4, 0x3e, 0x1e, 0xf4,
    0xc1, 0x97, 0xb2, 0x2e, 0x18, 0x0f, 0xc9, 0xfe, 0x83, 0x2b, 0x31, 0x55,
    0x2b, 0xdf, 0x8e, 0x41, 0xff, 0xd7, 0xbf, 0xfd, 0x27, 0x48, 0xf5, 0xe0,
    0x11, 0xe9, 0x59, 0x7b, 0xc8, 0x50, 0x36, 0x3b, 0xaf, 0x05, 0x36, 0x5b,
    0x9e, 0xba, 0x90, 0xbb, 0xc7, 0xa4, 0x69, 0xbf, 0x62, 0x8e, 0x76, 0x77,
    0x2d, 0x3f, 0xf9, 0x80, 0x67, 0x4b, 0x62, 0xb9, 0x2c, 0xb1, 0xda, 0xd6,
    0xfe, 0xe0, 0xf4, 0x62, 0x65, 0xe5, 0x6e, 0x45, 0x3a, 0x65, 0x05, 0x22,
    0x38, 0x5e, 0xe8, 0x30, 0x74, 0xb5, 0x29, 0x7a, 0xf8, 0x36, 0x2b, 0xc6,
    0x36, 0x3b, 0x73, 0x78, 0xbb, 0xbb, 0x3b, 0xcc, 0xb2, 0xb7, 0x3f, 0x6a,
    0x0e, 0xbc, 0x6d, 0xd3, 0x9f, 0x7a, 0x65, 0xa9, 0x43, 0x49, 0xe7, 0x2a,
    0x30, 0xda, 0xe8, 0x63, 0xf1, 0x35, 0xe9, 0xa9, 0x90, 0x88, 0xb6, 0xef,
    0x5c, 0x9d, 0x61, 0xe1, 0xd2, 0x55, 0x84, 0xf7, 0xe3, 0xf7, 0xdf, 0xfb,
    0x37, 0x94, 0x25, 0xd9, 0x4d, 0x10, 0xa7, 0x04, 0x17, 0x26, 0x2e, 0xf3,
    0xc6, 0xf3, 0xe7, 0xae, 0xf3, 0x85, 0x49, 0x0f, 0x41, 0xa4, 0x78, 0x6b,
    0xee, 0x78, 0xfe, 0x4d, 0xa4, 0x6b, 0x5b, 0x31, 0x9c, 0xbf, 0xf8, 0x47,
    0x10, 0xa5, 0xd9, 0x72, 0x09, 0xfa, 0x45, 0x79, 0x47, 0xdf, 0x68, 0xc7,
    0x6e, 0xff, 0x95, 0xb7, 0xde, 0xdc, 0x78, 0xd8, 0xef, 0x1b, 0xfa, 0x5d,
    0x82, 0x23, 0x0f, 0x4c, 0x0b, 0xae, 0x94, 0xf6, 0x3b, 0x9a, 0x43, 0x9c,
    0xf0, 0x32, 0x07, 0x47, 0x9c, 0x2a, 0x88, 0x07, 0x49, 0x66, 0x82, 0x57,
    0x91, 0xb8, 0xe5, 0xab, 0x3a, 0x27, 0x82, 0x88, 0x3e, 0x8e, 0xe6, 0xae,
    0x73, 0x86, 0xaf, 0x29, 0x9b, 0x07, 0x41, 0xe0, 0x78, 0xa3, 0xae, 0x7b,
    0x90, 0xb2, 0x2a, 0x3e, 0xce, 0x2b, 0x48, 0x5d, 0xdc, 0xd0, 0xdc, 0x60,
    0xc9, 0xe5, 0x83, 0x01, 0xf3, 0xe3, 0xee, 0xae, 0x1a, 0x47, 0xba, 0xe8,
    0x56, 0x93, 0xfd, 0x44, 0x44, 0x71, 0x07, 0x3e, 0x4b, 0xb0, 0x75, 0x5b,
    0x77, 0x6c, 0x9f, 0xec, 0x7b, 0x95, 0x93, 0xf7, 0x2b, 0xe9, 0xe4, 0x5d,
    0xed, 0xdc, 0x0f, 0xae, 0xb7, 0x3e, 0x8b, 0xb2, 0xe0, 0x32, 0xc3, 0x45,
    0xf2, 0x1a, 0xe7, 0xf8, 0x92, 0xa6, 0x54, 0x50, 0x08, 0xf8, 0xc0, 0x69,
    0x9a, 0xdd, 0x90, 0x44, 0x22, 0xd1, 0x53, 0xca, 0xf8, 0x64, 0x72, 0xe6,
    0x5f, 0x6f, 0xae, 0x2a, 0x67, 0xfb, 0x3b, 0x5c, 0x24, 0x10, 0xb9, 0x03,
    0x30, 0x48, 0x79, 0x06, 0x5e, 0xc7, 0x89, 0xf2, 0xb9, 0x53, 0x21, 0x33,
    0x29, 0x49, 0x7a, 0x1a, 0x0b, 0x6d, 0x32, 0x41, 0x18, 0xbd, 0x96, 0x9f,
    0xe9, 0x50, 0xe3, 0x77, 0x24, 0x99, 0x54, 0x68, 0x6b, 0x6b, 0x3f, 0xba,
    0x59, 0xb4, 0xa9, 0x15, 0x38, 0x44, 0x5a, 0x8d, 0x14, 0x97, 0xca, 0x64,
    0x80, 0x59, 0xa3, 0x7a, 0xfd, 0xc2, 0x36, 0x6c, 0x7a, 0xfe, 0x8f, 0xae,
    0xf4, 0xa9, 0x54, 0x51, 0x33, 0xd0, 0x46, 0x07, 0xcc, 0xe8, 0x38, 0x1a,
    0xf9, 0xd2, 0x04, 0xa4, 0xd9, 0xaf, 0x4d, 0x99, 0xac, 0x70, 0x43, 0x67,
    0xb4, 0xc7, 0x39, 0x4d, 0xec, 0x1a, 0x50, 0x78, 0xc6, 0x69, 0x52, 0xd7,
    0x68, 0x3a, 0x6d, 0xae, 0x5c, 0xc7, 0x04, 0x5b, 0xf5, 0xac, 0x10, 0x3e,
    0xab, 0x07, 0xf3, 0xfa, 0x75, 0xf5, 0xf6, 0xfe, 0x5e, 0xf6, 0xe4, 0x78,
    0xfe, 0xbf, 0xbb, 0x9e, 0x7f, 0xeb, 0x7a, 0xfe, 0x4f, 0x3a, 0xfe, 0x03,
    0xbc, 0x00, 0x5b, 0x37, 0xe6, 0x3d, 0x10, 0xf1, 0x68, 0x23, 0xa0, 0x7a,
    0xfe, 0xbf, 0xb9, 0x9e, 0xff, 0x83, 0x6b, 0x1c, 0xd1, 0x3e, 0xd9, 0xbc,
    0xf2, 0xbf, 0x49, 0xe2, 0x6f, 0x05, 0xaf, 0xd4, 0x0f, 0xb5, 0x2b, 0xde,
    0x37, 0x07, 0xaf, 0x2c, 0x71, 0x1c, 0x56, 0xad, 0xc1, 0x45, 0xd6, 0x87,
    0x0e, 0xac, 0x22, 0x78, 0xf4, 0x21, 0xb3, 0x8d, 0x55, 0x06, 0x8f, 0x3e,
    0xce, 0xa9, 0x8e, 0x14, 0xb3, 0xde, 0xd4, 0x85, 0xbe, 0x71, 0x92, 0xb5,
    0xde, 0x9a, 0x22, 0x19, 0x25, 0xe3, 0x67, 0x9d, 0x18, 0x68, 0xcb, 0xa5,
    0xb0, 0x8e, 0x7f, 0x86, 0xad, 0xb7, 0x22, 0x18, 0x22, 0x08, 0x9d, 0xad,
    0x36, 0xe5, 0x47, 0x8b, 0x26, 0xff, 0xbc, 0x19, 0x4e, 0x47, 0x78, 0x1b,
    0x28, 0x7a, 0x81, 0x31, 0xbe, 0x45, 0x3b, 0xa2, 0x05, 0xe6, 0xda, 0xb1,
    0xa2, 0x85, 0x59, 0xc1, 0xdd, 0xbf, 0xd8, 0xda, 0x5d, 0xf9, 0xdd, 0x34,
    0x74, 0x3e, 0xce, 0x66, 0x4e, 0x3d, 0xbf, 0x9f, 0xac, 0xf9, 0xfd, 0xce,
    0x2d, 0x2c, 0xa0, 0xf5, 0xe4, 0xc4, 0xcc, 0x63, 0x4f, 0x4a, 0xd7, 0xe4,
    0xa6, 0x3b, 0x60, 0x15, 0x81, 0x29, 0xac, 0xc0, 0x4b, 0xa9, 0x14, 0x42,
    0x18, 0xc9, 0x02, 0x99, 0xcf, 0xc9, 0x1a, 0xf3, 0xdf, 0x1b, 0xb6, 0x51,
    0x0d, 0xb5, 0xe7, 0x45, 0x75, 0x9b, 0xbc, 0x8b, 0xd1, 0xca, 0x75, 0x68,
    0xae, 0x8c, 0x0a, 0xe0, 0x18, 0x9d, 0x25, 0xea, 0xc2, 0xd0, 0xbc, 0x57,
    0xc5, 0x81, 0x4a, 0x0e, 0x00, 0x7e, 0x9a, 0x37, 0x8c, 0x88, 0x25, 0xe6,
    0x57, 0xd2, 0x63, 0x5a, 0xfd, 0x34, 0x6f, 0xe6, 0x58, 0x90, 0x1b, 0x7c,
    0x07, 0x6f, 0xf4, 0x4f, 0xf3, 0x26, 0x61, 0xb2, 0xa7, 0x84, 0xd9, 0x6a,
    0x92, 0x5b, 0x6b, 0x7e, 0xd6, 0xac, 0x00, 0x79, 0xc9, 0x5b, 0xb4, 0xbb,
    0xbb, 0x53, 0xd8, 0x97, 0xd7, 0xb0, 0x95, 0x26, 0xf5, 0x01, 0x58, 0x54,
    0xf3, 0x3c, 0xbd, 0xeb, 0x99, 0x20, 0x3a, 0xeb, 0x1c, 0x85, 0x2f, 0x4a,
    0xc8, 0x7e, 0x20, 0x93, 0x5b, 0x29, 0x44, 0xe7, 0x78, 0x0d, 0x07, 0xf6,
    0x6a, 0xe8, 0x8a, 0x3f, 0x36, 0x3f, 0x34, 0x93, 0x5c, 0xeb, 0x11, 0xa1,
    0x13, 0x98, 0x55, 0x0a, 0x47, 0xec, 0x4c, 0xa4, 0x80, 0xed, 0x84, 0xfa,
    0x71, 0xf4, 0xc8, 0xf6, 0xc2, 0x97, 0x87, 0xb4, 0x2d, 0x73, 0xc4, 0x67,
    0xa8, 0xe4, 0xf8, 0xc2, 0x2b, 0x01, 0x1c, 0x12, 0x1d, 0xc9, 0x60, 0xcd,
    0xb9, 0x96, 0x7f, 0x24, 0x7d, 0x73, 0x75, 0x08, 0x08, 0x6f, 0xbb, 0xd4,
    0x9b, 0xa6, 0x77, 0x3d, 0x9d, 0xcf, 0xa1, 0x05, 0x4a, 0xce, 0x19, 0xc1,
    0x45, 0xbc, 0x00, 0x92, 0x05, 0x4e, 0x4a, 0x1a, 0x72, 0xd4, 0x0e, 0xfe,
    0xfa, 0xb7, 0xff, 0x72, 0x46, 0x9b, 0x63, 0x9a, 0xaa, 0x19, 0xb5, 0x88,
    0x68, 0xe9, 0x8d, 0x1e, 0x31, 0xa8, 0xb4, 0x9c, 0xeb, 0x09, 0x57, 0x16,
    0xf4, 0x19, 0x04, 0xad, 0xc2, 0x0b, 0x51, 0xac, 0x58, 0x0c, 0xb4, 0x6a,
    0xe2, 0x20, 0x57, 0x3a, 0x11, 0x56, 0x25, 0x9e, 0x0e, 0xe0, 0x6d, 0xac,
    0xd7, 0xb6, 0x81, 0x9b, 0x4e, 0x75, 0x78, 0x52, 0xc3, 0x6f, 0xf5, 0xa1,
    0xf8, 0xa2, 0x2d, 0x11, 0x42, 0x34, 0xbe, 0x32, 0x51, 0x42, 0xe6, 0x2f,
    0xcd, 0x1b, 0xb1, 0x42, 0xb5, 0x09, 0x53, 0x19, 0x7a, 0xab, 0xd8, 0x21,
    0xa8, 0xd7, 0x89, 0x1c, 0x52, 0x45, 0x26, 0xa2, 0x56, 0x97, 0x9b, 0xc7,
    0xad, 0x91, 0x3e, 0x76, 0x64, 0x8e, 0x9a, 0xa8, 0x31, 0x9d, 0xca, 0x2e,
    0x95, 0xf5, 0xb7, 0x76, 0x95, 0x3a, 0xd1, 0x21, 0x12, 0x2a, 0x2c, 0x02,
    0xec, 0x98, 0x55, 0xdc, 0x4d, 0x2b, 0x44, 0xf5, 0xb7, 0x87, 0xda, 0xc8,
    0xc3, 0xaa, 0x43, 0x6d, 0x5e, 0x03, 0xcd, 0x47, 0x26, 0xa0, 0x5a, 0x9a,
    0xdf, 0xc5, 0x02, 0xab, 0x78, 0xd7, 0xba, 0x19, 0x88, 0x17, 0x60, 0x69,
    0x5a, 0x50, 0x6e, 0x40, 0xcc, 0x6f, 0x85, 0xe2, 0x2c, 0x31, 0x5b, 0x01,
    0x57, 0xb4, 0x21, 0x16, 0xa7, 0x3e, 0xf0, 0x6d, 0x46, 0x01, 0x55, 0xe3,
    0xc1, 0x38, 0x95, 0x4a, 0x0b, 0x5b, 0x83, 0x8a, 0x74, 0x52, 0x8f, 0xa3,
    0x71, 0x0c, 0xbb, 0xd9, 0x88, 0x95, 0x51, 0x35, 0x3c, 0x9f, 0x76, 0x7c,
    0xda, 0x65, 0x75, 0x2d, 0x4e, 0xd8, 0x1b, 0x0f, 0xfa, 0x5b, 0x66, 0x81,
    0x4d, 0x9d, 0x63, 0x81, 0xe6, 0xa3, 0x37, 0x2e, 0x88, 0xc8, 0x54, 0xd2,
    0x56, 0x56, 0xd3, 0x54, 0x78, 0xa7, 0x48, 0xf1, 0x0e, 0xbd, 0xbf, 0x17,
    0xe0, 0x0d, 0x2c, 0xc1, 0x69, 0xc2, 0x43, 0xf5, 0x0b, 0x78, 0xce, 0x0d,
    0x51, 0xa2, 0x32, 0xf2, 0x42, 0x6e, 0x97, 0x89, 0xe1, 0x69, 0xa1, 0xb4,
    0x37, 0xae, 0xbc, 0x8c, 0xca, 0xd6, 0xb0, 0xa5, 0xce, 0xf6, 0x80, 0x9c,
    0xbe, 0x6c, 0x0b, 0x82, 0xb4, 0x76, 0x18, 0xee, 0x34, 0xdc, 0x1c, 0xe2,
    0xd0, 0x1f, 0x75, 0xfb, 0xcc, 0x18, 0x5f, 0x5d, 0x2e, 0xa9, 0x50, 0x58,
    0x0b, 0x41, 0xb2, 0x22, 0xd1, 0xb5, 0x43, 0x6d, 0x8a, 0x5e, 0xa9, 0xf8,
    0x20, 0x60, 0xa9, 0xbe, 0xd1, 0x8f, 0xbf, 0x68, 0x87, 0x00, 0x79, 0x46,
    0x3b, 0x51, 0xc7, 0xf9, 0x78, 0xf2, 0x34, 0x8a, 0x3a, 0x20, 0xc8, 0x53,
    0x87, 0x52, 0x58, 0x21, 0x3f, 0x5e, 0xcd, 0xb3, 0x14, 0x56, 0xc0, 0x8f,
    0xe7, 0x1b, 0x0e, 0xd1, 0x14, 0xd7, 0x49, 0x85, 0xa4, 0x4e, 0xe9, 0x81,
    0xbd, 0xb5, 0x22, 0x0a, 0xd4, 0x2f, 0x88, 0x0e, 0xab, 0xe4, 0xd2, 0x76,
    0x94, 0x41, 0x15, 0x0c, 0x28, 0x2f, 0x16, 0xb7, 0x65, 0x8e, 0x46, 0x14,
    0xc9, 0xa8, 0xe5, 0x54, 0xd4, 0x00, 0xfc, 0x5a, 0x37, 0x67, 0xb9, 0x46,
    0x01, 0x30, 0x9f, 0x80, 0x05, 0x08, 0x6e, 0xca, 0x0e, 0xa9, 0x1d, 0x9f,
    0xc4, 0x71, 0x5f, 0xb2, 0x92, 0x95, 0x0e, 0xaf, 0x69, 0xf3, 0x13, 0x96,
    0xcd, 0xaf, 0xaf, 0x76, 0xb5, 0xd2, 0xed, 0x08, 0x4b, 0xb7, 0x03, 0x76,
    0xc0, 0x96, 0x52, 0x66, 0x67, 0xa0, 0xf4, 0x3c, 0x96, 0x9b, 0xc2, 0x6f,
    0x30, 0x15, 0x2a, 0xba, 0xd8, 0x30, 0x15, 0x1a, 0x83, 0xce, 0xe5, 0x86,
    0x37, 0xda, 0x3c, 0x74, 0x39, 0xda, 0x26, 0xb4, 0x54, 0x55, 0x7e, 0x7c,
    0x88, 0x2f, 0x34, 0x43, 0xa8, 0x4a, 0xb5, 0xa4, 0xdd, 0xab, 0x3a, 0xd9,
    0x40, 0x9b, 0xcd, 0xa6, 0xaf, 0xcd, 0x8e, 0x6d, 0x61, 0x48, 0xfd, 0x8e,
    0xd0, 0x53, 0x2b, 0xbc, 0xb6, 0xcd, 0x08, 0xb6, 0xe1, 0x37, 0x8b, 0xb6,
    0x95, 0xbc, 0xa5, 0x23, 0x5e, 0xec, 0x28, 0x97, 0x4a, 0x63, 0x38, 0x71,
    0xce, 0x64, 0x35, 0xf4, 0xee, 0xed, 0x1b, 0xb4, 0xca, 0x13, 0xac, 0x1c,
    0x40, 0xac, 0x42, 0xe5, 0xfd, 0x41, 0x12, 0xb0, 0x8b, 0x3b, 0xdb, 0x10,
    0x55, 0xcd, 0xdf, 0x56, 0x9b, 0xf8, 0x53, 0xa7, 0x5c, 0x1a, 0x28, 0xe5,
    0xbe, 0x35, 0xc4, 0xb8, 0x36, 0x83, 0x2c, 0xc5, 0x28, 0xd9, 0xab, 0xe1,
    0x11, 0x5b, 0x96, 0x4a, 0x90, 0xb6, 0x36, 0x73, 0x90, 0x2e, 0x76, 0x1d,
    0x4d, 0x79, 0x6a, 0x2f, 0x53, 0x2f, 0x00, 0x3b, 0x7d, 0x04, 0x26, 0xd3,
    0x5b, 0xdd, 0xb1, 0xcd, 0x65, 0x9a, 0xf9, 0xde, 0x8e, 0x36, 0x70, 0x98,
    0xdd, 0x23, 0xaf, 0xf8, 0xa5, 0x4e, 0xe5, 0x06, 0x82, 0xb3, 0xe6, 0xe7,
    0xb3, 0xae, 0x0c, 0x00, 0x5a, 0x5b, 0xd6, 0x4d, 0x47, 0xd6, 0x65, 0xf0,
    0x75, 0x88, 0x54, 0x4a, 0x84, 0x20, 0x05, 0xf7, 0x91, 0xf2, 0x95, 0xe1,
    0x40, 0x56, 0x17, 0x77, 0xf9, 0x02, 0xfc, 0x0f, 0x64, 0x6f, 0xb7, 0xae,
    0xb7, 0xad, 0x3b, 0x20, 0xd2, 0x10, 0xaf, 0x28, 0x5d, 0xd3, 0x71, 0x8a,
    0xfe, 0x44, 0x7b, 0x3f, 0x52, 0x43, 0xa0, 0xd5, 0x20, 0xda, 0x1d, 0x56,
    0x1d, 0xb5, 0xe4, 0x15, 0x33, 0x55, 0x2d, 0x50, 0xbd, 0xef, 0xd4, 0xec,
    0xad, 0x0e, 0xef, 0x82, 0xa4, 0x05, 0xad, 0x22, 0x70, 0x19, 0xf6, 0x45,
    0x93, 0xfb, 0xfc, 0x8c, 0x73, 0x84, 0xe7, 0x98, 0x32, 0xe8, 0x11, 0x6e,
    0x94, 0x44, 0x71, 0x10, 0xe3, 0x08, 0xe9, 0x29, 0x2a, 0xc5, 0x90, 0x25,
    0x91, 0xf5, 0xdb, 0x5d, 0x9c, 0x11, 0x01, 0x2a, 0x73, 0x0e, 0x4e, 0xf4,
    0x24, 0x07, 0x2c, 0xd9, 0x33, 0x1d, 0x80, 0x9a, 0xbd, 0x32, 0x04, 0xaf,
    0xb5, 0x7a, 0xb9, 0x12, 0x71, 0x3c, 0x5f, 0x8b, 0x32, 0xba, 0xd0, 0xc8,
    0x38, 0x9e, 0xaf, 0x05, 0x19, 0x5d, 0x6e, 0x24, 0x1c, 0xcf, 0xd7, 0x62,
    0x8c, 0x2e, 0x37, 0xf2, 0x8d, 0xe7, 0x27, 0xcc, 0xf4, 0x01, 0x92, 0x8d,
    0x57, 0xfa, 0x3c, 0x5a, 0xab, 0xd3, 0x0d, 0x89, 0x6f, 0x8e, 0x35, 0x64,
    0xbe, 0x51, 0x32, 0x84, 0x1b, 0x05, 0x19, 0xbf, 0xab, 0x41, 0x80, 0x8a,
    0x9b, 0xd4, 0x0e, 0x9e, 0x8f, 0xf3, 0x53, 0x8b, 0xfa, 0xdc, 0x90, 0x4b,
    0x8b, 0xfa, 0xf8, 0xe7, 0xe4, 0x22, 0xa4, 0xe5, 0xa8, 0x68, 0x6b, 0x32,
    0xbc, 0xdd, 0x5d, 0x97, 0x4b, 0x55, 0x87, 0x69, 0x1c, 0x75, 0xeb, 0x78,
    0x7e, 0x13, 0xa5, 0x68, 0x40, 0xd8, 0x93, 0xd0, 0xfc, 0x38, 0xcc, 0xa2,
    0x9b, 0x84, 0xdc, 0x62, 0x5d, 0x88, 0xb7, 0x56, 0x01, 0x75, 0xa8, 0x03,
    0x18, 0x9d, 0x13, 0x05, 0x38, 0x30, 0xf0, 0x67, 0xc3, 0x83, 0xd4, 0xa1,
    0x34, 0xc8, 0x1d, 0xb9, 0x05, 0xa5, 0xc3, 0x57, 0x71, 0xef, 0x26, 0x24,
    0xb9, 0x67, 0xda, 0xfa, 0x24, 0xfa, 0xf2, 0xe9, 0x9d, 0xce, 0xed, 0xfb,
    0xf1, 0xf2, 0x67, 0x12, 0x8b, 0x2f, 0x9f, 0xde, 0xb9, 0x70, 0x4f, 0x7e,
    0x48, 0xb3, 0x4b, 0xf7, 0xbc, 0xb5, 0x3c, 0xe1, 0x4b, 0xba, 0x36, 0xf4,
    0x2e, 0xfc, 0x35, 0x78, 0x4d, 0x85, 0xdd, 0xdc, 0x85, 0xa5, 0x07, 0xb7,
    0x7a, 0x5b, 0xda, 0x60, 0x2c, 0x13, 0xf0, 0xc9, 0xe4, 0x29, 0xc4, 0x67,
    0x01, 0xf8, 0x78, 0xc8, 0xbc, 0xab, 0x46, 0xf8, 0xeb, 0x09, 0x95, 0xa5,
    0xb0, 0x07, 0xc9, 0x44, 0x57, 0xb9, 0x4c, 0x11, 0xa8, 0xf2, 0xe9, 0xd1,
    0xf8, 0x0a, 0xb2, 0x87, 0x7c, 0x7a, 0x17, 0x14, 0xe4, 0x3a, 0xbb, 0xb2,
    0x66, 0x4b, 0xb6, 0x61, 0x5e, 0xba, 0xac, 0x77, 0x48, 0xe1, 0xc8, 0x9a,
    0x09, 0x33, 0xc8, 0xc8, 0xc4, 0x22, 0x06, 0x32, 0x48, 0x4b, 0x66, 0x99,
    0x93, 0xb7, 0x9a, 0xb4, 0xed, 0x40, 0x6a, 0x1f, 0x95, 0xd5, 0xca, 0xdd,
    0xae, 0x5a, 0xdd, 0xd3, 0x83, 0x6e, 0x04, 0x16, 0xd6, 0x22, 0x35, 0xce,
    0x0f, 0x72, 0x95, 0x48, 0xb5, 0xb1, 0x58, 0x1b, 0xea, 0xad, 0xa9, 0x9d,
    0x98, 0xf0, 0x0f, 0xbb, 0xbb, 0xd4, 0xe4, 0xa8, 0x0d, 0x64, 0x5e, 0xa7,
    0x2a, 0xd5, 0x93, 0x9b, 0x45, 0xf2, 0x90, 0x64, 0x56, 0x35, 0x97, 0x29,
    0x65, 0x59, 0xdd, 0x35, 0xb4, 0x20, 0x49, 0x80, 0xb4, 0xab, 0xe7, 0x35,
    0x86, 0x34, 0xab, 0x60, 0x27, 0xe1, 0x15, 0x66, 0x53, 0x0a, 0x63, 0x47,
    0x9a, 0xfb, 0xc2, 0xa5, 0x4b, 0xeb, 0x0d, 0x2c, 0xc8, 0xac, 0x20, 0x7c,
    0xd1, 0x4b, 0xb3, 0xf9, 0x37, 0xc2, 0x99, 0x6c, 0x01, 0x5d, 0xa8, 0xa6,
    0xcd, 0x40, 0x76, 0x23, 0xa1, 0x3e, 0x5d, 0x9b, 0x3c, 0xa7, 0x81, 0xca,
    0x90, 0xe4, 0x05, 0x39, 0x4e, 0xce, 0x00, 0xde, 0xdd, 0x41, 0xdf, 0x2b,
    0x91, 0xf4, 0x39, 0x4c, 0xc9, 0x35, 0x49, 0x9b, 0x7e, 0x27, 0x50, 0xed,
    0x2d, 0x4b, 0xdc, 0x17, 0x5e, 0xa9, 0x7c, 0x36, 0x95, 0xa8, 0x5d, 0x1a,
    0xf9, 0x6e, 0xfa, 0x4f, 0x53, 0x4f, 0x66, 0x93, 0x40, 0x84, 0x89, 0x82,
    0x12, 0x1e, 0x38, 0x1b, 0x41, 0xa4, 0xa9, 0x86, 0x5c, 0x81, 0x39, 0xeb,
    0x91, 0x3a, 0x8c, 0x86, 0x06, 0x63, 0xbe, 0x51, 0x83, 0x01, 0x33, 0x90,
    0xfa, 0x11, 0x09, 0x2b, 0x8f, 0xd7, 0x6f, 0xfa, 0x59, 0x65, 0x03, 0xb0,
    0xa7, 0xab, 0x5d, 0x18, 0xc0, 0x68, 0xed, 0x7e, 0xf2, 0xc1, 0xc2, 0xe4,
    0xdb, 0x45, 0x9c, 0xf8, 0xc3, 0xc3, 0xbe, 0x57, 0xc2, 0x22, 0x46, 0xff,
    0x74, 0xbc, 0xa7, 0x12, 0x7c, 0x83, 0xfb, 0xf7, 0x5d, 0x4a, 0xc6, 0x61,
    0x91, 0x65, 0x62, 0x3d, 0xcb, 0x98, 0xe8, 0xcd, 0xf0, 0x92, 0xa6, 0x77,
    0xe1, 0x69, 0x06, 0x19, 0xc3, 0xb8, 0xff, 0x3e, 0x63, 0x82, 0x93, 0xa2,
    0xc0, 0xc2, 0x5f, 0xd1, 0x1e, 0xc7, 0x8c, 0xf7, 0x38, 0x29, 0xe8, 0xcc,
    0xe7, 0x77, 0x5c, 0x90, 0x65, 0x6f, 0x45, 0xfd, 0x1e, 0xdc, 0x71, 0xd2,
    0x53, 0x05, 0xfe, 0x19, 0x99, 0x67, 0x04, 0x7d, 0x39, 0xf1, 0xeb, 0xba,
    0x23, 0xe5, 0xec, 0xf2, 0x64, 0x36, 0x84, 0x7f, 0x23, 0xb8, 0xba, 0x73,
    0x99, 0x87, 0x2b, 0x7c, 0x32, 0x38, 0x82, 0x7f, 0xaa, 0x42, 0x8f, 0xc7,
    0xb0, 0x31, 0x21, 0x6c, 0xca, 0xa8, 0xd7, 0xcb, 0x0a, 0xb8, 0x92, 0xa1,
    0x4e, 0x9a, 0x59, 0x15, 0x0c, 0xc3, 0x27, 0xb3, 0xd9, 0x73, 0xbc, 0x9f,
    0x8c, 0x7a, 0x3d, 0xbe, 0x20, 0x69, 0x5a, 0x75, 0xd2, 0xeb, 0x89, 0x2c,
    0x0f, 0x75, 0x42, 0xf6, 0x51, 0x4f, 0x89, 0x5f, 0xe1, 0x93, 0xe1, 0x21,
    0xfc, 0x33, 0xcf, 0x43, 0xab, 0x82, 0x34, 0x96, 0x84, 0x4f, 0x86, 0xf1,
    0x30, 0x1e, 0x92, 0x51, 0xaf, 0x07, 0xe9, 0x49, 0xc2, 0x27, 0x07, 0x83,
    0x83, 0xc1, 0xc1, 0xfe, 0xa8, 0xd7, 0x93, 0x41, 0xec, 0xe1, 0x93, 0xcb,
    0x97, 0x97, 0x47, 0x97, 0x97, 0xa3, 0x5e, 0x6f, 0x2e, 0xd3, 0x19, 0x3e,
    0x39, 0x9c, 0xbd, 0x3c, 0x18, 0xcc, 0x46, 0xbd, 0x9e, 0xf2, 0x00, 0x0d,
    0x75, 0x7e, 0x4f, 0x39, 0x1d, 0x9c, 0x64, 0x37, 0x61, 0x1f, 0xbd, 0xcc,
    0x6f, 0xd1, 0x7e, 0x3f, 0xbf, 0x45, 0x4f, 0xfa, 0xfd, 0xfe, 0x7e, 0x29,
    0x77, 0x57, 0x3b, 0xe7, 0x29, 0xe8, 0x00, 0x36, 0xf5, 0x62, 0xad, 0xb7,
    0x65, 0xf8, 0x72, 0x78, 0x34, 0xc4, 0x8d, 0x6d, 0x21, 0x33, 0x32, 0x23,
    0x49, 0x73, 0x5b, 0x64, 0xa3, 0x7a, 0xd1, 0xba, 0x8a, 0x5e, 0xf4, 0x6c,
    0x36, 0xab, 0x57, 0x6c, 0x3d, 0xc0, 0x6e, 0x3d, 0x9f, 0x1d, 0xce, 0xf6,
    0xeb, 0xe5, 0xce, 0xf0, 0x0c, 0xcf, 0x5e, 0x56, 0xcb, 0x4d, 0x5e, 0x24,
    0x87, 0xc9, 0xb0, 0x5e, 0xee, 0xf3, 0xa3, 0xe7, 0xcf, 0x9f, 0xe3, 0x7a,
    0xb9, 0xfb, 0x87, 0x2f, 0x0e, 0x87, 0x43, 0x7b, 0x75, 0x2f, 0xf2, 0x5b,
    0x34, 0x3c, 0x80, 0xd5, 0xa9, 0x99, 0x0f, 0x86, 0xe5, 0xff, 0x5a, 0x5f,
    0x66, 0xb7, 0x3d, 0x4e, 0xff, 0x0a, 0x0e, 0x3e, 0x2a, 0x07, 0x6a, 0xef,
    0x32, 0xbb, 0x2d, 0x21, 0x4b, 0xff, 0x5a, 0xa5, 0x70, 0xe8, 0x55, 0x99,
    0x1b, 0x54, 0xe2, 0x86, 0x12, 0x30, 0xde, 0x7a, 0x89, 0x8b, 0x39, 0x65,
    0x61, 0xdf, 0x5e, 0xfd, 0x35, 0x2e, 0x5c, 0xbd, 0x4c, 0x30, 0x75, 0x32,
    0x01, 0x1d, 0x93, 0x70, 0x70, 0x90, 0xdf, 0x96, 0x4a, 0x5d, 0xe3, 0x4b,
    0xb6, 0xd3, 0x57, 0x06, 0x5f, 0x09, 0xb6, 0x21, 0x65, 0x0b, 0x52, 0x50,
    0x61, 0x2a, 0xe0, 0x46, 0x15, 0x5f, 0xbb, 0xf6, 0xaf, 0xb3, 0x95, 0x80,
    0x55, 0x2b, 0xef, 0x18, 0x3d, 0x90, 0x02, 0x2b, 0x4f, 0xb7, 0x5c, 0xc7,
    0xab, 0x82, 0x67, 0x45, 0xa8, 0xbd, 0x0e, 0x75, 0x69, 0x68, 0x98, 0x2c,
    0xf3, 0x9a, 0x65, 0xa2, 0xa7, 0x6d, 0x4e, 0xa3, 0x2c, 0xc7, 0x31, 0x38,
    0x45, 0x05, 0x87, 0xa6, 0xf6, 0x2c, 0x8b, 0x57, 0xbc, 0x77, 0x4d, 0x39,
    0xbd, 0x4c, 0x89, 0x8f, 0x5b, 0xcf, 0x2a, 0x0c, 0xa5, 0x59, 0xa6, 0xe6,
    0xd9, 0x2e, 0x54, 0xb3, 0x6e, 0x96, 0x9a, 0x35, 0x84, 0xc3, 0xfc, 0x56,
    0x67, 0xc5, 0x6e, 0xac, 0x63, 0x64, 0xd6, 0xa8, 0xb2, 0x37, 0x41, 0xb5,
    0x52, 0x65, 0x04, 0x5e, 0x2f, 0x08, 0x80, 0x50, 0xf8, 0xfc, 0x20, 0xbf,
    0x1d, 0xe5, 0x38, 0x49, 0xe0, 0xb0, 0xfa, 0xe0, 0x31, 0xee, 0x0e, 0x9e,
    0xe7, 0xb7, 0x7e, 0x8c, 0xd3, 0xd8, 0x75, 0x07, 0xfd, 0xfe, 0xf5, 0x0d,
    0xea, 0xa1, 0xc1, 0xe0, 0xa8, 0x9f, 0xdf, 0x7a, 0x7b, 0x43, 0xcf, 0x1b,
    0x25, 0x94, 0x83, 0x75, 0x35, 0x9c, 0xa5, 0xe4, 0x76, 0x84, 0x53, 0x3a,
    0x67, 0x3d, 0x2a, 0xc8, 0x92, 0x87, 0x20, 0x8a, 0x91, 0x62, 0x34, 0xc7,
    0x79, 0x08, 0x10, 0xd1, 0x3d, 0x45, 0x91, 0xe5, 0xde, 0xa8, 0x02, 0x08,
    0x21, 0xb2, 0x65, 0x38, 0xa8, 0xe6, 0x0d, 0xb7, 0xe3, 0xf9, 0x28, 0xcf,
    0x38, 0x95, 0x0c, 0x1d, 0x17, 0x34, 0xbe, 0xba, 0x1b, 0x01, 0x3c, 0xf7,
    0x47, 0x7f, 0xed, 0x29, 0xf7, 0xcb, 0x61, 0xbf, 0x0c, 0x2e, 0x0b, 0xcc,
    0x92, 0xf5, 0xd7, 0x26, 0xa1, 0x3d, 0xea, 0x14, 0x1c, 0x8c, 0x80, 0xa2,
    0xf4, 0x12, 0x12, 0x67, 0x8a, 0x77, 0x0f, 0xc1, 0xfa, 0x3d, 0x5a, 0x52,
    0xa6, 0xbe, 0xf3, 0x10, 0x0e, 0x07, 0xfd, 0xfc, 0x56, 0xf7, 0x8c, 0x2e,
    0xab, 0xbe, 0x65, 0x2e, 0x91, 0xd1, 0x06, 0xe8, 0x18, 0x29, 0x29, 0xa2,
    0xc7, 0xe1, 0xac, 0xd9, 0x3c, 0x0c, 0xfa, 0x2f, 0x0e, 0xc9, 0xd2, 0x86,
    0xce, 0xc3, 0xba, 0x3f, 0xa9, 0xa9, 0x6c, 0xf5, 0x69, 0xd5, 0x1c, 0xe4,
    0xb7, 0xea, 0xf1, 0x46, 0x9d, 0xc7, 0x8b, 0x7e, 0xbf, 0xdb, 0xfd, 0x01,
    0x59, 0x8e, 0xd4, 0xe5, 0x90, 0x17, 0x7c, 0x90, 0xdf, 0x96, 0x0c, 0x5f,
    0x37, 0x37, 0x01, 0xb6, 0x1d, 0x76, 0x5d, 0xd7, 0x93, 0xc2, 0x6f, 0x08,
    0xd9, 0x7c, 0xa0, 0x2a, 0xc2, 0x6b, 0x7b, 0x1d, 0xf2, 0x76, 0x7b, 0x9b,
    0xb7, 0xc5, 0xc0, 0x02, 0xa0, 0xad, 0xc1, 0x10, 0x8e, 0x51, 0x1d, 0x58,
    0x81, 0x13, 0xba, 0xe2, 0xe1, 0xe1, 0x86, 0xf9, 0x5a, 0xcb, 0x19, 0xaa,
    0xa9, 0x21, 0x1c, 0x2e, 0x80, 0x19, 0x5f, 0x37, 0xcf, 0xa1, 0x03, 0x11,
    0x12, 0x0b, 0x79, 0xa5, 0x4e, 0x8e, 0xad, 0xb5, 0x04, 0xeb, 0xc7, 0x40,
    0xd8, 0x11, 0xec, 0x70, 0xcd, 0xe9, 0xfb, 0xf0, 0xd1, 0x85, 0xaa, 0x21,
    0x95, 0xc9, 0xa5, 0x7a, 0x0f, 0xb5, 0x7f, 0x51, 0xad, 0xcc, 0x82, 0x41,
    0x35, 0x29, 0x99, 0x61, 0xb5, 0xb5, 0xec, 0x97, 0x2f, 0xad, 0x6b, 0x72,
    0x98, 0xdf, 0x02, 0x56, 0x1f, 0x6d, 0xd8, 0xd2, 0x87, 0x4f, 0xd6, 0x9e,
    0x31, 0xa2, 0x6b, 0x05, 0x7d, 0x47, 0xf9, 0xed, 0x48, 0x5f, 0xc6, 0xa3,
    0xee, 0x76, 0xf7, 0xbf, 0x6b, 0xd0, 0x82, 0x17, 0x2f, 0x5e, 0xd8, 0x9d,
    0xe8, 0x34, 0x5a, 0x88, 0xae, 0x3b, 0x5b, 0x2b, 0x31, 0x36, 0x2c, 0xe3,
    0xb6, 0x46, 0xd6, 0xf0, 0x6f, 0x3f, 0xbf, 0x45, 0x8a, 0x94, 0x2c, 0xe9,
    0xad, 0x4b, 0x19, 0xe2, 0xc5, 0xfc, 0xd2, 0xb7, 0xdb, 0xa0, 0xe1, 0xe1,
    0x77, 0xbe, 0x28, 0x30, 0xe3, 0x39, 0x2e, 0x08, 0x13, 0x5e, 0x73, 0x44,
    0x95, 0x59, 0x75, 0xd3, 0x90, 0x8a, 0x06, 0x7a, 0x65, 0x00, 0x1f, 0x3d,
    0xe9, 0x69, 0x14, 0xaa, 0x16, 0xb9, 0x7f, 0x50, 0xaf, 0x72, 0xbf, 0x81,
    0x72, 0xba, 0x0b, 0x2e, 0x21, 0x96, 0x60, 0xbd, 0xc4, 0xb7, 0xfa, 0x7a,
    0x2a, 0xe4, 0xa3, 0x01, 0x5b, 0x82, 0x74, 0xd5, 0x7a, 0x00, 0x3b, 0x66,
    0x4e, 0x1d, 0xc2, 0x56, 0x47, 0xf0, 0xbf, 0x9e, 0x20, 0xcb, 0x1c, 0xac,
    0xf8, 0x80, 0xda, 0x57, 0x4b, 0x10, 0x3a, 0xa5, 0x87, 0x9f, 0x3b, 0xf4,
    0x97, 0x94, 0x01, 0x8a, 0xeb, 0xfb, 0x83, 0x59, 0xe1, 0x79, 0x12, 0x12,
    0x24, 0x29, 0xe1, 0x6a, 0x75, 0xdd, 0x35, 0x49, 0xc2, 0xe9, 0x7d, 0x0b,
    0xa8, 0xbc, 0xb0, 0x56, 0x07, 0xa8, 0xd4, 0x42, 0x34, 0x7d, 0xfb, 0x38,
    0x0c, 0x65, 0x83, 0x07, 0x6f, 0xa4, 0xe9, 0xa2, 0x75, 0xcb, 0x5f, 0x1c,
    0xd5, 0xf3, 0x0a, 0x6e, 0x68, 0x42, 0xd6, 0x72, 0x6d, 0x6a, 0x49, 0xe1,
    0x60, 0xaf, 0x37, 0x28, 0x03, 0xfd, 0xba, 0x07, 0xf7, 0x87, 0xb2, 0xb9,
    0x2f, 0xd3, 0x56, 0x65, 0x85, 0x8a, 0x28, 0x6c, 0xde, 0xa3, 0x9f, 0x57,
    0x5c, 0xd0, 0x99, 0x94, 0x8e, 0x81, 0xb3, 0x0e, 0x01, 0xbb, 0x90, 0xde,
    0x25, 0x11, 0x37, 0x84, 0xb0, 0x6d, 0xb7, 0x44, 0xde, 0xe7, 0x80, 0xdc,
    0x91, 0xcb, 0x22, 0xbb, 0xa9, 0xe8, 0x33, 0xea, 0xa3, 0x61, 0x0b, 0xfc,
    0x0d, 0x66, 0xb4, 0x01, 0xfe, 0xa8, 0x89, 0x1a, 0xe0, 0x3e, 0x48, 0x8c,
    0x23, 0x41, 0x0b, 0x34, 0xbd, 0xe1, 0x0a, 0xb8, 0xf4, 0x18, 0x73, 0xd2,
    0x45, 0x79, 0x47, 0x64, 0x59, 0x2e, 0x06, 0xfe, 0x62, 0xe8, 0x2f, 0xf6,
    0xfd, 0xc5, 0x41, 0xcd, 0x1b, 0xd8, 0x7c, 0xa9, 0xc5, 0x8f, 0x1a, 0x16,
    0xb5, 0x66, 0x34, 0xcb, 0xc5, 0x60, 0x5d, 0x0f, 0x3f, 0xdc, 0x07, 0xd2,
    0x37, 0xb4, 0x4a, 0x06, 0x2f, 0xa1, 0x64, 0x7f, 0xdd, 0x42, 0xdb, 0x8b,
    0x03, 0xbb, 0x64, 0x7f, 0xcb, 0x3a, 0xf5, 0x39, 0x69, 0x2a, 0xf6, 0x02,
    0x76, 0x89, 0x82, 0xf9, 0x66, 0x13, 0x9a, 0xd5, 0x53, 0x07, 0x22, 0x03,
    0xdf, 0xee, 0x81, 0x89, 0x04, 0xf2, 0x95, 0x1f, 0xd4, 0xb9, 0x12, 0xd6,
    0x0f, 0x23, 0x13, 0x79, 0x0e, 0x80, 0x51, 0xc1, 0xca, 0x6d, 0x43, 0x29,
    0x40, 0x20, 0x2e, 0x7a, 0x90, 0xe7, 0x96, 0x82, 0xd0, 0x3c, 0xd8, 0x3f,
    0x4c, 0xc8, 0xdc, 0xb7, 0x81, 0xd7, 0x7e, 0x18, 0x7a, 0x5e, 0x19, 0x58,
    0xf9, 0x63, 0xd7, 0x8f, 0xbf, 0x3c, 0x07, 0x9b, 0x2e, 0xcf, 0x51, 0x4d,
    0x71, 0x24, 0x65, 0x82, 0xbb, 0x14, 0x58, 0x99, 0x66, 0xd7, 0xd5, 0x4d,
    0xe8, 0x57, 0xf4, 0x64, 0xe3, 0x15, 0x1b, 0x3e, 0xee, 0x8e, 0xa5, 0x64,
    0x26, 0xc2, 0xfd, 0x2d, 0xcc, 0x8e, 0x75, 0xd1, 0x1a, 0x93, 0xd8, 0x48,
    0x83, 0x1f, 0xde, 0xef, 0xfe, 0x37, 0x01, 0xeb, 0x01, 0x59, 0xb6, 0x06,
    0x94, 0x21, 0xb2, 0x7e, 0xa3, 0x0c, 0xb7, 0x26, 0x00, 0xa7, 0x39, 0x4b,
    0xb3, 0x9b, 0x50, 0x59, 0x3b, 0xd4, 0x70, 0x55, 0x21, 0x49, 0x53, 0x9a,
    0x73, 0xca, 0x47, 0x32, 0x70, 0x57, 0x8e, 0x45, 0x42, 0x96, 0x41, 0x6e,
    0x16, 0x7b, 0xc3, 0x6b, 0xf8, 0x34, 0x34, 0xb6, 0x41, 0x72, 0x0e, 0x5b,
    0x1b, 0xd1, 0x24, 0xcb, 0x4d, 0x96, 0x37, 0xb0, 0x12, 0xde, 0x76, 0x59,
    0x0b, 0xc0, 0x6a, 0xf0, 0xd0, 0x83, 0x19, 0x84, 0xed, 0x69, 0xc0, 0xe5,
    0x2e, 0x65, 0x46, 0x83, 0x6e, 0x4b, 0x40, 0x81, 0x1b, 0xd0, 0x8b, 0x4d,
    0x4c, 0x8f, 0xbe, 0x0e, 0x16, 0x06, 0xbb, 0x1e, 0x3c, 0x8a, 0xee, 0x6e,
    0xe2, 0x6c, 0xd4, 0xfc, 0x1e, 0xda, 0x00, 0x78, 0x1f, 0x40, 0xda, 0xde,
    0x26, 0xdb, 0xa2, 0x5e, 0x54, 0xf4, 0x7a, 0xf0, 0xa2, 0x26, 0x65, 0x83,
    0x17, 0x1b, 0x29, 0x76, 0xe3, 0x52, 0xc9, 0x9c, 0x40, 0xcd, 0xb5, 0x6f,
    0xe1, 0x83, 0xf4, 0x2a, 0x40, 0x08, 0x0f, 0x95, 0x35, 0xc8, 0x5a, 0xd7,
    0x4b, 0xb3, 0xc3, 0x72, 0x86, 0x0f, 0x50, 0x7c, 0x23, 0x5f, 0xcf, 0x66,
    0x65, 0x20, 0xb2, 0x2c, 0xbd, 0xc4, 0x85, 0x1f, 0xc8, 0x4c, 0x81, 0x3a,
    0x64, 0xf0, 0xb1, 0xfc, 0x55, 0xeb, 0xbc, 0xcb, 0x66, 0x27, 0x6d, 0x7a,
    0x22, 0x2b, 0x13, 0x96, 0x34, 0xc0, 0x62, 0x58, 0x4b, 0x69, 0x52, 0x67,
    0x66, 0x98, 0x02, 0xb8, 0xa9, 0x86, 0x1b, 0x78, 0xfe, 0x6d, 0x9c, 0xd8,
    0xa1, 0x45, 0x5e, 0x8f, 0xb6, 0xe2, 0x14, 0xbd, 0xa1, 0x5f, 0xb9, 0x1a,
    0xda, 0x81, 0xaa, 0xbb, 0x95, 0x06, 0xa3, 0xe8, 0xa1, 0x37, 0x51, 0x00,
    0x7b, 0x9b, 0x75, 0x3f, 0x1a, 0xb6, 0xb6, 0xf4, 0x36, 0x54, 0x57, 0x8c,
    0x08, 0xbb, 0x82, 0xc5, 0x66, 0x95, 0x81, 0xe2, 0x9f, 0xd6, 0x1b, 0x06,
    0xd5, 0x9c, 0x55, 0x3d, 0x28, 0x8e, 0x5f, 0xce, 0xf4, 0x81, 0x7c, 0x23,
    0x1a, 0xdf, 0xcc, 0x03, 0x81, 0x00, 0x24, 0xc3, 0x6f, 0x9b, 0xc0, 0x21,
    0x0f, 0x35, 0xa1, 0x85, 0xd6, 0xb2, 0xab, 0xae, 0x64, 0x8b, 0xc3, 0x6f,
    0xe6, 0x7f, 0x9f, 0x1f, 0xf6, 0x9b, 0x18, 0xda, 0x06, 0x88, 0x36, 0x34,
    0x6d, 0x14, 0xe3, 0x6c, 0xd9, 0xbe, 0x73, 0x98, 0x0d, 0x2d, 0x80, 0xbe,
    0xa9, 0xfd, 0xfe, 0x77, 0x0d, 0xee, 0xcb, 0x06, 0xbb, 0xa3, 0x36, 0x1c,
    0xf5, 0xbf, 0x1d, 0x0e, 0x1f, 0x05, 0x76, 0xa5, 0x25, 0xe2, 0x37, 0x44,
    0xfb, 0xf5, 0x76, 0xe8, 0x52, 0x6d, 0xce, 0x65, 0xac, 0xb1, 0x09, 0xaa,
    0xbe, 0x30, 0xab, 0x3a, 0xd2, 0x3c, 0xa5, 0xc1, 0x41, 0x52, 0x14, 0xd2,
    0x8c, 0x9f, 0x44, 0x9a, 0x7e, 0x60, 0xb9, 0x1c, 0xac, 0x5b, 0xf7, 0xb1,
    0xe6, 0x4d, 0xf7, 0xbf, 0x6d, 0xc1, 0xcf, 0x1f, 0x40, 0xd2, 0xcd, 0xf1,
    0x91, 0x05, 0x98, 0x1d, 0x22, 0xa1, 0x3d, 0x88, 0xc0, 0xe4, 0xec, 0x07,
    0x2a, 0xb5, 0x2a, 0xfc, 0x6e, 0x41, 0xb0, 0xa6, 0x38, 0x76, 0xeb, 0x7e,
    0xdd, 0xda, 0x34, 0x54, 0x4f, 0x5f, 0xc5, 0x67, 0x0f, 0xb3, 0xbe, 0x06,
    0xfe, 0x47, 0xff, 0x60, 0x56, 0xa5, 0x86, 0x93, 0x6a, 0xd1, 0xe0, 0x36,
    0xb5, 0x6e, 0xdc, 0x01, 0xeb, 0x45, 0x47, 0xf3, 0xf0, 0x9b, 0x19, 0x84,
    0x66, 0xb7, 0x8f, 0x64, 0x7e, 0xac, 0xad, 0x1e, 0x5a, 0x53, 0xfe, 0x16,
    0x9a, 0xf1, 0xdc, 0x30, 0x7f, 0x32, 0x03, 0xb5, 0x06, 0xd7, 0x97, 0x35,
    0xb5, 0x7c, 0xf9, 0x18, 0xf1, 0x56, 0xe2, 0x9c, 0x8a, 0x60, 0xeb, 0xbe,
    0xb4, 0x9c, 0xbb, 0x8d, 0xe6, 0x95, 0x81, 0xe5, 0x92, 0xf5, 0x18, 0x0c,
    0xf6, 0x08, 0x20, 0xb1, 0xd6, 0x75, 0xd0, 0xbc, 0x70, 0x47, 0xc3, 0xe6,
    0xc5, 0x49, 0x20, 0xc3, 0xca, 0xd7, 0x50, 0xc5, 0x36, 0xf5, 0x8b, 0x9c,
    0x49, 0x9b, 0x27, 0x52, 0x6c, 0xb4, 0xb5, 0x26, 0x74, 0xd9, 0x66, 0x49,
    0xd4, 0x0d, 0x7f, 0x2c, 0xee, 0x1f, 0xcc, 0x0a, 0x34, 0x98, 0xd9, 0x32,
    0x5d, 0xdd, 0x1e, 0xad, 0xd2, 0x5a, 0xba, 0xd2, 0x33, 0x50, 0x7c, 0xb6,
    0xc2, 0x34, 0xf8, 0xb6, 0xc2, 0x34, 0xa0, 0xc6, 0xae, 0xc1, 0x52, 0x2a,
    0x97, 0x9e, 0xd4, 0x1e, 0x24, 0xdd, 0x2b, 0x7c, 0xb4, 0xe9, 0x0a, 0xd7,
    0xf9, 0x74, 0xd6, 0xff, 0x20, 0xec, 0xd3, 0xbe, 0x29, 0xf6, 0x18, 0x63,
    0xa3, 0xed, 0x7d, 0xc4, 0x2e, 0xc1, 0x82, 0x90, 0xa6, 0x8e, 0x72, 0xad,
    0x92, 0x42, 0xca, 0x94, 0xd8, 0x5b, 0x05, 0xe4, 0x06, 0xe2, 0x18, 0x18,
    0xc4, 0xd1, 0xd4, 0x1b, 0x8f, 0x60, 0x77, 0x2a, 0xbe, 0x0e, 0x00, 0xbb,
    0x3b, 0xbf, 0x30, 0xec, 0xdd, 0x90, 0xcb, 0x2b, 0x0a, 0x6c, 0xab, 0x4c,
    0x6f, 0x00, 0xaa, 0x80, 0x2b, 0x52, 0x54, 0xf3, 0xde, 0xda, 0x50, 0x59,
    0xe6, 0xd6, 0x06, 0x6e, 0x9d, 0x5f, 0xff, 0xf6, 0xff, 0x1c, 0x8b, 0x0e,
    0x0f, 0xb7, 0x89, 0xea, 0x92, 0xf5, 0x50, 0xca, 0xd6, 0x60, 0x70, 0xc8,
    0xed, 0xbe, 0xcf, 0xc1, 0xd1, 0xe3, 0xa2, 0x3d, 0x42, 0x2d, 0x0a, 0x15,
    0x19, 0x00, 0xa5, 0xfb, 0xb2, 0x9f, 0x90, 0xb9, 0x67, 0x1a, 0x6e, 0xd0,
    0x39, 0x3c, 0xa4, 0xbb, 0x6b, 0xa4, 0x45, 0xd2, 0x02, 0x53, 0x13, 0x4b,
    0x75, 0xea, 0x48, 0x44, 0xd6, 0xbd, 0x48, 0xa6, 0x9e, 0x49, 0x7a, 0x64,
    0xe8, 0xe4, 0xc0, 0xe2, 0xd3, 0x07, 0x1b, 0x51, 0x4f, 0x85, 0x6a, 0x1a,
    0x48, 0xe8, 0xf0, 0xf0, 0xb0, 0x8b, 0xe0, 0x81, 0xd1, 0x9b, 0xcd, 0x66,
    0x07, 0x87, 0xb6, 0x0a, 0x87, 0x32, 0x4e, 0x04, 0xc8, 0xf4, 0x60, 0x04,
    0xd1, 0x16, 0x9e, 0xa3, 0xf6, 0x74, 0xf4, 0xb7, 0x21, 0xb6, 0x32, 0x99,
    0x9d, 0xfa, 0x32, 0x65, 0xd3, 0x1a, 0x33, 0xba, 0x54, 0xc2, 0x4b, 0xf3,
    0x75, 0x4f, 0xbe, 0x46, 0xc1, 0xf0, 0x88, 0x23, 0x88, 0x7b, 0x04, 0x57,
    0x7b, 0x7d, 0x6f, 0xd4, 0x37, 0x1d, 0xaa, 0x0e, 0xa5, 0x4d, 0xa5, 0x82,
    0x4d, 0x4b, 0x95, 0xab, 0x75, 0xc8, 0xdd, 0x3b, 0x57, 0xb5, 0x94, 0xa9,
    0x8b, 0x1e, 0x73, 0x65, 0xcc, 0x6d, 0x39, 0xac, 0x6e, 0xcb, 0xa0, 0x2f,
    0xc9, 0xe7, 0xa1, 0xfc, 0xbf, 0x26, 0xa5, 0x20, 0xe3, 0xc9, 0x5b, 0x04,
    0xa7, 0xff, 0xb2, 0x25, 0x0f, 0x12, 0x96, 0x94, 0x81, 0x49, 0x2f, 0xf3,
    0x20, 0xf2, 0x2e, 0xb2, 0x9b, 0x87, 0xd4, 0xb9, 0x2d, 0xee, 0xae, 0x0c,
    0xda, 0xb4, 0xeb, 0xb1, 0x08, 0xb2, 0xdf, 0x42, 0x5b, 0xc3, 0xba, 0xb3,
    0xb5, 0x7d, 0xd7, 0x47, 0xff, 0x00, 0x6d, 0xe1, 0x61, 0xdd, 0xb7, 0xbe,
    0x41, 0x8f, 0x51, 0xab, 0x35, 0xd5, 0x4f, 0x2f, 0xad, 0x3e, 0xfe, 0x11,
    0xd2, 0xc0, 0x51, 0xab, 0x3f, 0x14, 0xa8, 0x2c, 0xa7, 0x1b, 0x14, 0x90,
    0x55, 0x32, 0xd3, 0x47, 0x5d, 0x7c, 0x45, 0x00, 0xaa, 0x36, 0xc8, 0xe6,
    0x72, 0xa1, 0xd0, 0xb0, 0xb8, 0x0d, 0xa3, 0xd4, 0x41, 0x07, 0x9f, 0xda,
    0x5d, 0xa8, 0x40, 0x13, 0x65, 0xf6, 0x83, 0x83, 0x42, 0x2b, 0xda, 0x5b,
    0x66, 0x2c, 0x93, 0xfc, 0x90, 0x5f, 0xfd, 0x6a, 0xb1, 0xe4, 0x41, 0x2b,
    0x83, 0xa2, 0xcd, 0xa6, 0xaa, 0xd5, 0x37, 0x5f, 0xa8, 0xae, 0xfb, 0x75,
    0x39, 0xd2, 0x48, 0xc9, 0x6f, 0xf7, 0x64, 0x5e, 0xb4, 0x4c, 0x87, 0x5b,
    0x65, 0xa5, 0xad, 0x66, 0x94, 0x6a, 0xfd, 0xcf, 0x9b, 0x23, 0x2b, 0xdc,
    0xbc, 0x75, 0xfc, 0xe6, 0xeb, 0xcd, 0x7a, 0x0e, 0x3b, 0x09, 0xce, 0x5a,
    0x1d, 0x15, 0x27, 0xe9, 0x4c, 0xdd, 0x44, 0x96, 0x09, 0xe0, 0xa9, 0x1b,
    0x66, 0xa0, 0x7e, 0x13, 0xd6, 0x9f, 0x1c, 0x1e, 0x1d, 0xf4, 0x07, 0xb5,
    0x25, 0x7f, 0x46, 0x9e, 0x5f, 0x26, 0xdb, 0xec, 0x44, 0x96, 0xfe, 0x92,
    0x2b, 0x0f, 0xb4, 0x2e, 0xb8, 0xfc, 0x66, 0xdd, 0x74, 0x53, 0xb7, 0xda,
    0x18, 0xe1, 0xb7, 0xf2, 0xbd, 0xca, 0x77, 0x4f, 0x81, 0xe6, 0xba, 0x32,
    0x47, 0xe2, 0x4b, 0x9e, 0xa5, 0x2b, 0x41, 0x2a, 0xfb, 0x6e, 0xbf, 0xaa,
    0x09, 0x77, 0xa3, 0x35, 0x8c, 0x82, 0xe2, 0x83, 0x61, 0x4d, 0x80, 0x86,
    0xfb, 0xed, 0x2d, 0xac, 0x88, 0x8c, 0xd9, 0xb1, 0xa1, 0x64, 0x25, 0xcc,
    0x80, 0x05, 0x49, 0x31, 0x90, 0x8d, 0xc6, 0x30, 0x21, 0x9e, 0x09, 0xa9,
    0xbc, 0xd2, 0xa4, 0xde, 0x19, 0x75, 0x27, 0xb8, 0x45, 0x49, 0x65, 0x14,
    0xa7, 0x23, 0xa3, 0x35, 0x7c, 0x98, 0x11, 0x07, 0xaf, 0x81, 0x0e, 0x83,
    0x60, 0x6f, 0x4d, 0xa8, 0xc3, 0x17, 0xbf, 0x97, 0xcb, 0xdf, 0x4e, 0xdb,
    0xb6, 0xb6, 0xd1, 0x6b, 0xa9, 0x99, 0x0a, 0xf9, 0x0b, 0x70, 0x94, 0x0b,
    0x9a, 0x79, 0x00, 0x53, 0xe8, 0xe2, 0xd1, 0xb8, 0x45, 0x22, 0xc1, 0xaa,
    0x89, 0x39, 0x3f, 0x63, 0x65, 0xb2, 0x5f, 0x69, 0x94, 0x51, 0xcb, 0x61,
    0x8a, 0x5a, 0x58, 0xea, 0x08, 0xdd, 0xba, 0xc1, 0x7a, 0xe5, 0x05, 0x59,
    0xdb, 0xa2, 0x56, 0x5e, 0x90, 0x9e, 0x56, 0x83, 0x56, 0x0c, 0xf2, 0xf0,
    0x45, 0x87, 0x41, 0x6e, 0x3a, 0xb8, 0x1c, 0xc2, 0x3f, 0x73, 0x6f, 0x08,
    0x21, 0xa3, 0x26, 0x39, 0xd9, 0x78, 0x83, 0x14, 0x5b, 0x79, 0xc2, 0xc0,
    0x77, 0x0a, 0x62, 0x53, 0xb1, 0xbf, 0x19, 0xc5, 0x95, 0x4f, 0xe4, 0x47,
    0x6b, 0x6b, 0x98, 0x9d, 0xd1, 0x5b, 0x92, 0x8c, 0x94, 0x21, 0x78, 0xa0,
    0xb4, 0x6c, 0xca, 0xe6, 0xfe, 0x5c, 0xb3, 0xf5, 0x7a, 0xf5, 0x2f, 0xba,
    0x3c, 0x6c, 0xcb, 0x7c, 0xff, 0x64, 0x30, 0x18, 0x58, 0x0a, 0xaf, 0x86,
    0x32, 0xfe, 0x60, 0x8b, 0x32, 0xbe, 0xbb, 0x98, 0xea, 0xea, 0x8c, 0x36,
    0x1c, 0xfa, 0xbf, 0xb9, 0x70, 0x87, 0x9b, 0x5c, 0xe9, 0x90, 0x8f, 0x34,
    0xfa, 0xec, 0xc9, 0x10, 0x08, 0xae, 0x78, 0x35, 0xe3, 0x0d, 0x70, 0xd8,
    0xd7, 0x4b, 0x0e, 0xe0, 0x3b, 0xb8, 0x6b, 0xd3, 0xfd, 0xc0, 0xea, 0x5e,
    0x9e, 0xdc, 0xb9, 0x92, 0x0a, 0x2e, 0x1a, 0xe7, 0xb9, 0xa3, 0xdc, 0xe8,
    0x30, 0x13, 0xe5, 0x3f, 0x5f, 0x91, 0xbb, 0x59, 0x81, 0x97, 0x84, 0xa3,
    0x4d, 0xac, 0xd6, 0xba, 0xff, 0x9d, 0xff, 0xfc, 0xf0, 0xbb, 0x75, 0xfb,
    0x7a, 0x74, 0x2c, 0xab, 0x92, 0x03, 0x9c, 0xcd, 0x66, 0x87, 0x3e, 0x3c,
    0x49, 0x94, 0x24, 0xd5, 0x83, 0x22, 0x5b, 0x7f, 0x95, 0x67, 0xac, 0xb8,
    0x6f, 0x9d, 0xde, 0x6e, 0xa3, 0x1a, 0xbd, 0x0c, 0xea, 0x74, 0x5e, 0x81,
    0x4e, 0x8f, 0xf1, 0xf7, 0xea, 0x2e, 0xff, 0x59, 0x0a, 0x82, 0x6e, 0x0d,
    0x0e, 0x47, 0xc0, 0xb9, 0x79, 0x6b, 0xed, 0x25, 0x52, 0x53, 0x6d, 0xe5,
    0x77, 0xd1, 0x50, 0x5e, 0x58, 0x0e, 0x0e, 0xbe, 0xed, 0x8f, 0x20, 0x77,
    0x5d, 0x1a, 0x71, 0xb7, 0x32, 0x5b, 0x0d, 0x65, 0x8b, 0xb1, 0x72, 0xfa,
    0xdb, 0xad, 0x9d, 0x4d, 0xa3, 0xd5, 0x37, 0xb1, 0x34, 0x2d, 0xa6, 0x76,
    0x3b, 0xff, 0xf7, 0xb2, 0x6f, 0x33, 0x8d, 0x5b, 0xeb, 0x6d, 0x94, 0x2e,
    0x1f, 0x10, 0x28, 0xa1, 0x6b, 0x29, 0x2a, 0x6f, 0x68, 0x87, 0xc0, 0x6d,
    0x21, 0x4c, 0x31, 0x17, 0xbd, 0x78, 0x41, 0xd3, 0xa4, 0xb9, 0x85, 0xdd,
    0xc3, 0x39, 0x1c, 0xda, 0x87, 0xa3, 0xb1, 0xce, 0xe1, 0x73, 0xdb, 0x7b,
    0xa5, 0x69, 0xbc, 0xb4, 0x4d, 0xf7, 0xb6, 0x49, 0xfd, 0xe0, 0xd0, 0x46,
    0x54, 0xdf, 0xae, 0x60, 0xb2, 0x4e, 0xc3, 0xaf, 0xb5, 0x7d, 0xbe, 0xa5,
    0x56, 0xf0, 0x1b, 0x1c, 0xe9, 0x03, 0xbb, 0xf9, 0x55, 0x46, 0xb3, 0x63,
    0xe6, 0x5e, 0xdb, 0x24, 0x40, 0xca, 0x08, 0xd2, 0x68, 0xd5, 0xa9, 0x37,
    0x36, 0xf6, 0x8f, 0xad, 0xc6, 0x8a, 0x2d, 0x0a, 0xc5, 0x6d, 0xfd, 0xb7,
    0x74, 0x62, 0x5f, 0xd7, 0x2d, 0x71, 0x51, 0xc0, 0xd7, 0xd8, 0x3b, 0xb7,
    0xbb, 0xcd, 0x48, 0x8f, 0x1e, 0x2f, 0xa2, 0x94, 0x25, 0x38, 0x6c, 0x4a,
    0x47, 0xcd, 0xe3, 0x3d, 0x58, 0xe8, 0xf8, 0x18, 0x04, 0xbd, 0xf1, 0xb1,
    0x02, 0x8a, 0x71, 0xf5, 0x29, 0x4a, 0x05, 0x10, 0xea, 0xd3, 0x93, 0x4f,
    0x74, 0xa6, 0x8b, 0x46, 0xaa, 0x60, 0xf3, 0xad, 0xf7, 0x77, 0xf8, 0x92,
    0x23, 0x93, 0x3f, 0xd5, 0x7c, 0xf8, 0x7d, 0x91, 0x2d, 0x89, 0x4e, 0x56,
    0x0c, 0x21, 0x8e, 0xaf, 0xde, 0xfc, 0xf1, 0xd5, 0xe7, 0xb7, 0x7f, 0x40,
    0xef, 0x5e, 0xfd, 0x70, 0x66, 0x45, 0x75, 0xb6, 0x5a, 0xb5, 0xd3, 0xef,
    0xee, 0xe1, 0xf1, 0xb1, 0xf4, 0x06, 0xb2, 0x46, 0x85, 0x7c, 0xa8, 0x48,
    0x1f, 0x14, 0x7c, 0x81, 0x0d, 0xb7, 0xa6, 0x38, 0x56, 0x21, 0x33, 0xb2,
    0x6d, 0xf5, 0xae, 0xfa, 0xf8, 0xd6, 0x78, 0x4b, 0xa0, 0x65, 0xb3, 0xba,
    0xfe, 0x92, 0xe0, 0x58, 0x7d, 0x1d, 0xa7, 0xf9, 0xce, 0x84, 0x99, 0x8c,
    0xf5, 0x97, 0x87, 0xe5, 0xdb, 0x3d, 0x86, 0xaf, 0xed, 0x4c, 0xe8, 0x0d,
    0x7f, 0x24, 0x9d, 0x46, 0x98, 0x26, 0x91, 0xe5, 0xbd, 0x53, 0x7d, 0xcc,
    0xa1, 0x2a, 0x31, 0x9f, 0x1c, 0x1e, 0xeb, 0x6f, 0x22, 0xbf, 0xcb, 0x24,
    0x10, 0xfe, 0xfa, 0xb7, 0xff, 0x32, 0xdb, 0x61, 0x98, 0x8b, 0x24, 0xb2,
    0x03, 0xac, 0x75, 0x57, 0x96, 0xcf, 0x4c, 0x63, 0xbf, 0x3e, 0xeb, 0x4a,
    0xf2, 0x5b, 0x0e, 0x48, 0x36, 0x74, 0xc6, 0xbf, 0xfe, 0x9f, 0xff, 0xdd,
    0x8a, 0x56, 0x55, 0xb0, 0x00, 0xe7, 0x0f, 0x88, 0x77, 0x7c, 0xcc, 0x8d,
    0x97, 0x51, 0x12, 0xa9, 0x55, 0x98, 0x19, 0x03, 0x6e, 0x45, 0xc6, 0x93,
    0xc0, 0xb1, 0x17, 0xdd, 0xbe, 0x3d, 0xc7, 0xb2, 0xdf, 0x5c, 0xbf, 0xd5,
    0x4e, 0x20, 0x63, 0x1b, 0x6a, 0x8e, 0xf7, 0xf2, 0xf1, 0xf1, 0x62, 0xd0,
    0x05, 0x83, 0xc5, 0xa0, 0xf1, 0x65, 0x08, 0x98, 0x44, 0xe3, 0x4b, 0xa5,
    0xba, 0x4f, 0x81, 0xe7, 0x2a, 0x24, 0x56, 0x66, 0x02, 0x81, 0x1a, 0xd6,
    0x6e, 0xa9, 0xf6, 0x30, 0xbd, 0x6a, 0x0d, 0x0a, 0x49, 0xe8, 0xd9, 0xd6,
    0x25, 0x76, 0x5d, 0xf5, 0xd2, 0x32, 0x65, 0x43, 0x6b, 0xfb, 0xd1, 0xde,
    0xdb, 0x33, 0xf9, 0x21, 0xd4, 0xea, 0xf3, 0xa9, 0xd5, 0x56, 0xea, 0x9d,
    0x68, 0xec, 0xa2, 0x01, 0x41, 0x3d, 0x02, 0x6c, 0xe3, 0xb7, 0x6f, 0x9e,
    0xf5, 0xd5, 0x1b, 0xbd, 0x79, 0xc3, 0xed, 0x10, 0xbd, 0x18, 0xb6, 0x37,
    0xb1, 0xf1, 0xe5, 0x4a, 0x6b, 0x0f, 0xfb, 0x90, 0x3c, 0x69, 0xf0, 0xdc,
    0xfa, 0x0c, 0x44, 0x73, 0x13, 0xcd, 0x34, 0xa4, 0x8b, 0x8a, 0xfc, 0x30,
    0xf9, 0x2a, 0x87, 0x40, 0xa2, 0xc1, 0xf3, 0x6d, 0x81, 0xcb, 0x01, 0x82,
    0xb8, 0x60, 0x9d, 0x36, 0x29, 0xc6, 0x0c, 0xa9, 0xc0, 0x07, 0x84, 0x51,
    0x42, 0x67, 0x33, 0x02, 0xf6, 0x4a, 0xf8, 0x5c, 0x60, 0x20, 0x17, 0x51,
    0xef, 0x82, 0xc6, 0xb7, 0x36, 0xa8, 0x9b, 0x18, 0x77, 0x5d, 0x43, 0x9b,
    0x4a, 0xeb, 0x8f, 0x0e, 0x6e, 0x5d, 0x7e, 0xeb, 0x1b, 0x8a, 0x34, 0x89,
    0x9a, 0x51, 0xbc, 0x72, 0x1d, 0x97, 0x77, 0xe8, 0xe4, 0xb4, 0xae, 0x6b,
    0x36, 0xaa, 0x13, 0x58, 0xaf, 0x47, 0x97, 0x32, 0xdf, 0x58, 0x85, 0xf2,
    0xcb, 0xcf, 0x8d, 0xd4, 0x11, 0xd6, 0x1b, 0xc1, 0x4e, 0xc7, 0x3e, 0xb7,
    0xa2, 0xdf, 0x65, 0xac, 0xbd, 0x1d, 0x04, 0x8e, 0x14, 0x01, 0x6d, 0xc3,
    0x6c, 0xe3, 0xbb, 0x81, 0x06, 0x4e, 0x2a, 0x8b, 0x97, 0xa9, 0x0d, 0xd4,
    0xd3, 0x3e, 0x5c, 0xf9, 0xac, 0xc1, 0xc6, 0x36, 0xaa, 0x99, 0x31, 0xea,
    0xdd, 0xb6, 0x9d, 0xbd, 0xb6, 0x00, 0x5c, 0x1d, 0x22, 0xa5, 0xc0, 0x6d,
    0xbf, 0x3d, 0x92, 0x6e, 0xdd, 0xfa, 0x2e, 0xe0, 0xf1, 0xde, 0x62, 0xdf,
    0xcc, 0xcf, 0xce, 0x7b, 0x5c, 0x9f, 0x45, 0x33, 0x5a, 0xba, 0xbe, 0x76,
    0x44, 0x8c, 0x5f, 0xcb, 0x57, 0x6d, 0xfc, 0x54, 0x65, 0x8d, 0x6e, 0xc4,
    0x1a, 0xab, 0x6e, 0xcd, 0xd2, 0x1a, 0x75, 0x96, 0x38, 0x6e, 0xbe, 0xad,
    0x17, 0x5e, 0x31, 0x1c, 0x63, 0x9d, 0x27, 0xdb, 0x06, 0x22, 0xd6, 0xc8,
    0xae, 0x6e, 0x22, 0x98, 0xad, 0xcc, 0xea, 0xfb, 0x83, 0xe6, 0x77, 0x9a,
    0xde, 0xc9, 0xa8, 0x17, 0x34, 0x2b, 0xb2, 0xa5, 0x0c, 0xaf, 0x37, 0x61,
    0xe9, 0xcd, 0x5c, 0xfd, 0xb5, 0xab, 0xd6, 0xf8, 0x63, 0xae, 0x22, 0x00,
    0xd3, 0x3b, 0x34, 0x87, 0x8f, 0xd8, 0x50, 0xb8, 0x17, 0x32, 0x31, 0x14,
    0x29, 0xac, 0x90, 0xc3, 0xf6, 0x57, 0x6a, 0xf4, 0x5c, 0xeb, 0x0f, 0x69,
    0x36, 0x67, 0x09, 0xf1, 0x6f, 0xea, 0xc2, 0xc9, 0x9c, 0xca, 0x09, 0x89,
    0xe9, 0x12, 0xa7, 0x8d, 0xa9, 0x0e, 0x5e, 0x0e, 0x83, 0xc1, 0xf3, 0xa3,
    0x60, 0x10, 0x0c, 0xfa, 0xfd, 0x76, 0xbf, 0xf6, 0x1e, 0xe8, 0xef, 0xe4,
    0x58, 0x89, 0xd0, 0x4d, 0x38, 0x76, 0x2b, 0xa3, 0x39, 0xc4, 0x33, 0x8e,
    0x3f, 0xca, 0xa0, 0xc6, 0xac, 0xd8, 0x92, 0xf5, 0x1c, 0xc3, 0x27, 0x78,
    0xc6, 0xcd, 0x0f, 0xf1, 0xb8, 0x8c, 0x10, 0xb9, 0x69, 0x59, 0x81, 0xe4,
    0x77, 0x8e, 0x64, 0x12, 0x94, 0xed, 0xa9, 0xcd, 0xcd, 0x1c, 0x75, 0xf4,
    0x5c, 0x73, 0xe5, 0x26, 0xa6, 0x4e, 0x1d, 0x77, 0xf5, 0x04, 0x4c, 0x73,
    0xac, 0x93, 0x5a, 0x41, 0xc6, 0xb8, 0xba, 0x5e, 0xf3, 0xf8, 0x08, 0x84,
    0xc1, 0x5d, 0xa6, 0x98, 0x5d, 0x21, 0x3a, 0x43, 0x2b, 0xb6, 0xe2, 0xf0,
    0xb9, 0xc1, 0x6a, 0xe4, 0x36, 0x6d, 0xb0, 0xbd, 0x58, 0x9a, 0xd0, 0xad,
    0x82, 0xef, 0x5b, 0x88, 0xca, 0x7c, 0x92, 0xdc, 0x84, 0x94, 0x77, 0xe8,
    0x2e, 0xf4, 0x57, 0xdd, 0x79, 0xcb, 0xf0, 0x6e, 0x50, 0x8e, 0x55, 0xf2,
    0x9b, 0xae, 0xf0, 0x2b, 0xf3, 0x0d, 0x79, 0xf3, 0x21, 0xda, 0xea, 0x1e,
    0x5b, 0xdf, 0x93, 0x1c, 0x3f, 0x7c, 0x79, 0xe1, 0xc2, 0xd6, 0x29, 0x0a,
    0x9a, 0xb7, 0x15, 0xca, 0xdb, 0x8b, 0xaa, 0x27, 0x58, 0x33, 0xf4, 0x7a,
    0x7a, 0x8b, 0x03, 0xf5, 0xe5, 0x34, 0x20, 0x50, 0x07, 0xe3, 0xe3, 0x55,
    0x2a, 0x51, 0x9d, 0x2c, 0x19, 0x1f, 0xef, 0xad, 0xec, 0x0d, 0x97, 0x95,
    0x4f, 0xeb, 0x6f, 0x3e, 0xd6, 0xf5, 0xab, 0xaf, 0xf4, 0x35, 0x9a, 0x34,
    0xfe, 0xbf, 0x89, 0x06, 0x2b, 0xbe, 0xee, 0xef, 0xa2, 0xc0, 0xa7, 0x8b,
    0x3b, 0x2e, 0xbf, 0x7a, 0x46, 0x35, 0x6f, 0xa8, 0x88, 0xb0, 0xe1, 0x14,
    0x6b, 0x9a, 0xbb, 0x81, 0xa8, 0x19, 0xea, 0x62, 0x7d, 0xea, 0xa4, 0x4d,
    0x84, 0x8f, 0xaa, 0x7e, 0xad, 0x0f, 0x50, 0x54, 0x4d, 0xac, 0x50, 0x78,
    0x9b, 0x20, 0x69, 0x9e, 0x84, 0x5e, 0x93, 0x28, 0xcf, 0x24, 0x69, 0x3b,
    0x53, 0x5f, 0xe8, 0x6d, 0xb3, 0x8d, 0x55, 0xd2, 0x02, 0x89, 0xaf, 0xe5,
    0xaf, 0x0e, 0x38, 0x6e, 0x21, 0xf8, 0x26, 0x07, 0xa2, 0x6f, 0x92, 0xe5,
    0x01, 0xa6, 0x33, 0xf9, 0x10, 0x7d, 0xe0, 0x2b, 0x59, 0xcd, 0x3e, 0xc8,
    0x97, 0x12, 0x60, 0x24, 0xd8, 0x83, 0x4a, 0x59, 0x67, 0xf2, 0xe3, 0x35,
    0xc5, 0xaf, 0x56, 0x25, 0x29, 0xa1, 0xd9, 0xe6, 0x9a, 0x2a, 0xea, 0xcc,
    0x28, 0xe6, 0xcc, 0x2c, 0xd6, 0xc4, 0x64, 0x43, 0x91, 0xf3, 0xd2, 0x5f,
    0x5c, 0xf0, 0x21, 0x8f, 0x27, 0xbe, 0xab, 0x53, 0xc2, 0x17, 0x68, 0xd9,
    0xfe, 0x72, 0x42, 0xb0, 0x91, 0x46, 0xd7, 0x06, 0xe8, 0x07, 0x61, 0x47,
    0x45, 0xa4, 0x7d, 0x3b, 0xc0, 0xbc, 0xae, 0x78, 0x12, 0x03, 0x2b, 0x67,
    0xb2, 0x27, 0x1b, 0x56, 0xda, 0x10, 0x63, 0xa9, 0xa1, 0x75, 0xaf, 0x97,
    0xe3, 0x3a, 0x13, 0x80, 0x25, 0x3f, 0x7d, 0x94, 0x0a, 0x19, 0xf8, 0x38,
    0x4b, 0x4a, 0x50, 0xb1, 0x62, 0x8c, 0xb2, 0x79, 0xfd, 0xcd, 0x16, 0x40,
    0xae, 0x0a, 0x51, 0x92, 0x64, 0x4e, 0xe4, 0x77, 0x97, 0xb5, 0x78, 0x25,
    0xfb, 0xb4, 0xbf, 0xcf, 0xa0, 0x36, 0xc8, 0xa2, 0xa0, 0xdd, 0xcc, 0x09,
    0xa8, 0xe1, 0x4d, 0x64, 0x44, 0xbb, 0x16, 0x89, 0xb2, 0x7a, 0x1e, 0x43,
    0xd2, 0x82, 0x3a, 0x89, 0x82, 0xfd, 0x95, 0x82, 0x7a, 0xa5, 0x95, 0x56,
    0x75, 0xf3, 0xd0, 0x75, 0x73, 0x35, 0xba, 0x5a, 0x2e, 0x7c, 0x9f, 0x62,
    0x20, 0x3f, 0x4e, 0x31, 0xe8, 0xf7, 0x35, 0x91, 0x81, 0x5f, 0x0d, 0xd6,
    0xbc, 0x4e, 0x9c, 0x60, 0x25, 0x72, 0xb0, 0x3f, 0x64, 0xd7, 0xc9, 0x36,
    0x36, 0x06, 0x7f, 0xaf, 0xf6, 0x97, 0xe9, 0xcc, 0xc2, 0xea, 0x4b, 0xd4,
    0xca, 0x47, 0x21, 0xbf, 0x51, 0x77, 0xa7, 0x32, 0x34, 0xa8, 0x63, 0xb3,
    0xbf, 0xbf, 0xbd, 0x01, 0x90, 0x34, 0xbf, 0xf8, 0x9b, 0x20, 0x49, 0xf2,
    0x62, 0xa0, 0x5a, 0x34, 0xb0, 0x54, 0x09, 0xa1, 0xdb, 0x81, 0xa9, 0xc3,
    0xec, 0x58, 0x3c, 0x5d, 0x4d, 0xdf, 0x55, 0x18, 0x7d, 0x8b, 0x72, 0x13,
    0x9d, 0x68, 0x68, 0xfc, 0x27, 0x99, 0x0f, 0xd2, 0xe4, 0x1d, 0xda, 0x42,
    0xe7, 0x21, 0xbe, 0x7d, 0x2c, 0xb3, 0x15, 0x7c, 0x95, 0x8e, 0xbf, 0xab,
    0x98, 0x9c, 0xe6, 0x37, 0x32, 0x2a, 0x18, 0x30, 0xc1, 0xfc, 0xdb, 0xd9,
    0x2e, 0x19, 0xfe, 0xaa, 0x95, 0x0d, 0x0d, 0x4a, 0x9f, 0xcd, 0x66, 0xf6,
    0x31, 0xb7, 0x53, 0xbc, 0x75, 0xd9, 0x31, 0x9d, 0xe2, 0xcd, 0xea, 0x50,
    0x67, 0x7a, 0xb3, 0x60, 0xa1, 0x31, 0xfb, 0x57, 0x8a, 0xfd, 0x82, 0xa3,
    0xae, 0x37, 0x50, 0x67, 0x3b, 0x68, 0xed, 0x49, 0xb2, 0x88, 0xf3, 0xf1,
    0x2b, 0xf3, 0x4d, 0x74, 0xe4, 0xbe, 0xf9, 0xdd, 0xeb, 0x53, 0x6f, 0xcb,
    0xfe, 0x01, 0x4c, 0xd1, 0x58, 0x5e, 0x74, 0x1a, 0xa3, 0x93, 0xd3, 0xeb,
    0x83, 0x07, 0xf6, 0x51, 0x9f, 0xb2, 0xfe, 0xd4, 0x54, 0x0d, 0x56, 0xb5,
    0x21, 0x51, 0xad, 0xd3, 0x3c, 0xd6, 0x9f, 0x73, 0x7b, 0x65, 0x2a, 0x98,
    0xc8, 0x7f, 0x0d, 0xb6, 0x7c, 0xe3, 0x37, 0xdd, 0x3a, 0xd0, 0xf3, 0xb1,
    0x93, 0xc1, 0xe2, 0xec, 0xec, 0xe4, 0x4d, 0x7d, 0x72, 0x55, 0xbe, 0x85,
    0xf6, 0xae, 0x75, 0x1b, 0xe6, 0x1d, 0x6e, 0xae, 0x91, 0x22, 0xa1, 0xc5,
    0xce, 0x35, 0x98, 0xb6, 0x2f, 0x3a, 0xc8, 0x3e, 0x01, 0x96, 0x4d, 0xf2,
    0x6e, 0xce, 0x63, 0xf8, 0xe4, 0x3a, 0xfb, 0x44, 0x97, 0x53, 0x6e, 0xb7,
    0xff, 0xa0, 0xd2, 0x51, 0x34, 0x1a, 0xeb, 0x14, 0x15, 0x5f, 0x6f, 0xfc,
    0x93, 0xca, 0x59, 0xd1, 0x68, 0xac, 0xf3, 0x58, 0x7c, 0xbd, 0xf1, 0x9b,
    0x0f, 0x67, 0x88, 0x93, 0xe2, 0x9a, 0x14, 0x8d, 0xf6, 0x09, 0x7b, 0xc4,
    0xac, 0x65, 0x8c, 0xf6, 0x0c, 0xc7, 0x64, 0xd3, 0xee, 0x5a, 0xd9, 0x2b,
    0x1e, 0xd8, 0x5c, 0x23, 0x9e, 0xb4, 0xbb, 0xfe, 0xe1, 0xe3, 0xc7, 0xcf,
    0xc8, 0x64, 0xc9, 0xb0, 0x20, 0x7f, 0x43, 0xe2, 0x8c, 0xad, 0x98, 0x01,
    0xbd, 0x3a, 0xdd, 0x02, 0xfd, 0x15, 0xae, 0x79, 0x23, 0x15, 0xa2, 0x75,
    0x92, 0x33, 0xb8, 0x32, 0xc8, 0x5d, 0xb1, 0x3c, 0x5d, 0xcd, 0xd1, 0xbb,
    0x57, 0x1f, 0xd4, 0x07, 0xa9, 0xbd, 0xaf, 0xde, 0x8d, 0xea, 0x3b, 0x2f,
    0x15, 0x22, 0x55, 0xa6, 0x69, 0x20, 0xa6, 0x6a, 0x2e, 0x0b, 0xcc, 0x11,
    0xcb, 0xd0, 0x8d, 0x4a, 0x76, 0x8b, 0xd3, 0x14, 0x6c, 0x21, 0x01, 0x90,
    0x57, 0x04, 0x6b, 0x0d, 0xd1, 0xe1, 0xaf, 0x7f, 0xfb, 0xcf, 0xc1, 0x01,
    0xaf, 0x16, 0xed, 0xa3, 0x81, 0x2c, 0x7a, 0x09, 0x45, 0x9c, 0x08, 0x1f,
    0x0d, 0xfb, 0x1c, 0x29, 0x51, 0x36, 0xd0, 0x40, 0x6d, 0xea, 0xaa, 0x8f,
    0xd2, 0x28, 0xad, 0xd4, 0x08, 0x25, 0xad, 0x45, 0x81, 0xf8, 0x03, 0x2f,
    0x73, 0x5c, 0x80, 0x7b, 0x5e, 0x9e, 0xdd, 0xe8, 0x8f, 0xe7, 0xb2, 0x0c,
    0xd6, 0xa8, 0xb8, 0x24, 0x8b, 0x71, 0xb3, 0x33, 0xdc, 0xb4, 0x04, 0x8c,
    0x6d, 0x59, 0x3c, 0xbe, 0x42, 0x86, 0x54, 0xce, 0x8b, 0xdf, 0x44, 0x85,
    0x24, 0x23, 0x86, 0x6b, 0xa5, 0xc0, 0x70, 0xac, 0xf3, 0x3f, 0xa8, 0xd1,
    0x35, 0x80, 0x3c, 0x40, 0x92, 0x36, 0xa8, 0x79, 0x54, 0x92, 0x90, 0xf1,
    0x5b, 0xf9, 0x07, 0x15, 0x24, 0xc1, 0x32, 0x11, 0x97, 0x9a, 0x65, 0xbd,
    0x14, 0x9b, 0x67, 0xb1, 0x0c, 0xae, 0xe3, 0x13, 0x69, 0x8d, 0x33, 0xd5,
    0xeb, 0x5b, 0xa3, 0x8a, 0x25, 0xac, 0x43, 0x75, 0x9d, 0xbb, 0x26, 0xea,
    0x66, 0x12, 0x69, 0x43, 0x4f, 0xde, 0xd0, 0xf8, 0xa8, 0xf5, 0x71, 0x94,
    0x81, 0x7c, 0x67, 0x2e, 0x8d, 0xe6, 0x65, 0x37, 0xee, 0x6e, 0x42, 0xf1,
    0x9c, 0x65, 0x10, 0x84, 0xfc, 0xf7, 0x89, 0x1b, 0x67, 0xab, 0x1c, 0x56,
    0x60, 0xf6, 0xf9, 0x4d, 0xdd, 0xad, 0xbd, 0xbf, 0xf5, 0x36, 0x5a, 0x49,
    0x34, 0xc6, 0x9f, 0xd4, 0x6f, 0x94, 0x66, 0xf3, 0x36, 0x97, 0x9f, 0x03,
    0x8f, 0x9e, 0x44, 0x50, 0xab, 0xce, 0x5b, 0x61, 0xd2, 0x77, 0x1c, 0xef,
    0xe5, 0x05, 0xb1, 0xd7, 0xb5, 0xa7, 0x54, 0xc2, 0x9a, 0x5f, 0x96, 0x26,
    0x53, 0xf5, 0xf9, 0x40, 0x2d, 0x8a, 0x74, 0xc4, 0x0f, 0x73, 0xec, 0xca,
    0xb0, 0xb0, 0x07, 0x81, 0xfb, 0xe3, 0xff, 0x0f, 0xd4, 0x2d, 0xbc, 0x7b,
    0x74, 0x9f, 0x00, 0x00
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
    // The web server and serialized PixLite network worker run on different
    // tasks. They must never tokenize into the same scratch memory: otherwise
    // a status poll can overwrite a configuration request while it is being
    // validated, producing a partially applied form.
    configTokens = allocate<JsonToken>(CONFIG_TOKEN_CAPACITY);
    pixliteTokens = allocate<JsonToken>(CONFIG_TOKEN_CAPACITY);
    pixliteResponse = static_cast<char *>(heap_caps_calloc(
        PIXLITE_RESPONSE_LIMIT + 1,
        1,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    ready = selectMemoryMode(
                true,
                devices && media && mediaCounts && pixliteStatuses &&
                    logs && configTokens && pixliteTokens && pixliteResponse) ==
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
  JsonToken *configTokens = nullptr;
  JsonToken *pixliteTokens = nullptr;
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
    heap_caps_free(configTokens);
    heap_caps_free(pixliteTokens);
    heap_caps_free(pixliteResponse);
    devices = nullptr;
    media = nullptr;
    mediaCounts = nullptr;
    pixliteStatuses = nullptr;
    logs = nullptr;
    configTokens = nullptr;
    pixliteTokens = nullptr;
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
#include <apps/dhcpserver/dhcpserver_options.h>
#include <esp_attr.h>
#include <esp_netif_net_stack.h>
#include <esp_system.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>







namespace advatek {

// RTC slow memory survives a software restart but not loss of power. A BOOT
// request uses this one-time marker to enter direct-Ethernet recovery before
// the normal network task starts. The marker is consumed immediately so a
// fault during recovery always returns to the configured uplink on next boot.
inline constexpr uint32_t DIRECT_ETHERNET_RECOVERY_MAGIC = 0x41545245UL;
static RTC_NOINIT_ATTR uint32_t directEthernetRecoveryBootMarker;

class NetworkManager {
 public:
  NetworkManager(const BoardProfile &board, AppConfig &config, Diagnostics &log)
      : board_(board), config_(config), log_(log) {}

  static bool directEthernetRecoveryBootRequested() {
    return esp_reset_reason() == ESP_RST_SW &&
           directEthernetRecoveryBootMarker ==
               DIRECT_ETHERNET_RECOVERY_MAGIC;
  }

  void begin(const char *apName) {
    copyText(apName_, sizeof(apName_), apName);
    bootDirectEthernetRecovery_ =
        directEthernetRecoveryBootRequested();
    directEthernetRecoveryBootMarker = 0;
    if (bootDirectEthernetRecovery_) {
      beginEthernetRecoveryBoot();
    } else if (config_.network.uplink == UplinkMode::Ethernet) {
      // Keep an unassociated station interface initialised while Ethernet is
      // operational. On the ESP32-S3/Arduino-ESP32 3.3.10 combination tested
      // here, starting the Wi-Fi driver for the first time after W5500 and the
      // HTTP task were already running could fail. BOOT recovery can now
      // change this idle interface to AP mode without attempting a late radio
      // initialisation. No credentials are used and this is not a Wi-Fi
      // fallback: Ethernet remains the only operational uplink.
      WiFi.persistent(false);
      const bool recoveryRadioReady = WiFi.mode(WIFI_MODE_STA);
      WiFi.STA.disconnect(false, false);
      log_.add(
          recoveryRadioReady ? LogLevel::Info : LogLevel::Warning,
          recoveryRadioReady
              ? "Wi-Fi recovery radio initialized idle; Ethernet remains the only operational uplink"
              : "Wi-Fi recovery radio could not initialize at boot");
      beginEthernet();
    } else {
      // Always drive Station mode from the schema-versioned application
      // configuration. Arduino's separate persistent Wi-Fi store can retain
      // stale credentials/mode from a previous sketch and leave begin() at
      // WL_DISCONNECTED without producing an association attempt.
      WiFi.persistent(false);
      WiFi.mode(WIFI_MODE_STA);
      beginWifiStation();
    }
  }

  void loop() {
    // Wi-Fi events run on the Arduino network event task. The callback only
    // copies the small reason code; diagnostics are written here on the main
    // network loop so the ring buffer never has concurrent writers.
    if (wifiDisconnectPending_) {
      wifiDisconnectPending_ = false;
      lastWifiDisconnectReason_ = pendingWifiDisconnectReason_;
      const bool initialDriverRetry =
          !wasOnline_ &&
          lastWifiDisconnectReason_ == WIFI_REASON_AUTH_EXPIRE;
      if (initialDriverRetry) {
        log_.addf(
            LogLevel::Info,
            "Wi-Fi station initial connection retry (reason %u)",
            lastWifiDisconnectReason_);
      } else {
        log_.addf(
            LogLevel::Warning,
            "Wi-Fi station disconnected: %s (reason %u)",
            wifiDisconnectReasonName(lastWifiDisconnectReason_),
            lastWifiDisconnectReason_);
      }
    }
    if (apRunning_ || ethernetRecoveryRunning_) {
      dns_.processNextRequest();
      if (apStopAt_ && dueAt(millis(), apStopAt_)) stopAccessPoint();
      // The lwIP DHCP server cannot start while the W5500 interface is down.
      // Direct recovery is deliberately armed before the user connects the
      // computer, so defer DHCP until that direct cable creates link.
      if (ethernetRecoveryRunning_ &&
          !ethernetDhcpServer_ &&
          ETH.linkUp() &&
          ethernetRecoveryDhcpAttempts_ < 3 &&
          (!ethernetRecoveryDhcpNextAttempt_ ||
           dueAt(millis(), ethernetRecoveryDhcpNextAttempt_))) {
        ++ethernetRecoveryDhcpAttempts_;
        if (startDirectEthernetDhcp(IPAddress(192, 168, 4, 1))) {
          dns_.start(53, "*", IPAddress(192, 168, 4, 1));
          log_.add(
              LogLevel::Warning,
              "Direct Ethernet recovery DHCP active at 192.168.4.1");
        } else if (ethernetRecoveryDhcpAttempts_ < 3) {
          ethernetRecoveryDhcpNextAttempt_ = millis() + 500UL;
          log_.addf(
              LogLevel::Warning,
              "Direct Ethernet recovery DHCP attempt %u of 3 failed (lwIP %d)",
              ethernetRecoveryDhcpAttempts_,
              static_cast<int>(ethernetRecoveryDhcpLastError_));
        } else {
          log_.addf(
              LogLevel::Error,
              "Direct Ethernet recovery DHCP failed after 3 attempts (lwIP %d)",
              static_cast<int>(ethernetRecoveryDhcpLastError_));
          recoveryRestartAt_ = millis() + 1500UL;
        }
      }
      if (ethernetRecoveryStopAt_ &&
          dueAt(millis(), ethernetRecoveryStopAt_)) {
        stopEthernetRecovery();
      }
    }
    if (recoveryRestartAt_ && dueAt(millis(), recoveryRestartAt_)) {
      recoveryRestartAt_ = 0;
      ESP.restart();
    }
    const bool nowOnline = uplinkConnected();
    if (nowOnline != wasOnline_) {
      wasOnline_ = nowOnline;
      if (nowOnline) {
        if (config_.network.uplink == UplinkMode::WifiStation) {
          lastWifiDisconnectReason_ = 0;
        }
        activeInterface().setDefault();
        startMdns();
        log_.addf(LogLevel::Info, "Operational uplink connected at %s", localIP().toString().c_str());
      } else {
        log_.add(LogLevel::Warning, "Operational uplink disconnected");
      }
    }
  }

  bool uplinkConnected() const {
    return usingEthernetInterface()
               ? ETH.linkUp() && ETH.localIP() != IPAddress()
               : WiFi.STA.status() == WL_CONNECTED;
  }

  NetworkInterface &activeInterface() {
    return usingEthernetInterface()
               ? static_cast<NetworkInterface &>(ETH)
               : static_cast<NetworkInterface &>(WiFi.STA);
  }

  IPAddress localIP() const {
    return usingEthernetInterface() ? ETH.localIP() : WiFi.STA.localIP();
  }

  IPAddress netmask() const {
    return usingEthernetInterface()
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
  uint8_t wifiStatusCode() const {
    return static_cast<uint8_t>(WiFi.STA.status());
  }
  const char *wifiStatusName() const {
    switch (WiFi.STA.status()) {
      case WL_IDLE_STATUS: return "idle";
      case WL_NO_SSID_AVAIL: return "network-not-found";
      case WL_SCAN_COMPLETED: return "scan-complete";
      case WL_CONNECTED: return "connected";
      case WL_CONNECT_FAILED: return "connection-failed";
      case WL_CONNECTION_LOST: return "connection-lost";
      case WL_DISCONNECTED: return "disconnected";
      case WL_STOPPED: return "stopped";
      default: return "unavailable";
    }
  }
  uint8_t wifiDisconnectReason() const {
    return lastWifiDisconnectReason_;
  }
  const char *wifiDisconnectReasonText() const {
    return wifiDisconnectReasonName(lastWifiDisconnectReason_);
  }
  int32_t wifiRssiDbm() const {
    return WiFi.STA.status() == WL_CONNECTED ? WiFi.STA.RSSI() : 0;
  }

  bool openRecoveryNetwork() {
    config_.network.accessPointPassword[0] = '\0';
    if (usesEthernetRecovery(config_.network)) {
      log_.add(
          LogLevel::Warning,
          "Direct Ethernet recovery requested; restarting into isolated recovery mode");
      directEthernetRecoveryBootMarker =
          DIRECT_ETHERNET_RECOVERY_MAGIC;
      delay(150);
      ESP.restart();
      return true;
    }
    if (!beginAccessPoint(900000UL)) return false;
    log_.add(LogLevel::Warning, "Open recovery access point enabled for 15 minutes");
    return true;
  }

 private:
  bool usingEthernetInterface() const {
    return bootDirectEthernetRecovery_ ||
           ethernetRecoveryRunning_ ||
           config_.network.uplink == UplinkMode::Ethernet;
  }

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
    if (bootDirectEthernetRecovery_) {
      log_.add(
          LogLevel::Info,
          "W5500 initialized for direct Ethernet recovery staging");
    } else {
      applyStatic(ETH, config_.network.ethernet);
      log_.addf(
          LogLevel::Info,
          "W5500 initialized for %s addressing",
          config_.network.ethernet.mode == IpMode::Static ? "static" : "DHCP");
    }
  }

  void beginWifiStation() {
    log_.add(LogLevel::Info, "Starting explicit Wi-Fi station uplink");
    if (!wifiDisconnectEventHandle_) {
      wifiDisconnectEventHandle_ = WiFi.onEvent(
          [this](arduino_event_id_t, arduino_event_info_t info) {
            pendingWifiDisconnectReason_ =
                info.wifi_sta_disconnected.reason;
            wifiDisconnectPending_ = true;
          },
          ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    }
    applyStatic(WiFi.STA, config_.network.wifi);
    WiFi.STA.setHostname(config_.network.hostname);
    WiFi.begin(config_.network.wifiSsid, config_.network.wifiPassword);
#ifdef ADVATEK_WIFI_DIAGNOSTIC_ETHERNET
    // Hardware-acceptance builds may expose the HTTP diagnostics on W5500
    // while Wi-Fi remains the selected operational uplink. Release artifacts
    // never define this flag, so this cannot become a silent field fallback.
    ethernetInitialized_ = board_.beginEthernet(board_);
    log_.add(
        ethernetInitialized_ ? LogLevel::Warning : LogLevel::Error,
        ethernetInitialized_
            ? "Hardware test only: Ethernet diagnostics path enabled"
            : "Hardware test only: Ethernet diagnostics path failed");
#endif
  }

  static const char *wifiDisconnectReasonName(uint8_t reason) {
    switch (reason) {
      case 0: return "none recorded";
      case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
      case WIFI_REASON_HANDSHAKE_TIMEOUT:
        return "security handshake timed out";
      case WIFI_REASON_BEACON_TIMEOUT:
        return "access point signal lost";
      case WIFI_REASON_NO_AP_FOUND:
        return "configured network not found";
      case WIFI_REASON_AUTH_FAIL:
        return "authentication failed";
      case WIFI_REASON_ASSOC_FAIL:
        return "association failed";
      case WIFI_REASON_CONNECTION_FAIL:
        return "connection failed";
      case WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY:
        return "network found with incompatible security";
      case WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD:
        return "network security below configured threshold";
      case WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD:
        return "network signal below threshold";
      default:
        return "connection ended";
    }
  }

  bool beginAccessPoint(uint32_t lifetimeMs) {
    const wifi_mode_t mode =
        config_.network.uplink == UplinkMode::WifiStation
            ? WIFI_MODE_APSTA
            : WIFI_MODE_AP;
    bool interfaceReady = false;
    for (uint8_t attempt = 1; attempt <= 3 && !interfaceReady; ++attempt) {
      const bool modeReady = WiFi.mode(mode);
      const bool apReady = modeReady && WiFi.AP.begin();
      interfaceReady = modeReady && apReady;
      if (!interfaceReady) {
        log_.addf(
            LogLevel::Warning,
            "Recovery Wi-Fi AP interface attempt %u failed (mode=%s ap=%s)",
            attempt,
            modeReady ? "ok" : "failed",
            apReady ? "ok" : "failed");
        delay(250);
      }
    }
    if (!interfaceReady) {
      log_.add(
          LogLevel::Error,
          "Recovery access point could not enable the Wi-Fi AP interface after 3 attempts");
      return false;
    }
    IPAddress ip(192, 168, 4, 1);
    IPAddress mask(255, 255, 255, 0);
    // Arduino-ESP32 treats the fourth server-interface address as the DHCP
    // lease-range start, not as a normal DNS field. Supplying it explicitly is
    // required for a usable recovery client lease.
    IPAddress leaseStart(192, 168, 4, 2);
    if (!WiFi.AP.config(ip, ip, mask, leaseStart, ip)) {
      log_.add(
          LogLevel::Error,
          "Recovery access point could not configure 192.168.4.1 DHCP");
      WiFi.AP.end();
      return false;
    }
    const char *password = config_.network.accessPointPassword;
    const bool created =
        password[0] && strlen(password) >= 8
            ? WiFi.AP.create(apName_, password)
            : WiFi.AP.create(apName_);
    if (!created ||
        !WiFi.AP.waitStatusBits(ESP_NETIF_STARTED_BIT, 1000) ||
        WiFi.AP.localIP() != ip ||
        WiFi.AP.SSID() != String(apName_)) {
      log_.add(
          LogLevel::Error,
          "Recovery access point did not start or advertise correctly");
      WiFi.AP.end();
      return false;
    }
    dns_.start(53, "*", ip);
    startMdns();
    apRunning_ = true;
    apStopAt_ = lifetimeMs ? millis() + lifetimeMs : 0;
    log_.addf(LogLevel::Info, "Recovery access point %s started", apName_);
    return true;
  }

  struct DhcpStartContext {
    struct netif *lwipNetif = nullptr;
    ip4_addr_t serverAddress{};
    ip4_addr_t subnetMask{};
    dhcps_t *server = nullptr;
    err_t result = ERR_ARG;
  };

  // Espressif's low-level server invokes this callback unconditionally after
  // a successful DHCP ACK. esp-netif normally registers its own callback, but
  // direct recovery intentionally uses the server beneath esp-netif because
  // an Ethernet client interface cannot change to server mode at runtime.
  static void directEthernetLeaseAssigned(
      void *,
      u8_t[4],
      u8_t[6]) {}

  static void startDhcpOnTcpipThread(void *argument) {
    DhcpStartContext *context =
        static_cast<DhcpStartContext *>(argument);
    context->server = dhcps_new();
    if (!context->server || !context->lwipNetif) {
      if (context->server) {
        dhcps_delete(context->server);
        context->server = nullptr;
      }
      context->result = ERR_MEM;
      return;
    }
    // Calling lwIP's DHCP server directly bypasses esp-netif's normal server
    // wrapper. Supply its essential subnet option explicitly; without it,
    // Windows can briefly display the otherwise valid default lease and then
    // reject it. The server's compact built-in lease/router/DNS defaults are
    // retained for maximum Arduino-ESP32 compatibility.
    context->result = dhcps_set_option_info(
        context->server,
        SUBNET_MASK,
        &context->subnetMask,
        sizeof(context->subnetMask));
    if (context->result != ERR_OK) {
      dhcps_delete(context->server);
      context->server = nullptr;
      return;
    }
    context->result = dhcps_set_new_lease_cb(
        context->server,
        directEthernetLeaseAssigned,
        nullptr);
    if (context->result != ERR_OK) {
      dhcps_delete(context->server);
      context->server = nullptr;
      return;
    }
    context->result = dhcps_start(
        context->server,
        context->lwipNetif,
        context->serverAddress);
    if (context->result != ERR_OK) {
      dhcps_delete(context->server);
      context->server = nullptr;
    }
  }

  bool startDirectEthernetDhcp(const IPAddress &address) {
    DhcpStartContext context;
    context.lwipNetif =
        static_cast<struct netif *>(
            esp_netif_get_netif_impl(ETH.netif()));
    if (!ip4addr_aton(address.toString().c_str(), &context.serverAddress)) {
      ethernetRecoveryDhcpLastError_ = ERR_ARG;
      return false;
    }
    IP4_ADDR(&context.subnetMask, 255, 255, 255, 0);
    const err_t callbackResult = tcpip_callback_with_block(
        startDhcpOnTcpipThread,
        &context,
        1);
    ethernetRecoveryDhcpLastError_ =
        callbackResult != ERR_OK ? callbackResult : context.result;
    if (callbackResult != ERR_OK ||
        context.result != ERR_OK ||
        !context.server) {
      return false;
    }
    ethernetDhcpServer_ = context.server;
    return true;
  }

  void beginEthernetRecoveryBoot() {
    log_.add(
        LogLevel::Warning,
        "Starting one-time direct Ethernet recovery boot");
    beginEthernet();
    if (!ethernetInitialized_) {
      log_.add(
          LogLevel::Error,
          "Direct Ethernet recovery unavailable because W5500 initialization failed");
      recoveryRestartAt_ = millis() + 1500UL;
      return;
    }

    // Never place a DHCP server on an installed LAN. Give W5500 three seconds
    // to report any existing link before assigning the recovery address.
    const uint32_t linkCheckUntil = millis() + 3000UL;
    while (!ETH.linkUp() && !dueAt(millis(), linkCheckUntil)) delay(25);
    if (!directEthernetRecoveryAllowed(ETH.linkUp())) {
      log_.add(
          LogLevel::Error,
          "Direct Ethernet recovery refused: disconnect the Ethernet cable before BOOT recovery");
      recoveryRestartAt_ = millis() + 1500UL;
      return;
    }

    const IPAddress address(192, 168, 4, 1);
    const IPAddress mask(255, 255, 255, 0);
    if (!ETH.config(address, address, mask, address)) {
      log_.add(LogLevel::Error, "Direct Ethernet recovery could not set 192.168.4.1");
      recoveryRestartAt_ = millis() + 1500UL;
      return;
    }
    ETH.setDefault();
    ethernetRecoveryRunning_ = true;
    bootDirectEthernetRecovery_ = false;
    ethernetRecoveryDhcpAttempts_ = 0;
    ethernetRecoveryDhcpNextAttempt_ = 0;
    ethernetRecoveryStopAt_ = millis() + 900000UL;
    log_.add(
        LogLevel::Warning,
        "Direct Ethernet recovery armed at 192.168.4.1 for 15 minutes; connect one computer directly to start DHCP");
  }

  void stopEthernetRecovery() {
    dns_.stop();
    ethernetRecoveryRunning_ = false;
    ethernetRecoveryStopAt_ = 0;
    log_.add(LogLevel::Info, "Direct Ethernet recovery expired; restarting normal uplink");
    // A restart lets ESP-IDF dispose of the temporary DHCP service within its
    // own network teardown instead of deleting lwIP state from Arduino code.
    recoveryRestartAt_ = millis() + 100UL;
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
  bool bootDirectEthernetRecovery_ = false;
  bool wasOnline_ = false;
  bool mdnsStarted_ = false;
  uint32_t apStopAt_ = 0;
  uint32_t ethernetRecoveryStopAt_ = 0;
  uint32_t ethernetRecoveryDhcpNextAttempt_ = 0;
  uint32_t recoveryRestartAt_ = 0;
  uint8_t ethernetRecoveryDhcpAttempts_ = 0;
  err_t ethernetRecoveryDhcpLastError_ = ERR_OK;
  dhcps_t *ethernetDhcpServer_ = nullptr;
  char apName_[40]{};
  bool ethernetInitialized_ = false;
  uint8_t ethernetInitAttempts_ = 0;
  char ethernetFailureReason_[96]{};
  network_event_handle_t wifiDisconnectEventHandle_ = 0;
  volatile bool wifiDisconnectPending_ = false;
  volatile uint8_t pendingWifiDisconnectReason_ = 0;
  uint8_t lastWifiDisconnectReason_ = 0;
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
        !memory_.pixliteTokens || !memory_.pixliteResponse) {
      return requestFailed(index, RESPONSE_MEMORY_UNAVAILABLE, "", "file list");
    }
    const int code = post(index, pixLiteFileListRequest(nextId()));
    if (!responseAccepted(index, code, "file list")) return false;
    uint8_t &count = memory_.mediaCounts[index];
    count = 0;
    JsonDocument document(
        response(), memory_.pixliteTokens, CONFIG_TOKEN_CAPACITY);
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
    if (!memory_.pixliteTokens) {
      return requestFailed(index, -4, "", operation);
    }

    PixLiteApiError apiError;
    if (!inspectPixLiteResponse(
            response(),
            memory_.pixliteTokens,
            CONFIG_TOKEN_CAPACITY,
            apiError)) {
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
    if (!memory_.pixliteTokens) return;
    JsonDocument document(
        responseBody, memory_.pixliteTokens, CONFIG_TOKEN_CAPACITY);
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
  virtual bool pixliteOperationsAvailable() const = 0;
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
      if (!delegate_.pixliteOperationsAvailable()) {
        sendError(503, "PixLite operations unavailable during direct Ethernet recovery");
        return;
      }
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
      if (!delegate_.pixliteOperationsAvailable()) {
        sendError(503, "PixLite operations unavailable during direct Ethernet recovery");
        return;
      }
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
      if (!delegate_.pixliteOperationsAvailable()) {
        sendError(503, "PixLite operations unavailable during direct Ethernet recovery");
        return;
      }
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
    // An empty ID means "add a new controller" in this endpoint. The shared
    // target resolver intentionally treats an empty ID as the legacy primary
    // alias for actions, so only call it here when the request supplied an ID.
    int8_t targetIndex =
        requestedId[0] ? pixLiteIndexById(config_, requestedId) : -1;
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
    if (!memory_.configTokens) {
      sendError(503, "Configuration changes disabled in degraded memory mode");
      return;
    }
    AppConfig candidate = config_;
    char error[128];
    if (!applyConfigJson(
            requestBody.c_str(),
            candidate,
            board_,
            memory_.configTokens,
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
        requestBody.c_str(), memory_.configTokens, CONFIG_TOKEN_CAPACITY);
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
    // Preserve a clear release-to-ready sequence across the one-time direct
    // recovery restart: white means "recovery is starting"; cyan means the
    // isolated Ethernet service is ready for the computer to connect.
    if (NetworkManager::directEthernetRecoveryBootRequested()) {
      showRecoveryStartingWhite();
    } else {
      showStatusOrange();
    }
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
    json += "\"wifi\":{\"statusCode\":" +
            String(network_.wifiStatusCode()) +
            ",\"status\":\"" +
            jsonEscape(network_.wifiStatusName()) +
            "\",\"disconnectReasonCode\":" +
            String(network_.wifiDisconnectReason()) +
            ",\"disconnectReason\":\"" +
            jsonEscape(network_.wifiDisconnectReasonText()) +
            "\",\"rssiDbm\":" +
            String(network_.wifiRssiDbm()) + "},";
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

  bool pixliteOperationsAvailable() const override {
    return !network_.ethernetRecoveryRunning();
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
      // A directly connected recovery computer creates physical Ethernet link
      // but cannot reach the configured PixLite LAN. Keep the recovery UI
      // responsive by suppressing all PixLite HTTP work in this mode.
      const bool pixliteNetworkReady =
          uplinkConnected && !network_.ethernetRecoveryRunning();
      if (!pixliteNetworkReady) {
        // Ensure that restoring Ethernet/Wi-Fi also refreshes every saved
        // controller after its first successful status poll.
        memset(lastStatusPollOnline, 0, sizeof(lastStatusPollOnline));
      }
      PendingAction pending{};
      if (pixliteNetworkReady) {
        for (uint8_t target = 0; target < MAX_PIXLITES; ++target) {
          if (mailboxes_[target].take(pending, millis())) {
            pixlite_.execute(target, pending.action);
          }
        }
      }
      bool mediaRequestAttempted = false;
      if (pixliteNetworkReady && config_.pixliteCount) {
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
      if (!mediaRequestAttempted &&
          pixliteNetworkReady &&
          config_.pixliteCount) {
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
        // End the orange/white hold indication deterministically on white.
        // The WS2812 retains this state through the short software restart,
        // and begin() restores white before direct recovery reaches cyan.
        showRecoveryStartingWhite();
        store_.clearAuthentication(config_);
        const bool recoveryStarted = network_.openRecoveryNetwork();
        if (recoveryStarted) {
          recoveryLedIntent_ = RecoveryIntent::None;
          recoveryFeedbackUntil_ = 0;
        } else {
          recoveryLedIntent_ = RecoveryIntent::None;
          recoveryFailureUntil_ = now + 5000;
        }
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

  void showRecoveryStartingWhite() {
    if (board_.statusLedPin < 0) return;
    // Recovery feedback must remain visible even if the normal status LED was
    // disabled or dimmed in the web interface.
    writeStatusLed(255, 255, 255);
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
    if (recoveryFailureUntil_) {
      if (dueAt(now, recoveryFailureUntil_)) {
        recoveryFailureUntil_ = 0;
        showStatusOrange();
      } else {
        writeStatusLed(((now / 250U) & 1U) == 0 ? 255 : 0, 0, 0);
        return;
      }
    }
    if (recoveryLedIntent_ == RecoveryIntent::FactoryReset) {
      writeStatusLed(255, 0, 0);
      return;
    }
    if (recoveryLedIntent_ == RecoveryIntent::ClearAuthentication) {
      if (!recoveryPressed_ && dueAt(now, recoveryFeedbackUntil_)) {
        recoveryLedIntent_ = RecoveryIntent::None;
        recoveryFeedbackUntil_ = 0;
      } else if (((now / 250U) & 1U) == 0) {
        writeStatusLed(255, 48, 0);
      } else {
        writeStatusLed(255, 255, 255);
        return;
      }
      if (recoveryLedIntent_ == RecoveryIntent::ClearAuthentication) return;
    }
    if (recoveryPressed_ &&
        recoveryLedIntent_ == RecoveryIntent::Cancelled) {
      showStatusOrange();
      return;
    }
    if (network_.accessPointRunning()) {
      const uint8_t blue = ((now / 500U) & 1U) == 0 ? 255 : 32;
      writeStatusLed(0, 0, blue);
      return;
    }
    if (network_.ethernetRecoveryRunning()) {
      const uint8_t cyan = ((now / 500U) & 1U) == 0 ? 255 : 32;
      writeStatusLed(0, cyan, cyan);
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
  uint32_t recoveryFailureUntil_ = 0;
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
