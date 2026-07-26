/*
  Waveshare ESP32-S3-ETH hardware canary

  This maintained diagnostic verifies the board identity, memory, BOOT button,
  RGB LED and W5500 Ethernet path without configuring any contact-input GPIO.
  It is not a release firmware artifact.

  SPDX-License-Identifier: MIT
*/

#include <Arduino.h>
#include <ETH.h>
#include <Network.h>
#include <SPI.h>
#include <esp_arduino_version.h>
#include <esp_heap_caps.h>
#include <esp_system.h>

namespace {

constexpr int8_t W5500_MISO = 12;
constexpr int8_t W5500_MOSI = 11;
constexpr int8_t W5500_SCLK = 13;
constexpr int8_t W5500_CS = 14;
constexpr int8_t W5500_RESET = 9;
constexpr int8_t W5500_INTERRUPT = 10;
constexpr int8_t W5500_PHY_ADDRESS = 1;
constexpr int8_t BOOT_BUTTON = 0;
constexpr int8_t RGB_LED = 21;

bool ethernetStarted = false;
bool lastBootPressed = false;
uint32_t nextReportAt = 0;

const char *resetReasonName(esp_reset_reason_t reason) {
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

void printEthernetState() {
  Serial.printf(
      "ETH link=%s ip=%s mask=%s gateway=%s dns=%s speed=%luMbps duplex=%s\r\n",
      ETH.linkUp() ? "up" : "down",
      ETH.localIP().toString().c_str(),
      ETH.subnetMask().toString().c_str(),
      ETH.gatewayIP().toString().c_str(),
      ETH.dnsIP().toString().c_str(),
      static_cast<unsigned long>(ETH.linkUp() ? ETH.linkSpeed() : 0),
      ETH.linkUp() && ETH.fullDuplex() ? "full" : "half/unknown");
}

void onNetworkEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("EVENT ETH started");
      ETH.setHostname("advatek-trigger-canary");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("EVENT ETH cable/link connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.printf("EVENT ETH got IP; MAC=%s\r\n", ETH.macAddress().c_str());
      printEthernetState();
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("EVENT ETH lost IP");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("EVENT ETH disconnected");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("EVENT ETH stopped");
      break;
    default:
      break;
  }
}

void exerciseRgbLed() {
  Serial.println("RGB test: red, green, blue, then dim white");
  rgbLedWrite(RGB_LED, 32, 0, 0);
  delay(300);
  rgbLedWrite(RGB_LED, 0, 32, 0);
  delay(300);
  rgbLedWrite(RGB_LED, 0, 0, 32);
  delay(300);
  rgbLedWrite(RGB_LED, 8, 8, 8);
}

void beginEthernet() {
  Network.onEvent(onNetworkEvent);
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
  Serial.printf(
      "W5500 begin=%s pins MISO=%d MOSI=%d SCLK=%d CS=%d RESET=%d INT=%d\r\n",
      ethernetStarted ? "success" : "FAILED",
      W5500_MISO,
      W5500_MOSI,
      W5500_SCLK,
      W5500_CS,
      W5500_RESET,
      W5500_INTERRUPT);
}

}  // namespace

void setup() {
  Serial.begin(115200);
  // A generous first-boot window lets a maintainer start Serial Monitor after
  // an upload and still capture the identity report. The canary proceeds
  // unattended after the timeout, so Ethernet testing never depends on USB.
  const uint32_t serialDeadline = millis() + 30000;
  while (!Serial && static_cast<int32_t>(millis() - serialDeadline) < 0) delay(10);

  Serial.println();
  Serial.println("=== Advatek Waveshare ESP32-S3-ETH hardware canary ===");
  Serial.printf(
      "Arduino-ESP32=%s chip=%s revision=%u cores=%u CPU=%uMHz\r\n",
      ESP_ARDUINO_VERSION_STR,
      ESP.getChipModel(),
      ESP.getChipRevision(),
      ESP.getChipCores(),
      ESP.getCpuFreqMHz());
  Serial.printf(
      "Reset=%s (%d) flash=%lu PSRAM=%lu freePSRAM=%lu\r\n",
      resetReasonName(esp_reset_reason()),
      static_cast<int>(esp_reset_reason()),
      static_cast<unsigned long>(ESP.getFlashChipSize()),
      static_cast<unsigned long>(ESP.getPsramSize()),
      static_cast<unsigned long>(ESP.getFreePsram()));
  Serial.printf(
      "Memory check flash16MB=%s PSRAM8MB=%s internalFree=%lu largestInternal=%lu\r\n",
      ESP.getFlashChipSize() >= 16U * 1024U * 1024U ? "PASS" : "FAIL",
      ESP.getPsramSize() >= 8U * 1024U * 1024U ? "PASS" : "FAIL",
      static_cast<unsigned long>(
          heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)),
      static_cast<unsigned long>(
          heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)));

  pinMode(BOOT_BUTTON, INPUT_PULLUP);
  lastBootPressed = digitalRead(BOOT_BUTTON) == LOW;
  Serial.printf("BOOT GPIO0 initial=%s\r\n", lastBootPressed ? "pressed" : "released");
  exerciseRgbLed();
  beginEthernet();
  nextReportAt = millis() + 2000;
}

void loop() {
  const bool bootPressed = digitalRead(BOOT_BUTTON) == LOW;
  if (bootPressed != lastBootPressed) {
    lastBootPressed = bootPressed;
    Serial.printf("BOOT GPIO0=%s\r\n", bootPressed ? "pressed" : "released");
  }
  if (static_cast<int32_t>(millis() - nextReportAt) >= 0) {
    nextReportAt = millis() + 2000;
    Serial.printf(
        "CANARY uptime=%lus freeInternal=%lu freePSRAM=%lu ",
        static_cast<unsigned long>(millis() / 1000U),
        static_cast<unsigned long>(
            heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)),
        static_cast<unsigned long>(ESP.getFreePsram()));
    if (ethernetStarted) printEthernetState();
    else Serial.println("ETH initialization failed");
  }
  delay(10);
}
