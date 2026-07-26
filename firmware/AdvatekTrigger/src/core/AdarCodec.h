#pragma once

#include "AdarPacket.h"
#include "JsonLite.h"
#include "Types.h"
#include "Util.h"

namespace advatek {

inline bool parseAdarDiscoveryResponse(
    const uint8_t *packet,
    size_t length,
    bool viaBroadcast,
    DiscoveredDevice &device) {
  if (!validAdarResponseHeader(packet, length) || length >= 1024) return false;
  char json[1024];
  const size_t jsonLength = length - 12;
  memcpy(json, packet + 12, jsonLength);
  json[jsonLength] = '\0';
  JsonToken tokens[96];
  JsonDocument document(json, tokens, 96);
  if (!document.parse()) return false;
  const int16_t root = document.root();
  const int16_t family = document.objectValue(root, "prodFamily");
  const int16_t mac = document.objectValue(root, "macAddr");
  const int16_t ip = document.objectValue(root, "ipAddr");
  const int16_t product = document.objectValue(root, "prodName");
  if (family < 0 || mac < 0 || ip < 0 || product < 0) return false;

  memset(&device, 0, sizeof(device));
  device.productFamily = static_cast<uint32_t>(document.integerValue(family, -1));
  if (device.productFamily != PIXLITE_MK3_FAMILY ||
      !document.stringValue(mac, device.macAddress, sizeof(device.macAddress)) ||
      !document.stringValue(ip, device.ipAddress, sizeof(device.ipAddress)) ||
      !document.stringValue(product, device.productName, sizeof(device.productName))) {
    return false;
  }
  uint8_t parsedMac[6];
  IPAddress parsedIp;
  if (!parseMac12(device.macAddress, parsedMac) || !parseIPv4(device.ipAddress, parsedIp)) return false;
  const int16_t familyName = document.objectValue(root, "prodFamilyName");
  const int16_t nickname = document.objectValue(root, "nickname");
  const int16_t firmware = document.objectValue(root, "fwVer");
  if (familyName >= 0) document.stringValue(familyName, device.familyName, sizeof(device.familyName));
  if (nickname >= 0) document.stringValue(nickname, device.nickname, sizeof(device.nickname));
  if (firmware >= 0) document.stringValue(firmware, device.firmwareVersion, sizeof(device.firmwareVersion));
  device.authRequired = document.booleanValue(document.objectValue(root, "authReqd"), false);
  device.emergencyRecovery =
      document.booleanValue(document.objectValue(root, "inEmergRecovery"), false);
  device.viaBroadcast = viaBroadcast;
  return true;
}

}  // namespace advatek
