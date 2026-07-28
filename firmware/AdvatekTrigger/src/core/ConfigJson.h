#pragma once

#include "../boards/BoardProfile.h"
#include "JsonLite.h"
#include "Types.h"
#include "Util.h"
#include "Validation.h"

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
