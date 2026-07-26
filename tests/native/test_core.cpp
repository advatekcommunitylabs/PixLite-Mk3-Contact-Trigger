#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "../../firmware/AdvatekTrigger/src/core/ActionPolicy.h"
#include "../../firmware/AdvatekTrigger/src/core/AdarPacket.h"
#include "../../firmware/AdvatekTrigger/src/core/ConfigMigration.h"
#include "../../firmware/AdvatekTrigger/src/core/InputEngine.h"
#include "../../firmware/AdvatekTrigger/src/core/LatestAction.h"
#include "../../firmware/AdvatekTrigger/src/core/MediaRefreshPolicy.h"
#include "../../firmware/AdvatekTrigger/src/core/PixLiteStatusPolicy.h"
#include "../../firmware/AdvatekTrigger/src/core/PixLiteResponsePolicy.h"
#include "../../firmware/AdvatekTrigger/src/core/RuntimePolicy.h"
#include "../../firmware/AdvatekTrigger/src/core/RecoveryPolicy.h"
#include "../../firmware/AdvatekTrigger/src/core/SceneStepPolicy.h"
#include "../../firmware/AdvatekTrigger/src/core/Validation.h"

using namespace advatek;

static void testDebounceAndEdges() {
  InputRuntime runtime{};
  InputEngine::initialize(runtime, false, 100);
  assert(!InputEngine::update(runtime, true, 50, 110).occurred);
  assert(!InputEngine::update(runtime, false, 50, 120).occurred);
  assert(!InputEngine::update(runtime, true, 50, 130).occurred);
  assert(!InputEngine::update(runtime, true, 50, 179).occurred);
  const InputEvent press = InputEngine::update(runtime, true, 50, 180);
  assert(press.occurred && press.active);
  assert(runtime.eventSequence == 1);
  const InputEvent release = InputEngine::update(runtime, false, 10, 200);
  assert(!release.occurred);
  assert(InputEngine::update(runtime, false, 10, 210).occurred);
  assert(runtime.eventSequence == 2);
}

static void testMillisRollover() {
  InputRuntime runtime{};
  InputEngine::initialize(runtime, false, 0xFFFFFFF0u);
  InputEngine::update(runtime, true, 20, 0xFFFFFFF5u);
  assert(!InputEngine::update(runtime, true, 20, 0x00000008u).occurred);
  assert(InputEngine::update(runtime, true, 20, 0x00000009u).occurred);
  assert(actionStillFresh(0xFFFFFFF0u, 0x00000020u));
}

static void testNormallyClosedTranslation() {
  const bool electricalLow = true;
  const bool normallyOpenActive = electricalLow;
  const bool normallyClosedActive = !electricalLow;
  assert(normallyOpenActive);
  assert(!normallyClosedActive);
}

static void testLatestWinsAndExpiry() {
  LatestActionSlot slot;
  ActionConfig first{};
  first.kind = ActionKind::PlayScene;
  ActionConfig second{};
  second.kind = ActionKind::StopPlayback;
  slot.publish(0, true, first, 100);
  slot.publish(1, false, second, 101);
  PendingAction result{};
  assert(slot.take(result, 102));
  assert(result.inputIndex == 1 && result.action.kind == ActionKind::StopPlayback);
  assert(!slot.take(result, 103));
  slot.publish(0, true, first, 100);
  assert(!slot.take(result, 2101));
}

static void testIntensityClamp() {
  assert(nextRampPercent(50, 5, false) == 55);
  assert(nextRampPercent(98, 5, false) == 100);
  assert(nextRampPercent(3, 5, true) == 0);
}

static void testSceneStepping() {
  MediaFile files[5]{};
  strcpy(files[0].name, "Opening.scn");
  strcpy(files[1].name, "Intermission.pl");
  files[1].playlist = true;
  strcpy(files[2].name, "Middle.scn");
  strcpy(files[3].name, "Closing.scn");
  strcpy(files[4].name, "Night.pl");
  files[4].playlist = true;

  assert(sceneStepIndex(files, 5, "", false) == 0);
  assert(sceneStepIndex(files, 5, "", true) == 3);
  assert(sceneStepIndex(files, 5, "Opening.scn", false) == 2);
  assert(sceneStepIndex(files, 5, "Middle.scn", true) == 0);
  assert(sceneStepIndex(files, 5, "Closing.scn", false) == 0);
  assert(sceneStepIndex(files, 5, "OPENING.SCN", true) == 3);
  assert(sceneStepIndex(files, 5, "not-in-list.scn", false) == 0);
  assert(sceneStepIndex(files, 5, "not-in-list.scn", true) == 3);
  assert(sceneStepIndex(&files[1], 1, "", false) == -1);
  assert(sceneStepIndex(nullptr, 0, "", false) == -1);
}

static void testMediaRefreshAfterReconnect() {
  assert(!mediaRefreshRequiredAfterPoll(true, true));
  assert(!mediaRefreshRequiredAfterPoll(true, false));
  assert(!mediaRefreshRequiredAfterPoll(false, false));
  assert(mediaRefreshRequiredAfterPoll(false, true));
}

static void testAdarPacketBytes() {
  uint8_t packet[64]{};
  char excluded[1][13] = {"AABBCCDDEEFF"};
  const size_t length = buildAdarDiscoveryRequest(packet, sizeof(packet), excluded, 1);
  assert(length == 40);
  assert(memcmp(packet, "DiscProt", 8) == 0);
  assert(packet[8] == 0x12 && packet[9] == 0x01);
  assert(packet[10] == 0x01 && packet[11] == 0x01);
  for (int i = 12; i < 20; ++i) assert(packet[i] == 0xFF);
  for (int i = 20; i < 26; ++i) assert(packet[i] == 0x00);
  for (int i = 26; i < 32; ++i) assert(packet[i] == 0xFF);
  assert(packet[32] == 0 && packet[33] == 1);
  const uint8_t mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  assert(memcmp(packet + 34, mac, 6) == 0);
  assert(buildAdarDiscoveryRequest(packet, 33, nullptr, 0) == 0);
  uint8_t response[] = {
      'D','i','s','c','P','r','o','t', 0x21,0x02, 0x01,0x01, '{','}'};
  assert(validAdarResponseHeader(response, sizeof(response)));
  response[9] = 0x03;
  assert(!validAdarResponseHeader(response, sizeof(response)));
  response[9] = 0x02;
  response[11] = 0x02;
  assert(!validAdarResponseHeader(response, sizeof(response)));
}

static void testConfigurationSchemaGate() {
  const uint8_t allowed[] = {1, 2, 15, 16, 18, 38, 39, 40};
  AppConfig current{};
  current.schemaVersion = CONFIG_SCHEMA_VERSION;
  strcpy(current.hardware.boardId, "board");
  strcpy(current.hardware.profileVersion, "1.2.0");
  AppConfig decoded{};
  bool migrated = true;
  assert(migrateStoredConfig(
      &current, sizeof(current), decoded, "board", "1.2.0", allowed, 8, migrated));
  assert(!migrated);

  AppConfig oldProfile = current;
  strcpy(oldProfile.hardware.profileVersion, "1.1.0");
  oldProfile.hardware.inputGpios[0] = 17;
  oldProfile.hardware.inputGpios[1] = 47;
  oldProfile.hardware.inputGpios[2] = 16;
  assert(migrateStoredConfig(
      &oldProfile, sizeof(oldProfile), decoded, "board", "1.2.0", allowed, 8, migrated));
  assert(migrated);
  assert(strcmp(decoded.hardware.profileVersion, "1.2.0") == 0);
  assert(decoded.hardware.inputGpios[0] == PIN_UNASSIGNED);
  assert(decoded.hardware.inputGpios[1] == PIN_UNASSIGNED);
  assert(decoded.hardware.inputGpios[2] == 16);

  AppConfig otherBoard = current;
  strcpy(otherBoard.hardware.boardId, "different-board");
  otherBoard.hardware.inputGpios[0] = 1;
  otherBoard.inputs[0].enabled = true;
  assert(migrateStoredConfig(
      &otherBoard,
      sizeof(otherBoard),
      decoded,
      "board",
      "1.2.0",
      allowed,
      8,
      migrated));
  assert(migrated);
  assert(strcmp(decoded.hardware.boardId, "board") == 0);
  assert(decoded.hardware.inputGpios[0] == PIN_UNASSIGNED);
  assert(strcmp(decoded.inputs[0].name, otherBoard.inputs[0].name) == 0);

  AppConfigV2 schemaV2{};
  schemaV2.magic = 0x41544354UL;
  schemaV2.schemaVersion = 2;
  schemaV2.sequence = 51;
  strcpy(schemaV2.hardware.boardId, "board");
  strcpy(schemaV2.hardware.profileVersion, "1.2.0");
  schemaV2.hardware.inputGpios[0] = 1;
  schemaV2.inputs[0].enabled = true;
  schemaV2.inputs[0].debounceMs = 125;
  strcpy(schemaV2.inputs[0].name, "Existing v2 input");
  strcpy(schemaV2.pixlite.host, "192.168.1.84");
  assert(migrateStoredConfig(
      &schemaV2, sizeof(schemaV2), decoded, "board", "1.2.0", allowed, 8, migrated));
  assert(migrated);
  assert(decoded.schemaVersion == CONFIG_SCHEMA_VERSION);
  assert(decoded.sequence == 51);
  assert(decoded.hardware.inputGpios[0] == 1);
  assert(strcmp(decoded.inputs[0].name, "Existing v2 input") == 0);
  assert(decoded.inputs[0].debounceMs == 125);
  assert(decoded.pixliteCount == 1);
  assert(strcmp(decoded.pixlites[0].host, "192.168.1.84") == 0);
  assert(strcmp(decoded.pixlites[0].id, "primary") == 0);
  assert(decoded.statusLed.enabled);
  assert(decoded.statusLed.brightnessPercent == 100);

  AppConfigV1 legacy{};
  legacy.magic = 0x41544354UL;
  legacy.schemaVersion = 1;
  legacy.sequence = 42;
  legacy.inputs[0].enabled = true;
  legacy.inputs[0].gpio = 33;
  legacy.inputs[0].debounceMs = 75;
  strcpy(legacy.inputs[0].name, "Unsafe legacy input");
  legacy.inputs[0].onAction.kind = ActionKind::PlayScene;
  legacy.inputs[1].enabled = true;
  legacy.inputs[1].gpio = 38;
  assert(migrateStoredConfig(
      &legacy, sizeof(legacy), decoded, "board", "1.2.0", allowed, 8, migrated));
  assert(migrated);
  assert(decoded.schemaVersion == CONFIG_SCHEMA_VERSION && decoded.sequence == 42);
  assert(strcmp(decoded.hardware.boardId, "board") == 0);
  assert(strcmp(decoded.hardware.profileVersion, "1.2.0") == 0);
  assert(decoded.hardware.inputGpios[0] == PIN_UNASSIGNED);
  assert(decoded.hardware.inputGpios[1] == 38);
  assert(strcmp(decoded.inputs[0].name, "Unsafe legacy input") == 0);
  assert(decoded.inputs[0].debounceMs == 75);
  assert(decoded.inputs[0].onAction.kind == ActionKind::PlayScene);

  AppConfigV3 schemaV3{};
  schemaV3.magic = 0x41544354UL;
  schemaV3.schemaVersion = 3;
  schemaV3.sequence = 52;
  strcpy(schemaV3.hardware.boardId, "board");
  strcpy(schemaV3.hardware.profileVersion, "1.2.0");
  strcpy(schemaV3.pixlite.host, "192.168.1.84");
  strcpy(schemaV3.pixlite.macAddress, "E0B6F5E0E9C1");
  strcpy(schemaV3.pixlite.username, "oper");
  schemaV3.pixlite.port = 80;
  strcpy(schemaV3.inputs[0].onAction.targetId, "primary");
  assert(migrateStoredConfig(
      &schemaV3, sizeof(schemaV3), decoded, "board", "1.2.0", allowed, 8, migrated));
  assert(migrated);
  assert(decoded.pixliteCount == 1);
  assert(strcmp(decoded.pixlites[0].id, "primary") == 0);
  assert(strcmp(decoded.pixlites[0].macAddress, "E0B6F5E0E9C1") == 0);
  assert(strcmp(decoded.inputs[0].onAction.targetId, "primary") == 0);

  AppConfigV4 schemaV4{};
  schemaV4.magic = 0x41544354UL;
  schemaV4.schemaVersion = 4;
  schemaV4.sequence = 53;
  strcpy(schemaV4.hardware.boardId, "board");
  strcpy(schemaV4.hardware.profileVersion, "1.2.0");
  schemaV4.inputs[0].enabled = true;
  schemaV4.inputs[0].onAction.kind = ActionKind::StopPlayback;
  schemaV4.hardware.inputGpios[0] = 16;
  assert(migrateStoredConfig(
      &schemaV4, sizeof(schemaV4), decoded, "board", "1.2.0", allowed, 8, migrated));
  assert(migrated);
  assert(decoded.schemaVersion == CONFIG_SCHEMA_VERSION);
  assert(decoded.inputs[0].onAction.kind == ActionKind::StopPlayback);
  assert(decoded.inputs[0].onAction.testRed == 255);
  assert(decoded.inputs[0].onAction.testGreen == 0);
  assert(decoded.inputs[0].onAction.testBlue == 0);

  legacy.schemaVersion = 6;
  assert(!migrateStoredConfig(
      &legacy, sizeof(legacy), decoded, "board", "1.2.0", allowed, 8, migrated));
}

static void testRecoveryHoldZones() {
  assert(recoveryIntentForDuration(4999) == RecoveryIntent::None);
  assert(recoveryIntentForDuration(5000) == RecoveryIntent::ClearAuthentication);
  assert(recoveryIntentForDuration(14999) == RecoveryIntent::ClearAuthentication);
  assert(recoveryIntentForDuration(15000) == RecoveryIntent::FactoryReset);
  assert(recoveryIntentForDuration(19999) == RecoveryIntent::FactoryReset);
  assert(recoveryIntentForDuration(20000) == RecoveryIntent::Cancelled);
  assert(directEthernetRecoveryAllowed(false));
  assert(!directEthernetRecoveryAllowed(true));
}

static void testMemoryAndBoundedPayloadPolicy() {
  assert(selectMemoryMode(true, true) == MemoryMode::Normal);
  assert(selectMemoryMode(false, true) == MemoryMode::DegradedRecovery);
  assert(selectMemoryMode(true, false) == MemoryMode::DegradedRecovery);
  assert(payloadWithinLimit(12 * 1024, 12 * 1024));
  assert(!payloadWithinLimit(12 * 1024 + 1, 12 * 1024));
  assert(payloadWithinLimit(32 * 1024, 32 * 1024));
  assert(!payloadWithinLimit(32 * 1024 + 1, 32 * 1024));
}

static void testMdnsHostnameValidation() {
  assert(validMdnsHostname("advatrigger"));
  assert(validMdnsHostname("advatrigger-front-of-house"));
  assert(validMdnsHostname("Trigger2"));
  assert(!validMdnsHostname(""));
  assert(!validMdnsHostname("-advatrigger"));
  assert(!validMdnsHostname("advatrigger-"));
  assert(!validMdnsHostname("advatrigger.local"));
  assert(!validMdnsHostname("advatek trigger"));
  assert(!validMdnsHostname("abcdefghijklmnopqrstuvwxyz123456"));
}

static void testBoundedEthernetRetries() {
  BoundedRetryState retry{3, 0, false};
  assert(retry.canAttempt());
  retry.record(false);
  retry.record(false);
  assert(retry.attempts == 2 && retry.canAttempt());
  retry.record(false);
  assert(retry.attempts == 3 && !retry.canAttempt() && !retry.succeeded);
  retry.record(true);
  assert(retry.attempts == 3 && !retry.succeeded);

  BoundedRetryState success{3, 0, false};
  success.record(false);
  success.record(true);
  assert(success.attempts == 2 && success.succeeded && !success.canAttempt());
}

static void testPixLiteProgramPriority() {
  assert(programPriorityIsHigh("Hi"));
  assert(programPriorityIsHigh("hi"));
  assert(!programPriorityIsHigh("Lo"));
  assert(!programPriorityIsHigh(nullptr));
}

static void testPixLiteApiErrorResponse() {
  JsonToken tokens[48]{};
  PixLiteApiError error;
  assert(inspectPixLiteResponse(
      "{\"resp\":\"modePlayback\",\"id\":1,\"result\":{\"status\":{}}}",
      tokens,
      48,
      error));
  assert(!error.present);

  assert(inspectPixLiteResponse(
      "{\"err\":{\"code\":1,\"msg\":\"Parameter mode is malformed\"},"
      "\"resp\":\"modePlayback\",\"id\":2}",
      tokens,
      48,
      error));
  assert(error.present);
  assert(error.code == 1);
  assert(strcmp(error.message, "Parameter mode is malformed") == 0);

  // A nested playback state error is status information, not request rejection.
  assert(inspectPixLiteResponse(
      "{\"result\":{\"status\":{\"state\":{\"err\":{\"code\":8,"
      "\"msg\":\"File failed\"}}}}}",
      tokens,
      48,
      error));
  assert(!error.present);
  assert(!inspectPixLiteResponse("{\"result\":", tokens, 48, error));
}

int main() {
  testDebounceAndEdges();
  testMillisRollover();
  testNormallyClosedTranslation();
  testLatestWinsAndExpiry();
  testIntensityClamp();
  testSceneStepping();
  testMediaRefreshAfterReconnect();
  testAdarPacketBytes();
  testConfigurationSchemaGate();
  testMemoryAndBoundedPayloadPolicy();
  testMdnsHostnameValidation();
  testBoundedEthernetRetries();
  testPixLiteProgramPriority();
  testPixLiteApiErrorResponse();
  testRecoveryHoldZones();
  return 0;
}
