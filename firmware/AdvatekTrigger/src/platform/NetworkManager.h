#pragma once

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

#include "../boards/BoardProfile.h"
#include "../core/Types.h"
#include "../core/RecoveryPolicy.h"
#include "../core/RuntimePolicy.h"
#include "../core/Util.h"
#include "Diagnostics.h"

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
