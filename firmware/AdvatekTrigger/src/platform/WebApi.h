#pragma once

#include <WebServer.h>

#include "../boards/BoardProfile.h"
#include "../core/ConfigJson.h"
#include "../core/PixLiteTargets.h"
#include "../core/Versions.h"
#include "../web/WebAsset.h"
#include "AdarDiscovery.h"
#include "ConfigStore.h"
#include "Diagnostics.h"
#include "MemoryResources.h"
#include "PixLiteClient.h"
#include "Security.h"

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
