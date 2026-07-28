/*
  Waveshare ESP32-S3-ETH Wi-Fi Station probe

  This maintained diagnostic attempts a Wi-Fi Station connection first, then
  starts W5500 Ethernet solely as an independent reporting path. It lets a
  maintainer inspect signal/authentication failures without adding any silent
  Ethernet fallback behaviour to the production firmware.

  Supply WIFI_TEST_SSID and WIFI_TEST_PASSWORD as compiler definitions. Never
  place site credentials in this source file or commit them to the repository.

  SPDX-License-Identifier: MIT
*/

#include <Arduino.h>
#include <ETH.h>
#include <Preferences.h>
#include <SPI.h>
#include <WebServer.h>
#include <WiFi.h>

#include "../../../firmware/AdvatekTrigger/src/core/Types.h"

#ifndef WIFI_TEST_SSID
#define WIFI_TEST_SSID ""
#endif

#ifndef WIFI_TEST_PASSWORD
#define WIFI_TEST_PASSWORD ""
#endif

namespace {

constexpr int8_t W5500_MISO = 12;
constexpr int8_t W5500_MOSI = 11;
constexpr int8_t W5500_SCLK = 13;
constexpr int8_t W5500_CS = 14;
constexpr int8_t W5500_RESET = 9;
constexpr int8_t W5500_INTERRUPT = 10;
constexpr int8_t W5500_PHY_ADDRESS = 1;
constexpr uint32_t WIFI_OBSERVATION_MS = 15000;

WebServer server(80);
volatile uint8_t wifiDisconnectReason = 0;
bool ethernetStarted = false;
bool configAHasSsid = false;
bool configAHasPassword = false;
bool configBHasSsid = false;
bool configBHasPassword = false;
bool storedConfigUsable = false;
uint32_t configASequence = 0;
uint32_t configBSequence = 0;
advatek::AppConfig storedConfig{};

bool bufferContains(
    const uint8_t *buffer,
    size_t bufferLength,
    const char *value) {
  const size_t valueLength = strlen(value);
  if (!valueLength || valueLength > bufferLength) return false;
  for (size_t offset = 0; offset + valueLength <= bufferLength; ++offset) {
    if (memcmp(buffer + offset, value, valueLength) == 0) return true;
  }
  return false;
}

void inspectConfigSlot(
    Preferences &preferences,
    const char *slot,
    bool &hasSsid,
    bool &hasPassword,
    uint32_t &sequence,
    advatek::AppConfig &candidate,
    bool &candidateUsable) {
  const size_t length = preferences.getBytesLength(slot);
  if (!length) return;
  uint8_t *buffer = static_cast<uint8_t *>(malloc(length));
  if (!buffer) return;
  if (preferences.getBytes(slot, buffer, length) == length) {
    hasSsid = bufferContains(buffer, length, WIFI_TEST_SSID);
    hasPassword = bufferContains(buffer, length, WIFI_TEST_PASSWORD);
    if (length == sizeof(advatek::AppConfig)) {
      const advatek::AppConfig &decoded =
          *reinterpret_cast<const advatek::AppConfig *>(buffer);
      sequence = decoded.sequence;
      if (decoded.magic == 0x41544354UL &&
          decoded.schemaVersion == advatek::CONFIG_SCHEMA_VERSION &&
          (!candidateUsable || decoded.sequence > candidate.sequence)) {
        candidate = decoded;
        candidateUsable = true;
      }
    }
  }
  free(buffer);
}

const char *wifiStatusName(wl_status_t status) {
  switch (status) {
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

String stateJson() {
  String json = "{\"probe\":\"waveshare-esp32-s3-eth-wifi\",";
  json += "\"wifi\":{\"status\":\"";
  json += wifiStatusName(WiFi.STA.status());
  json += "\",\"statusCode\":" + String(static_cast<int>(WiFi.STA.status()));
  json += ",\"disconnectReason\":" + String(wifiDisconnectReason);
  json += ",\"rssiDbm\":" +
          String(WiFi.STA.status() == WL_CONNECTED ? WiFi.STA.RSSI() : 0);
  json += ",\"ip\":\"" + WiFi.STA.localIP().toString() + "\"},";
  json += "\"ethernet\":{\"started\":";
  json += ethernetStarted ? "true" : "false";
  json += ",\"linkUp\":";
  json += ETH.linkUp() ? "true" : "false";
  json += ",\"ip\":\"" + ETH.localIP().toString() + "\"}}";
  json.remove(json.length() - 1);
  json += ",\"savedConfig\":{\"configAHasSsid\":";
  json += configAHasSsid ? "true" : "false";
  json += ",\"configAHasPassword\":";
  json += configAHasPassword ? "true" : "false";
  json += ",\"configBHasSsid\":";
  json += configBHasSsid ? "true" : "false";
  json += ",\"configBHasPassword\":";
  json += configBHasPassword ? "true" : "false";
  json += ",\"configASequence\":" + String(configASequence);
  json += ",\"configBSequence\":" + String(configBSequence);
  json += ",\"selectedUsable\":";
  json += storedConfigUsable ? "true" : "false";
  json += ",\"selectedUplink\":\"";
  json += storedConfigUsable &&
                  storedConfig.network.uplink ==
                      advatek::UplinkMode::WifiStation
              ? "wifi"
              : "ethernet";
  json += "\",\"selectedWifiAddressing\":\"";
  json += storedConfigUsable &&
                  storedConfig.network.wifi.mode == advatek::IpMode::Static
              ? "static"
              : "dhcp";
  json += "\",\"selectedSsidLength\":" +
          String(storedConfigUsable
                     ? strlen(storedConfig.network.wifiSsid)
                     : 0);
  json += ",\"selectedPasswordLength\":" +
          String(storedConfigUsable
                     ? strlen(storedConfig.network.wifiPassword)
                     : 0);
  json += "}}";
  return json;
}

void beginEthernetReporting() {
  pinMode(W5500_RESET, OUTPUT);
  digitalWrite(W5500_RESET, LOW);
  delay(50);
  digitalWrite(W5500_RESET, HIGH);
  delay(200);
  SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI);
  ethernetStarted = ETH.begin(
      ETH_PHY_W5500,
      W5500_PHY_ADDRESS,
      W5500_CS,
      W5500_INTERRUPT,
      W5500_RESET,
      SPI);
  server.on("/probe", []() {
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", stateJson());
  });
  server.begin();
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Preferences preferences;
  if (preferences.begin("adv-trigger", true)) {
    inspectConfigSlot(
        preferences,
        "configA",
        configAHasSsid,
        configAHasPassword,
        configASequence,
        storedConfig,
        storedConfigUsable);
    inspectConfigSlot(
        preferences,
        "configB",
        configBHasSsid,
        configBHasPassword,
        configBSequence,
        storedConfig,
        storedConfigUsable);
    preferences.end();
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.onEvent(
      [](arduino_event_id_t, arduino_event_info_t info) {
        wifiDisconnectReason = info.wifi_sta_disconnected.reason;
      },
      ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.STA.setHostname("advatek-wifi-probe");
  WiFi.begin(
      storedConfigUsable
          ? storedConfig.network.wifiSsid
          : WIFI_TEST_SSID,
      storedConfigUsable
          ? storedConfig.network.wifiPassword
          : WIFI_TEST_PASSWORD);

  const uint32_t observationEnd = millis() + WIFI_OBSERVATION_MS;
  while (static_cast<int32_t>(millis() - observationEnd) < 0 &&
         WiFi.STA.status() != WL_CONNECTED) {
    delay(25);
  }
  beginEthernetReporting();
}

void loop() {
  server.handleClient();
  delay(2);
}
