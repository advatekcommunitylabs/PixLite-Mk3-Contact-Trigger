#pragma once

#include "JsonLite.h"
#include "Types.h"
#include "Util.h"

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
