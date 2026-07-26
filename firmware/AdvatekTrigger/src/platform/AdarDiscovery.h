#pragma once

#include <NetworkUdp.h>

#include "../core/AdarCodec.h"
#include "Diagnostics.h"
#include "MemoryResources.h"
#include "NetworkManager.h"

namespace advatek {

class AdarDiscovery {
 public:
  AdarDiscovery(NetworkManager &network, Diagnostics &log, MemoryResources &memory)
      : network_(network), log_(log), memory_(memory) {}

  uint8_t count() const { return count_; }
  bool truncated() const { return truncated_; }
  const DiscoveredDevice &device(uint8_t index) const {
    return memory_.devices[index];
  }

  void discover() {
    count_ = 0;
    truncated_ = false;
    if (!memory_.devices) {
      log_.add(LogLevel::Error, "ADAR discovery disabled: PSRAM workspace unavailable");
      return;
    }
    if (!network_.uplinkConnected()) {
      log_.add(LogLevel::Warning, "Discovery skipped because the operational uplink is offline");
      return;
    }
    IPAddress listening;
    listening.fromString(ADAR_CLIENT_MULTICAST);
    if (!udp_.beginMulticast(listening, 0)) {
      log_.add(LogLevel::Error, "Could not open ADAR discovery socket");
      return;
    }
    runCycle(false, 3);
    runCycle(true, 2);
    udp_.stop();
    log_.addf(
        LogLevel::Info,
        "ADAR discovery completed with %u PixLite Mk3 device(s)%s",
        count_,
        truncated_ ? " (list truncated)" : "");
  }

  int16_t findByMac(const char *mac) const {
    for (uint8_t i = 0; i < count_; ++i) {
      if (strcasecmp(memory_.devices[i].macAddress, mac) == 0) return i;
    }
    return -1;
  }

 private:
  void runCycle(bool broadcast, uint8_t attempts) {
    for (uint8_t attempt = 0; attempt < attempts; ++attempt) {
      sendRequest(broadcast);
      const uint32_t deadline = millis() + 500;
      while (!dueAt(millis(), deadline)) {
        const int packetLength = udp_.parsePacket();
        if (packetLength > 0 && packetLength <= static_cast<int>(sizeof(packet_))) {
          const int read = udp_.read(packet_, packetLength);
          DiscoveredDevice candidate{};
          if (read == packetLength &&
              parseAdarDiscoveryResponse(packet_, read, broadcast, candidate)) {
            addOrUpdate(candidate);
          }
        }
        delay(2);
      }
    }
  }

  void sendRequest(bool broadcast) {
    char exclusions[50][13]{};
    const uint8_t excludedCount = count_ < 50 ? count_ : 50;
    for (uint8_t i = 0; i < excludedCount; ++i) {
      copyText(exclusions[i], sizeof(exclusions[i]), memory_.devices[i].macAddress);
    }
    const size_t length =
        buildAdarDiscoveryRequest(packet_, sizeof(packet_), exclusions, excludedCount);
    IPAddress destination;
    if (broadcast) destination = network_.broadcastIP();
    else destination.fromString(ADAR_DEVICE_MULTICAST);
    udp_.beginPacket(destination, ADAR_PORT);
    udp_.write(packet_, length);
    udp_.endPacket();
  }

  void addOrUpdate(const DiscoveredDevice &candidate) {
    const int16_t existing = findByMac(candidate.macAddress);
    if (existing >= 0) {
      memory_.devices[existing] = candidate;
      return;
    }
    if (count_ >= MAX_DISCOVERED_DEVICES) {
      truncated_ = true;
      return;
    }
    memory_.devices[count_++] = candidate;
  }

  NetworkManager &network_;
  Diagnostics &log_;
  MemoryResources &memory_;
  NetworkUDP udp_;
  uint8_t packet_[1024]{};
  uint8_t count_ = 0;
  bool truncated_ = false;
};

}  // namespace advatek
