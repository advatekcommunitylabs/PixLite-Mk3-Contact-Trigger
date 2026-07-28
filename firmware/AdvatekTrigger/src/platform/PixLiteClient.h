#pragma once

#include <HTTPClient.h>

#include "../core/JsonLite.h"
#include "../core/MediaOrderPolicy.h"
#include "../core/PixLiteProtocol.h"
#include "../core/PixLiteResponsePolicy.h"
#include "../core/SceneStepPolicy.h"
#include "../core/PixLiteStatusPolicy.h"
#include "../core/PixLiteTargets.h"
#include "Diagnostics.h"
#include "MemoryResources.h"
#include "Security.h"

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
