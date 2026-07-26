import { describe, expect, it } from 'vitest';
import { existsSync, readFileSync } from 'node:fs';

describe('repository contracts', () => {
  const manifest = JSON.parse(readFileSync('boards/manifest.json', 'utf8'));
  const compatibility = JSON.parse(readFileSync('compatibility.json', 'utf8'));

  it('keeps published compatibility metadata aligned with canonical versions', () => {
    const versions = readFileSync('firmware/AdvatekTrigger/src/core/Versions.h', 'utf8');
    const firmware = versions.match(/FIRMWARE_VERSION = "([^"]+)"/)?.[1];
    const webUi = versions.match(/WEB_UI_VERSION = "([^"]+)"/)?.[1];
    const schema = Number(versions.match(/CONFIG_SCHEMA_VERSION = (\d+)/)?.[1]);
    const board = manifest.boards[0];
    const publishedBoard = compatibility.boards.find(
      (candidate: {id: string}) => candidate.id === board.id,
    );

    expect(compatibility.firmware).toBe(firmware);
    expect(compatibility.webUi).toBe(webUi);
    expect(compatibility.configurationSchema).toBe(schema);
    expect(publishedBoard.profileVersion).toBe(board.profileVersion);
    expect(publishedBoard.arduinoEsp32).toBe('3.3.10');
  });

  it('keeps the Waveshare network pins away from assignable contacts', () => {
    const board = manifest.boards[0];
    const reserved = Object.values(board.ethernet).filter(value => typeof value === 'number');
    expect(board.allowedInputPins.filter((pin: number) => reserved.includes(pin))).toEqual([]);
    expect(new Set(board.allowedInputPins).size).toBe(8);
    expect(board.allowedInputPins).toEqual([1, 2, 15, 16, 18, 38, 39, 40]);
    expect(board.reservedPins.octalPsram).toEqual([33, 34, 35, 36, 37]);
    expect(board.cameraSharedPins).toEqual([1, 2, 15, 18, 38, 39, 40]);
    expect(board.reservedPins.poeObstructed).toEqual([17, 47]);
    expect(board.poeModule).toBe('power-only');
  });

  it('defines the industrial 8DI target without forking shared behavior', () => {
    const board = manifest.boards.find(
      (candidate: {id: string}) =>
        candidate.id === 'waveshare-esp32-s3-eth-8di-8ro',
    );
    expect(board.allowedInputPins).toEqual([4, 5, 6, 7, 8, 9, 10, 11]);
    expect(board.inputTerminals).toEqual([
      'DI1', 'DI2', 'DI3', 'DI4', 'DI5', 'DI6', 'DI7', 'DI8',
    ]);
    expect(board.ethernet).toMatchObject({
      miso: 14, mosi: 13, sclk: 15, cs: 16, reset: 39, interrupt: 12,
    });
    expect(board.statusLedPin).toBe(38);
    expect(board.recoveryButtonPin).toBe(0);
  });

  it('publishes a unique generated Arduino artifact for every board profile', () => {
    expect(new Set(manifest.boards.map((board: {id: string}) => board.id)).size)
      .toBe(manifest.boards.length);
    expect(new Set(manifest.boards.map((board: {artifact: string}) => board.artifact)).size)
      .toBe(manifest.boards.length);
    for (const board of manifest.boards) {
      expect(board.buildMacro).toMatch(/^ADVATEK_BOARD_/);
      expect(existsSync(`generated/${board.artifact}`)).toBe(true);
    }
  });

  it('keeps memory-heavy operations bounded and PSRAM-backed', () => {
    const source = readFileSync('firmware/AdvatekTrigger/src/platform/MemoryResources.h', 'utf8');
    expect(source).toContain('CONFIG_REQUEST_LIMIT = 12U * 1024U');
    expect(source).toContain('PIXLITE_RESPONSE_LIMIT = 32U * 1024U');
    expect(source).toContain('MALLOC_CAP_SPIRAM');
    expect(source).toContain('if (!ready) release()');
  });

  it('uses a bounded W5500 startup retry state', () => {
    const source = readFileSync('firmware/AdvatekTrigger/src/platform/NetworkManager.h', 'utf8');
    expect(source).toContain('BoundedRetryState retry{3, 0, false}');
    expect(source).toContain('Wi-Fi fallback is disabled');
    expect(source).toContain('ethernetInitAttempts_');
  });

  it('keeps the status LED orange and pulses it without blocking input scans', () => {
    const app = readFileSync('firmware/AdvatekTrigger/src/platform/App.h', 'utf8');
    expect(app).toContain('scaleStatusLedChannel(255)');
    expect(app).toContain('scaleStatusLedChannel(48)');
    expect(app).toContain('if (!config_.statusLed.enabled)');
    expect(app).toContain('triggerFlashUntil_ = now + 120');
    expect(app).not.toContain('delay(120)');
  });

  it('exposes every debounced input edge to the lightweight SPA activity poll', () => {
    const types = readFileSync('firmware/AdvatekTrigger/src/core/Types.h', 'utf8');
    const engine = readFileSync('firmware/AdvatekTrigger/src/core/InputEngine.h', 'utf8');
    const app = readFileSync('firmware/AdvatekTrigger/src/platform/App.h', 'utf8');
    const webApi = readFileSync('firmware/AdvatekTrigger/src/platform/WebApi.h', 'utf8');
    expect(types).toContain('uint32_t eventSequence');
    expect(engine).toContain('++runtime.eventSequence');
    expect(app).toContain('String inputStateJson() override');
    expect(app).toContain('\\"eventSequence\\"');
    expect(webApi).toContain('server_.on("/api/inputs", HTTP_GET');
  });

  it('persists status LED controls and migrates the Test Color schema v5', () => {
    const versions = readFileSync('firmware/AdvatekTrigger/src/core/Versions.h', 'utf8');
    const migration = readFileSync('firmware/AdvatekTrigger/src/core/ConfigMigration.h', 'utf8');
    const configJson = readFileSync('firmware/AdvatekTrigger/src/core/ConfigJson.h', 'utf8');
    expect(versions).toContain('CONFIG_SCHEMA_VERSION = 5');
    expect(migration).toContain('struct AppConfigV2');
    expect(migration).toContain('struct AppConfigV3');
    expect(migration).toContain('struct AppConfigV4');
    expect(migration).toContain('migrateActionV4');
    expect(migration).toContain('migrateLegacyPixLite');
    expect(configJson).toContain('"brightnessPercent"');
  });

  it('bounds saved PixLites at sixteen and keeps action targeting in the core', () => {
    const types = readFileSync('firmware/AdvatekTrigger/src/core/Types.h', 'utf8');
    const targets = readFileSync('firmware/AdvatekTrigger/src/core/PixLiteTargets.h', 'utf8');
    const memory = readFileSync('firmware/AdvatekTrigger/src/platform/MemoryResources.h', 'utf8');
    expect(types).toContain('MAX_PIXLITES = 16');
    expect(types).toContain('PixLiteConfig pixlites[MAX_PIXLITES]');
    expect(targets).toContain('pixLiteIndexById');
    expect(memory).toContain('MAX_PIXLITES * MAX_MEDIA_FILES');
    const webApi = readFileSync('firmware/AdvatekTrigger/src/platform/WebApi.h', 'utf8');
    expect(webApi).toContain('pixLiteIndexById(config_, candidate) < 0');
    expect(webApi).toContain('if (!config_.inputs[i].enabled) continue');
    expect(webApi).toContain('requestedPassword[0]');
    expect(webApi).toContain('\\",\\"via\\":\\"');
    const app = readFileSync('firmware/AdvatekTrigger/src/platform/App.h', 'utf8');
    expect(app).toContain('mediaPrimedIdentity[candidate] = identity');
    expect(app).toContain('pixliteTargetIdentity(config_.pixlites[candidate])');
    expect(app).toContain('MEDIA_REFRESH_MS');
    expect(app).toContain('MEDIA_RETRY_MS');
    expect(app).toContain('mediaRefreshRequiredAfterPoll');
    expect(app).toContain('reconnected; refreshing media');
    expect(app).toContain('lastStatusPollOnline[candidate] = true');
    const client = readFileSync(
      'firmware/AdvatekTrigger/src/platform/PixLiteClient.h',
      'utf8',
    );
    expect(client).toContain('now - failureLoggedAt_[index] >= MEDIA_RETRY_MS');
  });

  it('supports per-PixLite next and previous scene actions with wraparound', () => {
    const types = readFileSync('firmware/AdvatekTrigger/src/core/Types.h', 'utf8');
    const policy = readFileSync(
      'firmware/AdvatekTrigger/src/core/SceneStepPolicy.h',
      'utf8',
    );
    const client = readFileSync(
      'firmware/AdvatekTrigger/src/platform/PixLiteClient.h',
      'utf8',
    );
    const web = readFileSync('web/src/main.ts', 'utf8');
    expect(types).toContain('NextScene');
    expect(types).toContain('PreviousScene');
    expect(policy).toContain('previous ? lastScene : firstScene');
    expect(client).toContain('sceneStepIndex(');
    expect(client).toContain('ActionConfig playback = action');
    expect(web).toContain("['nextScene', 'Next scene']");
    expect(web).toContain("['previousScene', 'Previous scene']");
    expect(web).toContain(
      "repeat: ['playScene', 'playPlaylist', 'nextScene', 'previousScene'].includes(kind)",
    );
    expect(web).toContain('Play selected scene');
  });

  it('defaults new playback actions to loop until explicitly stopped', () => {
    const defaults = readFileSync(
      'firmware/AdvatekTrigger/src/core/Defaults.h',
      'utf8',
    );
    const web = readFileSync('web/src/main.ts', 'utf8');
    expect(defaults).toContain('action.repeat = PlaybackRepeat::Forever');
    expect(web).toContain("mediaName: '', repeat: 'forever'");
    expect(web.indexOf('<option value="forever"'))
      .toBeLessThan(web.indexOf('<option value="once"'));
  });

  it('tracks the real PixLite A4-S status response shape', () => {
    const fixture = JSON.parse(
      readFileSync('tests/fixtures/pixlite-a4s-mk3-status-3.14.2.json', 'utf8'),
    );
    const outputs = fixture.result.status.outInt;
    expect(outputs.map((output: {out: string}) => output.out)).toEqual(['Pix', 'Aux:1']);
    expect(outputs.every((output: {progPri: string}) => output.progPri === 'Lo')).toBe(true);
    const client = readFileSync(
      'firmware/AdvatekTrigger/src/platform/PixLiteClient.h',
      'utf8',
    );
    expect(client).toContain('programPriorityIsHigh(priority)');
  });

  it('pins the supported Arduino core and board artifact name', () => {
    expect(manifest.boards[0].arduinoCore).toBe('>=3.3.10 <4');
    expect(manifest.boards[0].artifact).toMatch(/Waveshare-ESP32-S3-ETH\.ino$/);
  });

  it('ships a beginner download path, pinout, and self-contained release guide', () => {
    const readme = readFileSync('README.md', 'utf8');
    const release = readFileSync('.github/workflows/release.yml', 'utf8');
    const startHere = readFileSync('docs/RELEASE-START-HERE.md', 'utf8');
    expect(readme).toContain(
      'AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip',
    );
    expect(readme).toContain(
      'AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip',
    );
    expect(readme).toContain('docs/HARDWARE.md');
    expect(existsSync('docs/waveshare-esp32-s3-eth-project-pinout.svg')).toBe(true);
    expect(release).toContain('docs/RELEASE-START-HERE.md');
    expect(release).toContain('waveshare-esp32-s3-eth-project-pinout.svg');
    expect(startHere).toContain('ESP32S3 Dev Module');
    expect(startHere).toContain('100 ms');
    const directDiagram = readFileSync(
      'docs/assets/hardware-schematics/01-direct-buttons.drawio',
      'utf8',
    );
    const hardware = readFileSync('docs/HARDWARE.md', 'utf8');
    expect(directDiagram).toContain('Button 8');
    expect(directDiagram).toContain('GPIO40');
    expect(hardware).toContain('GND-return wire are the pair');
  });

  it('never serializes known secret fields into redacted config code paths', () => {
    const source = readFileSync('firmware/AdvatekTrigger/src/core/ConfigJson.h', 'utf8');
    expect(source).toContain('bool includeSecrets = false');
    expect(source).toContain('if (includeSecrets)');
    expect(source).toContain('secretsOmitted');
  });

  it('retries only the inactive NVS slot and rolls sequence back on failure', () => {
    const store = readFileSync(
      'firmware/AdvatekTrigger/src/platform/ConfigStore.h',
      'utf8',
    );
    expect(store).toContain('const uint32_t previousSequence = config.sequence');
    expect(store).toContain('preferences_.remove(slot)');
    expect(store).toContain('config.sequence = previousSequence');
  });

  it('uses a conservative but user-adjustable mechanical-contact debounce', () => {
    const defaults = readFileSync('firmware/AdvatekTrigger/src/core/Defaults.h', 'utf8');
    const validation = readFileSync('firmware/AdvatekTrigger/src/core/Validation.h', 'utf8');
    expect(defaults).toContain('input.debounceMs = 100');
    expect(validation).toContain('input.debounceMs < 10 || input.debounceMs > 2000');
  });

  it('commissions over Ethernet and reserves the uniquely named AP for recovery', () => {
    const app = readFileSync('firmware/AdvatekTrigger/src/platform/App.h', 'utf8');
    const defaults = readFileSync('firmware/AdvatekTrigger/src/core/Defaults.h', 'utf8');
    const network = readFileSync('firmware/AdvatekTrigger/src/platform/NetworkManager.h', 'utf8');
    const validation = readFileSync('firmware/AdvatekTrigger/src/core/Validation.h', 'utf8');
    expect(app).toContain('const char *hostname = "advatrigger"');
    expect(app).toContain('"Advatek-Trigger-%s"');
    expect(defaults).toContain('accessPointMode = AccessPointMode::Disabled');
    expect(network).not.toContain('config_.sequence <= 1');
    expect(network).not.toContain('ethernetFailed');
    expect(network).toContain('openRecoveryNetwork()');
    expect(network).toContain('WIFI_MODE_STA');
    expect(network).toContain('Direct Ethernet recovery refused');
    expect(network).toContain('dhcps_start(');
    expect(network).toContain('ETH.config(address, address, mask, address)');
    expect(validation).toContain('validMdnsHostname');
    expect(validation).toContain('cannot start or end with a hyphen');
  });

  it('keeps the required PixLite v1 operation vocabulary in the codec', () => {
    const protocol = readFileSync('firmware/AdvatekTrigger/src/core/PixLiteProtocol.h', 'utf8');
    for (const operation of ['fileList', 'modePlayback', 'modeLive', 'modeTestData', 'setColor', 'Blank', 'progInt', 'HiSet', 'Lo', 'statusRead']) {
      expect(protocol).toContain(operation);
    }
    expect(protocol).not.toContain('\\"req\\":\\"modeCtrl\\"');
    expect(protocol).toContain('\\"op\\":\\"File\\"');
    expect(protocol).toContain('\\"op\\":\\"Blank\\"');
    expect(protocol).not.toContain('\\"mode\\":\\"File\\"');
    expect(protocol).not.toContain('\\"mode\\":\\"Blank\\"');
    expect(protocol).toContain('PixLite v1.9 rejects Lo');
    const releaseGuard = protocol.indexOf('if (strcmp(operation, "Lo") != 0) {');
    const durationField = protocol.indexOf('if (fadeMs) body += ",\\"durMs\\":"');
    expect(releaseGuard).toBeGreaterThan(-1);
    expect(durationField).toBeGreaterThan(releaseGuard);
    expect(durationField).toBeLessThan(protocol.indexOf('body += \'}\';', releaseGuard));
    expect(protocol).toContain('*.scn');
    expect(protocol).toContain('*.pl');
    const configJson = readFileSync(
      'firmware/AdvatekTrigger/src/core/ConfigJson.h',
      'utf8',
    );
    expect(configJson).toContain('"testRed"');
    expect(configJson).toContain('"testGreen"');
    expect(configJson).toContain('"testBlue"');
    const responsePolicy = readFileSync(
      'firmware/AdvatekTrigger/src/core/PixLiteResponsePolicy.h',
      'utf8',
    );
    expect(responsePolicy).toContain('document.objectValue(document.root(), "err")');
    const security = readFileSync('firmware/AdvatekTrigger/src/platform/Security.h', 'utf8');
    expect(security).toContain('sha256Base64Url');
    expect(security).toContain('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_');
  });

  it('makes BOOT recovery visible and cancellable before release', () => {
    const policy = readFileSync(
      'firmware/AdvatekTrigger/src/core/RecoveryPolicy.h',
      'utf8',
    );
    const app = readFileSync('firmware/AdvatekTrigger/src/platform/App.h', 'utf8');
    expect(policy).toContain('heldMs >= 20000');
    expect(policy).toContain('heldMs >= 15000');
    expect(policy).toContain('heldMs >= 5000');
    expect(app).toContain('RecoveryIntent::ClearAuthentication');
    expect(app).toContain('rgbLedWrite(board_.statusLedPin, 255, 0, 0)');
    expect(app).toContain('factoryResetDueAt_ = millis() + 750');
  });
});
