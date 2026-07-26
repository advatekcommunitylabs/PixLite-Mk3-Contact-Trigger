#pragma once

#include "Types.h"
#include "Util.h"
#include "../boards/BoardProfile.h"

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
