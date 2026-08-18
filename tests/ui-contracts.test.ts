import { describe, expect, it } from 'vitest';
import { readFileSync } from 'node:fs';

const html = readFileSync('web/index.html', 'utf8');
const css = readFileSync('web/src/style.css', 'utf8');
const script = readFileSync('web/src/main.ts', 'utf8');

describe('compact commissioning UI contracts', () => {
  it('exposes every required operational section', () => {
    for (const id of ['status', 'pixlites', 'inputs', 'system', 'network', 'backup', 'diagnostics']) {
      expect(html).toContain(`id="${id}"`);
    }
    expect(html).not.toContain('id="intensity"');
    expect(html).not.toContain('id="intensity-value"');
  });

  it('keeps accessibility, branding, and responsive affordances in the embedded source', () => {
    expect(html).toContain('aria-live="polite"');
    expect(html).toContain('aria-label="Sections"');
    expect(css).toContain('@media(max-width:850px)');
    expect(css).toContain('--orange:#f15a2c');
    expect(html).toContain('ADVATEK COMMUNITY LABS');
    expect(html).not.toContain('class="brand-mark"');
    expect(html).toContain('rel="icon" href="data:image/svg+xml');
    expect(html).toContain("fill='%23f15a2c'");
    expect(html).toContain('id="status-led-enabled"');
    expect(html).toContain('aria-label="Status LED brightness"');
    expect(script).toContain("JSON.stringify({statusLed})");
    expect(html).toContain('id="hostname-preview"');
    expect(script).toContain('http://${hostname}.local/');
    expect(script).toContain("['Device IP', state.uplinkOnline ? state.ip : 'Unavailable',");
    expect(script).toContain('normalizeHostname');
    expect(html).toContain('<h2>Network</h2>');
    expect(html).toContain('Operational Wi-Fi SSID');
    expect(html).not.toContain('id="ap-mode"');
    expect(html).toContain('id="recovery-connection"');
    expect(html).toContain('Direct Ethernet DHCP (unplug LAN first)');
    expect(html).toContain('Wi-Fi has no wired fallback');
    expect(html).toContain('5–14s recovery');
    expect(html).toContain('Wi-Fi recovery opens setup');
    expect(html).toContain('direct Ethernet needs separate power and no LAN');
    expect(script).toContain("recoveryConnection: value('recovery-connection')");
    expect(script).toContain('Enter the operational Wi-Fi network name before switching to Wi-Fi.');
    expect(script).toContain("$<HTMLButtonElement>('apply-network').disabled = invalid");
    expect(script).toContain("if (value('wifi-password')) network.wifiPassword");
    expect(script).not.toContain("confirm('Save network settings");
    expect(script).toContain("button.textContent = 'Tap again to save and restart'");
    expect(script).toContain("button.textContent = 'Settings accepted - restarting'");
    expect(script).toContain('iOS and Android captive-portal browsers');
  });

  it('handles authentication, discovery empty states, and cross-board remapping', () => {
    expect(script).toContain("response.status === 401");
    expect(script).toContain('No PixLite Mk3 controllers found');
    expect(html).toContain('id="devices" class="device-list empty-state" hidden');
    expect(html).toContain('id="close-media"');
    expect(script).toContain('remapRequired');
    expect(html).toContain('20s cancel');
    expect(script).toContain('let pins = [1, 2, 15, 16, 18, 38, 39, 40]');
    expect(script).toContain('config.boardCapabilities?.allowedInputPins');
    expect(script).toContain("config.boardCapabilities?.inputHardware === 'isolated'");
    expect(script).toContain('`DI${terminal} (GPIO ${pin})`');
    expect(script).not.toContain('(camera shared)');
    expect(script).toContain("isolatedInputs ? 'input' : 'pin'");
  });

  it('adds only the inputs an installer needs and defaults them to 100 ms', () => {
    expect(html).toContain('id="add-input"');
    expect(html).toContain('No inputs configured');
    expect(script).toContain("findIndex((input: Json) => !input.enabled)");
    expect(script).toContain('debounceMs: 100');
    expect(script).toContain('data-remove-input');
    expect(script).toContain('active.length >= 8');
  });

  it('autosaves valid input edits without requiring a save button', () => {
    expect(html).toContain('id="input-save-status"');
    expect(html).toContain('id="input-save-status" class="muted" aria-live="polite">Saved');
    expect(html).not.toContain('id="save-inputs"');
    expect(script).toContain("window.setTimeout(saveInputsNow, 800)");
    expect(script).toContain("$('input-list').oninput = scheduleInputSave");
    expect(script).toContain("setInputSaveStatus('Complete required fields')");
    expect(script).toContain("setInputSaveStatus('Saved')");
  });

  it('tests each input as an ordered held make and break action', () => {
    expect(script).toContain('data-test-input="${index}"');
    expect(script).toContain('Hold to test');
    expect(script).toContain("api('/api/action/test'");
    expect(script).toContain('inputTestQueues.get(index)');
    expect(script).toContain('queueInputTest(index, active)');
    expect(script).toContain('button.onpointercancel');
    expect(script).toContain('button.onlostpointercapture');
    expect(script).toContain('button.onblur');
    expect(css).toContain('.test-input.testing');
  });

  it('mirrors debounced make and break events with a per-input virtual LED', () => {
    expect(script).toContain('id="i${index}-activity"');
    expect(script).toContain("api('/api/inputs')");
    expect(script).toContain('setInterval(refreshInputs, 250)');
    expect(script).toContain('sequence - previous');
    expect(script).toContain("setProperty('--pulses'");
    expect(css).toContain('.input-activity.flash');
    expect(css).toContain('@keyframes input-activity-flash');
    expect(css).toContain('background:#fff');
  });

  it('supports sixteen saved PixLite Mk3 controllers and targets every GPIO action explicitly', () => {
    expect(html).toContain('0 of 16 configured');
    expect(script).toContain('targetId:');
    expect(script).toContain('targetOptions(action.targetId)');
    expect(script).toContain('/api/pixlites/remove');
    expect(script).toContain('/api/media?targetId=');
    expect(script).toContain('primeMediaChoices');
    expect(script).toContain('Choose or type a media filename');
    expect(script).toContain('MAC ${escapeHtml(target.mac');
    expect(script).toContain("['testColor', 'Test mode · solid colour']");
    expect(script).toContain("['testColorFade', 'Test mode · RGB colour fade']");
    expect(script).toContain('type="color"');
    expect(script).toContain('colorChannels(testColor)');
    expect(script).toContain("document.addEventListener('pointerdown'");
    expect(script).toContain("active.type === 'color'");
    expect(html).toContain('PixLite Mk3 controllers');
    expect(html).toContain('Discover PixLite Mk3 controllers');
    expect(html).not.toContain('>PixLites<');
    expect(html).not.toContain('>Add PixLite<');
  });
});
