#pragma once

#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ETH.h>
#include <WiFi.h>
#include <apps/dhcpserver/dhcpserver.h>
#include <esp_netif_net_stack.h>
#include <lwip/netif.h>

#include "../boards/BoardProfile.h"
#include "../core/Types.h"
#include "../core/RecoveryPolicy.h"
#include "../core/RuntimePolicy.h"
#include "../core/Util.h"
#include "Diagnostics.h"

namespace advatek {

class NetworkManager {
 public:
  NetworkManager(const BoardProfile &board, AppConfig &config, Diagnostics &log)
      : board_(board), config_(config), log_(log) {}

  void begin(const char *apName) {
    copyText(apName_, sizeof(apName_), apName);
    if (config_.network.uplink == UplinkMode::Ethernet) {
      WiFi.mode(WIFI_MODE_NULL);
      beginEthernet();
    } else {
      WiFi.mode(WIFI_MODE_STA);
      beginWifiStation();
    }
  }

  void loop() {
    if (apRunning_ || ethernetRecoveryRunning_) {
      dns_.processNextRequest();
      if (apStopAt_ && dueAt(millis(), apStopAt_)) stopAccessPoint();
      if (ethernetRecoveryStopAt_ &&
          dueAt(millis(), ethernetRecoveryStopAt_)) {
        stopEthernetRecovery();
      }
    }
    const bool nowOnline = uplinkConnected();
    if (nowOnline != wasOnline_) {
      wasOnline_ = nowOnline;
      if (nowOnline) {
        activeInterface().setDefault();
        startMdns();
        log_.addf(LogLevel::Info, "Operational uplink connected at %s", localIP().toString().c_str());
      } else {
        log_.add(LogLevel::Warning, "Operational uplink disconnected");
      }
    }
  }

  bool uplinkConnected() const {
    return config_.network.uplink == UplinkMode::Ethernet
               ? ETH.linkUp() && ETH.localIP() != IPAddress()
               : WiFi.STA.status() == WL_CONNECTED;
  }

  NetworkInterface &activeInterface() {
    return config_.network.uplink == UplinkMode::Ethernet
               ? static_cast<NetworkInterface &>(ETH)
               : static_cast<NetworkInterface &>(WiFi.STA);
  }

  IPAddress localIP() const {
    return config_.network.uplink == UplinkMode::Ethernet ? ETH.localIP() : WiFi.STA.localIP();
  }

  IPAddress netmask() const {
    return config_.network.uplink == UplinkMode::Ethernet
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

  bool openRecoveryNetwork() {
    config_.network.accessPointPassword[0] = '\0';
    if (usesEthernetRecovery(config_.network)) {
      return beginEthernetRecovery();
    }
    beginAccessPoint(900000UL);
    log_.add(LogLevel::Warning, "Open recovery access point enabled for 15 minutes");
    return true;
  }

 private:
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
    applyStatic(ETH, config_.network.ethernet);
    log_.addf(
        LogLevel::Info,
        "W5500 initialized for %s addressing",
        config_.network.ethernet.mode == IpMode::Static ? "static" : "DHCP");
  }

  void beginWifiStation() {
    log_.add(LogLevel::Info, "Starting explicit Wi-Fi station uplink");
    applyStatic(WiFi.STA, config_.network.wifi);
    WiFi.STA.setHostname(config_.network.hostname);
    WiFi.begin(config_.network.wifiSsid, config_.network.wifiPassword);
  }

  void beginAccessPoint(uint32_t lifetimeMs) {
    WiFi.mode(
        config_.network.uplink == UplinkMode::WifiStation
            ? WIFI_MODE_APSTA
            : WIFI_MODE_AP);
    WiFi.AP.begin();
    IPAddress ip(192, 168, 4, 1);
    WiFi.AP.config(ip, ip, IPAddress(255, 255, 255, 0));
    const char *password = config_.network.accessPointPassword;
    if (password[0] && strlen(password) >= 8) WiFi.AP.create(apName_, password);
    else WiFi.AP.create(apName_);
    dns_.start(53, "*", ip);
    startMdns();
    apRunning_ = true;
    apStopAt_ = lifetimeMs ? millis() + lifetimeMs : 0;
    log_.addf(LogLevel::Info, "Recovery access point %s started", apName_);
  }

  bool beginEthernetRecovery() {
    // Starting a DHCP server on an installed LAN could disrupt every device on
    // that segment. Require no physical link when BOOT recovery is released;
    // the installer plugs the computer in only after recovery mode is armed.
    if (!ethernetInitialized_) beginEthernet();
    if (!ethernetInitialized_) {
      log_.add(
          LogLevel::Error,
          "Direct Ethernet recovery unavailable because W5500 initialization failed");
      return false;
    }
    // A Wi-Fi-operated unit may not have initialized W5500 before recovery.
    // Give an already-connected PHY time to report link before making the
    // safety decision; a disconnected direct cable simply adds this delay.
    const uint32_t linkCheckUntil = millis() + 3000UL;
    while (!ETH.linkUp() && !dueAt(millis(), linkCheckUntil)) delay(25);
    if (!directEthernetRecoveryAllowed(ETH.linkUp())) {
      log_.add(
          LogLevel::Error,
          "Direct Ethernet recovery refused: disconnect the Ethernet cable before BOOT recovery");
      return false;
    }

    const IPAddress address(192, 168, 4, 1);
    const IPAddress mask(255, 255, 255, 0);
    if (!ETH.config(address, address, mask, address)) {
      log_.add(LogLevel::Error, "Direct Ethernet recovery could not set 192.168.4.1");
      return false;
    }
    struct netif *lwipNetif =
        static_cast<struct netif *>(esp_netif_get_netif_impl(ETH.netif()));
    ethernetDhcpServer_ = dhcps_new();
    ip4_addr_t serverAddress{};
    serverAddress.addr = static_cast<uint32_t>(address);
    if (!lwipNetif || !ethernetDhcpServer_ ||
        dhcps_start(ethernetDhcpServer_, lwipNetif, serverAddress) != ERR_OK) {
      if (ethernetDhcpServer_) {
        dhcps_delete(ethernetDhcpServer_);
        ethernetDhcpServer_ = nullptr;
      }
      log_.add(LogLevel::Error, "Direct Ethernet recovery DHCP server failed to start");
      return false;
    }

    ETH.setDefault();
    dns_.start(53, "*", address);
    ethernetRecoveryRunning_ = true;
    ethernetRecoveryStopAt_ = millis() + 900000UL;
    log_.add(
        LogLevel::Warning,
        "Direct Ethernet recovery armed at 192.168.4.1 for 15 minutes; connect one computer directly");
    return true;
  }

  void stopEthernetRecovery() {
    dns_.stop();
    if (ethernetDhcpServer_) {
      struct netif *lwipNetif =
          static_cast<struct netif *>(esp_netif_get_netif_impl(ETH.netif()));
      if (lwipNetif) dhcps_stop(ethernetDhcpServer_, lwipNetif);
      dhcps_delete(ethernetDhcpServer_);
      ethernetDhcpServer_ = nullptr;
    }
    ethernetRecoveryRunning_ = false;
    ethernetRecoveryStopAt_ = 0;
    log_.add(LogLevel::Info, "Direct Ethernet recovery expired; restarting normal uplink");
    delay(50);
    ESP.restart();
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
  bool wasOnline_ = false;
  bool mdnsStarted_ = false;
  uint32_t apStopAt_ = 0;
  uint32_t ethernetRecoveryStopAt_ = 0;
  dhcps_t *ethernetDhcpServer_ = nullptr;
  char apName_[40]{};
  bool ethernetInitialized_ = false;
  uint8_t ethernetInitAttempts_ = 0;
  char ethernetFailureReason_[96]{};
};

}  // namespace advatek
