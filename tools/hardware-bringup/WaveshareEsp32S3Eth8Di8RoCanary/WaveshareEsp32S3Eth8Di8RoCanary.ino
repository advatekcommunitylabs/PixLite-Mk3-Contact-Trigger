/*
 * Waveshare ESP32-S3-ETH-8DI-8RO hardware canary
 *
 * Run this small sketch before installing the production firmware on a newly
 * received standard or PoE board. It verifies memory, RGB LED, BOOT, W5500,
 * and all eight isolated digital inputs. Relays, RS485, buzzer, TF card, and
 * other peripherals are deliberately left untouched.
 *
 * Arduino IDE: ESP32S3 Dev Module, 16MB flash, OPI PSRAM,
 * Hardware CDC and JTAG, USB CDC On Boot enabled.
 */

#include <Arduino.h>
#include <ETH.h>
#include <ESPmDNS.h>
#include <SPI.h>
#include <WebServer.h>
#include <esp_heap_caps.h>

namespace {

constexpr uint8_t kRgbPin = 38;
constexpr uint8_t kBootPin = 0;
constexpr uint8_t kW5500IrqPin = 12;
constexpr uint8_t kW5500MosiPin = 13;
constexpr uint8_t kW5500MisoPin = 14;
constexpr uint8_t kW5500SclkPin = 15;
constexpr uint8_t kW5500CsPin = 16;
constexpr uint8_t kW5500ResetPin = 39;
constexpr uint8_t kInputPins[] = {4, 5, 6, 7, 8, 9, 10, 11};

WebServer server(80);
bool lastBootState = HIGH;
bool lastInputStates[8] = {};
uint32_t lastReportMs = 0;

void setRgb(uint8_t red, uint8_t green, uint8_t blue) {
  // This board's fitted addressable LED uses RGB byte order. The smaller
  // Waveshare ESP32-S3-ETH development board uses the WS2812B GRB default.
  rgbLedWriteOrdered(kRgbPin, LED_COLOR_ORDER_RGB, red, green, blue);
}

void printNetworkState() {
  Serial.printf(
      "Ethernet: link=%s speed=%u Mbps duplex=%s MAC=%s IP=%s mask=%s gateway=%s DNS=%s\n",
      ETH.linkUp() ? "up" : "down", ETH.linkSpeed(),
      ETH.fullDuplex() ? "full" : "half", ETH.macAddress().c_str(),
      ETH.localIP().toString().c_str(), ETH.subnetMask().toString().c_str(),
      ETH.gatewayIP().toString().c_str(), ETH.dnsIP().toString().c_str());
}

String inputStateJson() {
  // This deliberately exposes the electrical level as well as the interpreted
  // active-low state. It makes first-board polarity and terminal tests
  // conclusive even when the canary is powered by PoE with USB disconnected.
  String json = "{\"inputs\":[";
  json.reserve(360);
  for (size_t index = 0; index < 8; ++index) {
    if (index) json += ',';
    const bool rawHigh = digitalRead(kInputPins[index]) == HIGH;
    json += "{\"terminal\":\"DI" + String(index + 1) +
            "\",\"gpio\":" + String(kInputPins[index]) +
            ",\"rawHigh\":" + String(rawHigh ? "true" : "false") +
            ",\"active\":" + String(rawHigh ? "false" : "true") + "}";
  }
  json += "]}";
  return json;
}

void startDiagnosticServer() {
  if (!ETH.linkUp() || ETH.localIP() == IPAddress()) return;

  server.on("/", HTTP_GET, []() {
    server.send(
        200, "text/plain",
        "Advatek Trigger industrial input canary\n"
        "Open /api/inputs for DI1-DI8 electrical levels.\n");
  });
  server.on("/api/inputs", HTTP_GET, []() {
    server.send(200, "application/json", inputStateJson());
  });
  server.begin();

  if (MDNS.begin("advatek-8di-canary")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("Input diagnostics: http://advatek-8di-canary.local/api/inputs");
  } else {
    Serial.printf("Input diagnostics: http://%s/api/inputs\n",
                  ETH.localIP().toString().c_str());
  }
}

bool startEthernet() {
  pinMode(kW5500ResetPin, OUTPUT);
  digitalWrite(kW5500ResetPin, LOW);
  delay(100);
  digitalWrite(kW5500ResetPin, HIGH);
  delay(250);

  SPI.begin(kW5500SclkPin, kW5500MisoPin, kW5500MosiPin, kW5500CsPin);
  for (uint8_t attempt = 1; attempt <= 3; ++attempt) {
    Serial.printf("W5500 initialization attempt %u/3\n", attempt);
    if (ETH.begin(ETH_PHY_W5500, 1, kW5500CsPin, kW5500IrqPin,
                  kW5500ResetPin, SPI)) {
      return true;
    }
    delay(500);
  }
  return false;
}

void reportChangedInputs() {
  const bool bootState = digitalRead(kBootPin);
  if (bootState != lastBootState) {
    lastBootState = bootState;
    Serial.printf("BOOT: %s\n", bootState == LOW ? "pressed" : "released");
  }

  for (size_t index = 0; index < 8; ++index) {
    const bool state = digitalRead(kInputPins[index]);
    if (state != lastInputStates[index]) {
      lastInputStates[index] = state;
      Serial.printf("DI%u (GPIO%u): %s\n", static_cast<unsigned>(index + 1),
                    kInputPins[index],
                    state == LOW ? "ACTIVE" : "inactive");
      setRgb(255, 48, 0);
      delay(40);
      setRgb(32, 6, 0);
    }
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println();
  Serial.println("Advatek Trigger - Waveshare ESP32-S3-ETH-8DI-8RO canary");
  Serial.printf("Chip: %s revision %u, cores %u\n", ESP.getChipModel(),
                ESP.getChipRevision(), ESP.getChipCores());
  Serial.printf("Flash: %u bytes; PSRAM: %u bytes, free %u bytes\n",
                ESP.getFlashChipSize(), ESP.getPsramSize(), ESP.getFreePsram());
  Serial.printf("Internal heap: free %u bytes, largest block %u bytes\n",
                ESP.getFreeHeap(),
                heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));

  pinMode(kBootPin, INPUT_PULLUP);
  lastBootState = digitalRead(kBootPin);
  for (size_t index = 0; index < 8; ++index) {
    pinMode(kInputPins[index], INPUT_PULLUP);
    lastInputStates[index] = digitalRead(kInputPins[index]);
    Serial.printf("DI%u (GPIO%u): %s\n", static_cast<unsigned>(index + 1),
                  kInputPins[index],
                  lastInputStates[index] == LOW ? "ACTIVE" : "inactive");
  }

  setRgb(255, 0, 0);
  delay(200);
  setRgb(0, 255, 0);
  delay(200);
  setRgb(0, 0, 255);
  delay(200);
  setRgb(32, 6, 0);

  if (!startEthernet()) {
    Serial.println("ERROR: W5500 initialization failed.");
    setRgb(255, 0, 0);
    return;
  }

  Serial.println("W5500 initialized; waiting up to 15 seconds for link and DHCP...");
  const uint32_t deadline = millis() + 15000;
  while ((!ETH.linkUp() || ETH.localIP() == IPAddress()) &&
         static_cast<int32_t>(deadline - millis()) > 0) {
    delay(100);
  }
  printNetworkState();
  startDiagnosticServer();
}

void loop() {
  server.handleClient();
  reportChangedInputs();
  if (millis() - lastReportMs >= 5000) {
    lastReportMs = millis();
    printNetworkState();
  }
  delay(5);
}
