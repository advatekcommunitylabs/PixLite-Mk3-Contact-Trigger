#pragma once

#include <Arduino.h>
#include <esp_heap_caps.h>

#include "../core/JsonLite.h"
#include "../core/RuntimePolicy.h"
#include "../core/Types.h"

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
