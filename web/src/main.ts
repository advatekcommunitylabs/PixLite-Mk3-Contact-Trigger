import './style.css';

type Json = Record<string, any>;
type Edge = 'onAction' | 'offAction';

class ApiError extends Error {
  constructor(message: string, readonly payload: Json) { super(message); }
}

let config: Json;
let state: Json = {};
let password = sessionStorage.getItem('advatekPassword') ?? '';
let pins = [1, 2, 15, 16, 18, 38, 39, 40];
let isolatedInputs = false;
let inputSaveTimer: number | undefined;
let inputSaveRevision = 0;
let inputSaveActive = false;
let inputPollActive = false;
const inputEventSequences: number[] = [];
const media = new Map<string, Json>();
const inputTestQueues = new Map<number, Promise<void>>();
const kinds = [
  ['none', 'No action'], ['playScene', 'Play scene'], ['playPlaylist', 'Play playlist'],
  ['nextScene', 'Next scene'], ['previousScene', 'Previous scene'],
  ['stop', 'Live mode (stop playback)'], ['blank', 'Blank outputs'], ['testColor', 'Test mode · solid colour'],
  ['testColorFade', 'Test mode · RGB colour fade'],
  ['brighter', 'Intensity brighter'],
  ['darker', 'Intensity darker'], ['setIntensity', 'Set intensity'], ['releaseIntensity', 'Release override'],
];
const $ = <T extends HTMLElement>(id: string) => document.getElementById(id) as T;
const value = (id: string) => ($<HTMLInputElement | HTMLSelectElement>(id)).value;
const setValue = (id: string, data: unknown) => ($<HTMLInputElement | HTMLSelectElement>(id)).value = String(data ?? '');
const checked = (id: string) => $<HTMLInputElement>(id).checked;
const escapeHtml = (data: unknown) => String(data ?? '').replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]!));

// Chromium's native colour popup does not dismiss consistently on embedded
// Windows browsers. Explicitly blur it when the installer clicks elsewhere.
document.addEventListener('pointerdown', event => {
  const active = document.activeElement;
  if (active instanceof HTMLInputElement &&
      active.type === 'color' &&
      event.target !== active) {
    active.blur();
  }
}, true);
const inputLabel = (pin: number) => {
  const terminal = pins.indexOf(pin) + 1;
  return isolatedInputs && terminal > 0
    // Lead with the enclosure terminal printed on the industrial board. The
    // underlying ESP32 GPIO remains in brackets for installers and diagnostics.
    ? `DI${terminal} (GPIO ${pin})`
    : `GPIO ${pin}`;
};

async function api(path: string, init: RequestInit = {}) {
  const headers = new Headers(init.headers);
  if (password) headers.set('X-Advatek-Password', password);
  if (init.body) headers.set('Content-Type', 'application/json');
  const response = await fetch(path, {...init, headers});
  if (response.status === 401) {
    const entered = prompt('Advatek Trigger password');
    if (entered === null) throw new Error('Password required');
    password = entered;
    sessionStorage.setItem('advatekPassword', password);
    return api(path, init);
  }
  const data = await response.json();
  if (!response.ok) throw new ApiError(data.error || `Request failed (${response.status})`, data);
  return data;
}

function toast(message: unknown, error = false) {
  const target = $('toast');
  target.textContent = message instanceof Error ? message.message : String(message);
  target.style.borderLeftColor = error ? '#d6503b' : '#f15a2c';
  target.classList.add('show');
  setTimeout(() => target.classList.remove('show'), 3500);
}

function normalizeHostname(text: string) {
  return text.trim().toLowerCase().replace(/^https?:\/\//, '').replace(/\.local\/?$/, '')
    .replace(/[^a-z0-9-]+/g, '-').replace(/-+/g, '-').replace(/^-|-$/g, '')
    .slice(0, 31).replace(/-$/, '');
}

function defaultAction(): Json {
  return {
    kind: 'none', targetId: config?.primaryPixliteId || config?.pixlites?.[0]?.id || 'primary',
    mediaName: '', repeat: 'forever', output: 'pixels', auxOutput: 1,
    intensityPercent: 100, stepPercent: 5, holdDelayMs: 400,
    repeatIntervalMs: 250, fadeMs: 250, testColor: '#ff0000',
  };
}

function isGenericPixLiteName(name: string) {
  return !name || name === 'Primary PixLite' || name === 'Primary PixLite Mk3' ||
    /^PixLite(?: Mk3)? \d+$/.test(name);
}

function colorChannels(hex: string) {
  const normalized = /^#[0-9a-f]{6}$/i.test(hex) ? hex : '#ff0000';
  return {
    testRed: Number.parseInt(normalized.slice(1, 3), 16),
    testGreen: Number.parseInt(normalized.slice(3, 5), 16),
    testBlue: Number.parseInt(normalized.slice(5, 7), 16),
  };
}

function targetOptions(selected: string) {
  const choices = (config.pixlites || []).map((p: Json) =>
    `<option value="${escapeHtml(p.id)}" ${selected === p.id ? 'selected' : ''}>${escapeHtml(p.name || p.host)}</option>`).join('');
  return choices || '<option value="">Add a PixLite Mk3 first</option>';
}

function renderStatus() {
  const online = (state.pixlites || []).filter((p: Json) => p.online).length;
  const enabled = (config.inputs || []).filter((input: Json) => input.enabled).length;
  const address = state.hostname ? `http://${state.hostname}.local/` : 'Unavailable';
  $('connection').innerHTML = `<i></i>${escapeHtml(state.uplinkOnline ? `${state.uplink} · ${state.ip}` : `${state.uplink || 'Network'} offline`)}`;
  $('connection').className = `connection ${state.uplinkOnline ? 'online' : 'offline'}`;
  const cards = [
    ['Device IP', state.uplinkOnline ? state.ip : 'Unavailable', state.uplinkOnline ? `http://${state.ip}/` : ''],
    ['Local address', address, state.hostname ? `http://${state.hostname}.local/` : ''],
    ['PixLite Mk3 controllers', `${online} online · ${(config.pixlites || []).length} added`, ''],
    ['Inputs', `${enabled} configured${state.pinRemappingRequired ? ' · remap required' : ''}`, ''],
    ['Ethernet', state.ethernet?.linkUp ? `${state.ethernet.linkSpeedMbps} Mbps ${state.ethernet.fullDuplex ? 'full duplex' : 'half duplex'}` : state.ethernet?.failureReason || 'Link down', ''],
    ['Memory', state.degradedMode ? 'Degraded recovery mode' : `${Math.round((state.memory?.internalFree || 0) / 1024)} KB internal free`, ''],
    ['Firmware', state.firmwareVersion || '—', ''],
    ['Uptime', `${Math.floor((state.memory?.uptimeMs || 0) / 60000)} minutes`, ''],
  ];
  $('status-grid').innerHTML = cards.map(([label, content, href]) =>
    `<div class="status-card"><small>${escapeHtml(label)}</small>${href
      ? `<a href="${escapeHtml(href)}" title="Open ${escapeHtml(content)}">${escapeHtml(content)}</a>`
      : `<strong title="${escapeHtml(content)}">${escapeHtml(content)}</strong>`}</div>`).join('');
  const steps = [
    ['Connect the network', !!state.uplinkOnline, '#network'],
    ['Add a PixLite Mk3', !!config.pixlites?.length, '#pixlites'],
    ['Add an input', enabled > 0, '#inputs'],
  ];
  $('quick-start').innerHTML = steps.map(([label, done, href], index) =>
    `<a class="step ${done ? 'done' : ''}" href="${href}"><i>${done ? '✓' : index + 1}</i>${label}</a>`).join('');
  const done = steps.filter(x => x[1]).length;
  $('setup-progress').textContent = done === 3 ? 'Ready' : `${done} of 3 setup steps`;
}

async function refreshState() {
  try {
    state = await api('/api/state');
    if (!state.pixlites) state.pixlites = state.pixlite?.host
      ? [{id: 'primary', name: 'Primary PixLite Mk3', ...state.pixlite}]
      : [];
    renderStatus();
    renderSavedPixLites();
    updateInputActivity(state.inputs);
  } catch (error) { toast(error, true); }
}

function updateInputActivity(inputs: Json[] = []) {
  inputs.forEach((live: Json, index: number) => {
    const led = document.getElementById(`i${index}-activity`);
    if (led) {
      led.classList.toggle('active', !!live.active);
      led.title = live.active ? 'Contact active' : 'Contact inactive';
    }

    const sequence = Number(live.eventSequence || 0) >>> 0;
    const previous = inputEventSequences[index];
    inputEventSequences[index] = sequence;
    // A lower value means the controller restarted; establish a new baseline
    // instead of replaying old events as a large unsigned wraparound.
    if (previous === undefined || sequence < previous) return;
    const pulses = Math.min(sequence - previous, 4);
    if (led && pulses) {
      led.style.setProperty('--pulses', String(pulses));
      led.classList.remove('flash');
      void led.offsetWidth;
      led.classList.add('flash');
    }
  });
}

async function refreshInputs() {
  if (inputPollActive) return;
  inputPollActive = true;
  try {
    const snapshot = await api('/api/inputs');
    state.inputs = snapshot.inputs || [];
    updateInputActivity(state.inputs);
  } catch {
    // The regular status poll reports connection errors. Keep this frequent,
    // cosmetic poll quiet so a disconnected device cannot flood notifications.
  } finally {
    inputPollActive = false;
  }
}

function renderSavedPixLites() {
  const targets = config.pixlites || [];
  $('pixlite-count').textContent = `${targets.length} of ${config.maxPixlites || 16} configured`;
  $('saved-pixlites').innerHTML = targets.map((target: Json) => {
    const live = (state.pixlites || []).find((item: Json) => item.id === target.id) || {};
    return `<div class="saved-device">
      <span class="state-dot ${live.online ? 'online' : ''}"></span>
      <span class="device-info"><b>${escapeHtml(target.name || target.host)}</b><small>IP ${escapeHtml(target.host)} · MAC ${escapeHtml(target.mac || live.mac || 'unknown')}<br>${live.online ? `${escapeHtml(live.mode || 'Ready')} · ${live.latencyMs || 0} ms` : escapeHtml(live.error || 'Not connected')}</small></span>
      <span class="device-actions"><button data-media="${escapeHtml(target.id)}">View media</button><button data-edit="${escapeHtml(target.id)}">Edit</button><button class="danger" data-remove="${escapeHtml(target.id)}">Remove</button></span>
    </div>`;
  }).join('') || '<div class="empty-state"><b>No PixLite Mk3 controllers saved</b><span>Discover one automatically or add its IP address.</span></div>';
  $('saved-pixlites').querySelectorAll<HTMLButtonElement>('[data-media]').forEach(button => button.onclick = () => loadMedia(button.dataset.media!));
  $('saved-pixlites').querySelectorAll<HTMLButtonElement>('[data-edit]').forEach(button => button.onclick = () => openPixLite(config.pixlites.find((p: Json) => p.id === button.dataset.edit)));
  $('saved-pixlites').querySelectorAll<HTMLButtonElement>('[data-remove]').forEach(button => button.onclick = () => forgetPixLite(button.dataset.remove!));
}

function openPixLite(target: Json = {}) {
  $('pixlite-form').hidden = false;
  $('pixlite-form-title').textContent = target.id ? 'Edit PixLite Mk3' : 'Add PixLite Mk3';
  setValue('pix-target-id', target.id || '');
  setValue('pix-mac', target.mac || '');
  setValue('pix-name', target.name || '');
  setValue('pix-host', target.host || '');
  setValue('pix-user', target.username || 'oper');
  setValue('pix-password', '');
  $('pixlite-form').scrollIntoView({behavior: 'smooth', block: 'nearest'});
}

async function loadMedia(targetId: string) {
  try {
    const result = await api(`/api/media?targetId=${encodeURIComponent(targetId)}`);
    media.set(targetId, result);
    const target = config.pixlites.find((p: Json) => p.id === targetId);
    $('media-title').textContent = target?.name || target?.host || 'PixLite Mk3';
    $('scenes').innerHTML = result.scenes.map((name: string) => `<li>${escapeHtml(name)}</li>`).join('') || '<li class="muted">No scenes</li>';
    $('playlists').innerHTML = result.playlists.map((name: string) => `<li>${escapeHtml(name)}</li>`).join('') || '<li class="muted">No playlists</li>';
    $('media-panel').hidden = false;
    renderInputs();
  } catch (error) { toast(error, true); }
}

async function primeMediaChoices() {
  // A saved action must offer scenes immediately after a controller reboot.
  // Fetch each bounded catalogue sequentially so a page containing many saved
  // PixLite Mk3 controllers cannot overwhelm the ESP32 HTTP server with
  // parallel requests.
  let changed = false;
  for (const target of config.pixlites || []) {
    if (media.has(target.id)) continue;
    try {
      media.set(target.id, await api(`/api/media?targetId=${encodeURIComponent(target.id)}`));
      changed = true;
    } catch {
      // The status poll reports offline controllers. Keep the rest of the
      // input editor usable and allow View media to retry this target later.
    }
  }
  if (changed) {
    captureInputs();
    renderInputs();
  }
}

async function forgetPixLite(targetId: string) {
  const target = config.pixlites.find((p: Json) => p.id === targetId);
  if (!confirm(`Forget ${target?.name || target?.host}? Inputs using it must be retargeted first.`)) return;
  try {
    await api('/api/pixlites/remove', {method: 'POST', body: JSON.stringify({targetId})});
    media.delete(targetId);
    await reloadConfig();
    toast('PixLite Mk3 forgotten');
  } catch (error) { toast(error, true); }
}

function mediaList(action: Json) {
  const files = media.get(action.targetId);
  const names = action.kind === 'playPlaylist' ? files?.playlists : files?.scenes;
  return (names || []).map((name: string) => `<option value="${escapeHtml(name)}"></option>`).join('');
}

function actionEditor(index: number, edge: Edge, title: string, action: Json) {
  const prefix = `i${index}-${edge}`;
  return `<div class="action" data-action="${prefix}">
    <div class="action-title">${title}</div>
    <div class="action-grid">
      <label>Action<select id="${prefix}-kind">${kinds.map(([kind, label]) => `<option value="${kind}" ${action.kind === kind ? 'selected' : ''}>${label}</option>`).join('')}</select></label>
      <label data-field="target">Destination PixLite Mk3<select id="${prefix}-target">${targetOptions(action.targetId)}</select></label>
      <label class="span-2" data-field="media">Scene or playlist<input id="${prefix}-media" value="${escapeHtml(action.mediaName)}" list="${prefix}-files" placeholder="Choose or type a media filename" /><datalist id="${prefix}-files">${mediaList(action)}</datalist></label>
      <span class="field-help span-2" data-field="sceneStep">Uses alphabetical scene order and wraps at either end.</span>
      <label data-field="repeat">Play selected scene<select id="${prefix}-repeat"><option value="forever" ${action.repeat === 'forever' ? 'selected' : ''}>Loop until stopped</option><option value="once" ${action.repeat === 'once' ? 'selected' : ''}>Once</option></select></label>
      <label class="span-2" data-field="color">Test colour<div class="color-row"><input id="${prefix}-color" type="color" value="${escapeHtml(action.testColor || '#ff0000')}" aria-label="${title} test colour" /><output id="${prefix}-color-value">${escapeHtml(String(action.testColor || '#ff0000').toUpperCase())}</output></div><span class="field-help">Applies to all pixels until Live mode. Requires the destination's Administrator account.</span></label>
      <label data-field="output">Output<select id="${prefix}-output"><option value="pixels">Pixels</option><option value="aux" ${action.output === 'aux' ? 'selected' : ''}>Aux output</option><option value="pixelsAndAux" ${action.output === 'pixelsAndAux' ? 'selected' : ''}>Pixels + configured Aux</option></select></label>
      <label data-field="aux">Aux output #<input id="${prefix}-aux" type="number" min="1" max="8" value="${action.auxOutput ?? 1}" /></label>
      <label data-field="intensity">Intensity %<input id="${prefix}-intensity" type="number" min="0" max="100" value="${action.intensityPercent ?? 100}" /></label>
      <label data-field="step">Change per step %<input id="${prefix}-step" type="number" min="1" max="100" value="${action.stepPercent ?? 5}" /></label>
    </div>
    <details class="action-advanced" data-field="timing"><summary>Hold and fade timing</summary><div class="action-grid">
      <label data-field="hold">Hold before repeat ms<input id="${prefix}-hold" type="number" min="50" max="5000" value="${action.holdDelayMs ?? 400}" /></label>
      <label data-field="interval">Repeat every ms<input id="${prefix}-interval" type="number" min="50" max="5000" value="${action.repeatIntervalMs ?? 250}" /></label>
      <label data-field="fade">Fade ms<input id="${prefix}-fade" type="number" min="0" max="10000" value="${action.fadeMs ?? 250}" /></label>
    </div></details>
  </div>`;
}

// The device retains eight fixed input slots for deterministic memory use. The UI
// exposes only enabled slots, so a one-button installation starts with one card.
function renderInputs() {
  const active = config.inputs.map((input: Json, index: number) => ({input, index})).filter((x: Json) => x.input.enabled);
  $('input-count').textContent = `${active.length} of 8 inputs`;
  $('input-empty').hidden = active.length > 0;
  $('add-input').toggleAttribute('disabled', active.length >= 8);
  $('input-list').innerHTML = active.map(({input, index}: Json) => `
    <details class="input-card" data-input="${index}" open>
      <summary><span class="input-summary"><span class="input-title"><i id="i${index}-activity" class="input-activity" role="img" aria-label="Input activity; flashes white on contact make and break"></i><strong>${escapeHtml(input.name || `Input ${index + 1}`)}</strong></span><small>${input.gpio == null ? `Select ${isolatedInputs ? 'an input terminal' : 'a pin'}` : inputLabel(input.gpio)} · ${input.mode === 'maintained' ? 'Maintained switch' : 'Momentary button'}</small></span><span class="tag">${input.debounceMs || 100} ms debounce</span></summary>
      <div class="input-body">
        <div class="input-basics">
          <label>Name<input id="i${index}-name" maxlength="31" value="${escapeHtml(input.name || `Input ${index + 1}`)}" /></label>
          <label>${isolatedInputs ? 'Input terminal' : 'GPIO pin'}<select id="i${index}-gpio"><option value="-1">Select ${isolatedInputs ? 'input' : 'pin'}</option>${pins.map(pin => `<option value="${pin}" ${input.gpio === pin ? 'selected' : ''}>${inputLabel(pin)}</option>`).join('')}</select></label>
          <label>Contact type<select id="i${index}-mode"><option value="momentary">Momentary button</option><option value="maintained" ${input.mode === 'maintained' ? 'selected' : ''}>Maintained switch</option></select></label>
          <label>Debounce (ms)<input id="i${index}-debounce" type="number" min="10" max="2000" value="${input.debounceMs || 100}" /></label>
          <label class="polarity"><input id="i${index}-nc" type="checkbox" ${input.normallyClosed ? 'checked' : ''} /> Normally closed</label>
          <span class="input-controls">
            <button type="button" class="test-input" data-test-input="${index}">Hold to test</button>
            <button type="button" class="danger remove-input" data-remove-input="${index}">Remove</button>
          </span>
        </div>
        <div class="actions">${actionEditor(index, 'onAction', input.mode === 'maintained' ? 'Latch on · contact closes' : 'Press · contact closes', input.onAction)}${actionEditor(index, 'offAction', input.mode === 'maintained' ? 'Latch off · contact opens' : 'Release · contact opens', input.offAction)}</div>
      </div>
    </details>`).join('');
  $('input-list').querySelectorAll<HTMLButtonElement>('[data-remove-input]').forEach(button => button.onclick = event => {
    event.preventDefault();
    const index = Number(button.dataset.removeInput);
    config.inputs[index] = {...config.inputs[index], enabled: false, gpio: null, debounceMs: 100, onAction: defaultAction(), offAction: defaultAction()};
    renderInputs();
    scheduleInputSave();
  });
  $('input-list').querySelectorAll<HTMLButtonElement>('[data-test-input]').forEach(button => {
    const index = Number(button.dataset.testInput);
    button.onpointerdown = event => {
      if (event.button !== 0) return;
      button.setPointerCapture(event.pointerId);
      setInputTest(index, button, true);
    };
    const release = () => setInputTest(index, button, false);
    button.onpointerup = release;
    button.onpointercancel = release;
    button.onlostpointercapture = release;
    button.onkeydown = event =>
      !event.repeat &&
      [' ', 'Enter'].includes(event.key) &&
      setInputTest(index, button, true);
    button.onkeyup = release;
    button.onblur = release;
  });
  active.forEach(({index}: Json) => {
    $<HTMLSelectElement>(`i${index}-mode`).onchange = () => { captureInputs(); renderInputs(); scheduleInputSave(); };
    (['onAction', 'offAction'] as Edge[]).forEach(edge => {
      $<HTMLSelectElement>(`i${index}-${edge}-kind`).onchange = () => updateActionFields(index, edge);
      $<HTMLSelectElement>(`i${index}-${edge}-target`).onchange = () => updateActionMedia(index, edge);
      $<HTMLSelectElement>(`i${index}-${edge}-output`).onchange = () => updateActionFields(index, edge);
      const color = $<HTMLInputElement>(`i${index}-${edge}-color`);
      color.oninput = () => $(`i${index}-${edge}-color-value`).textContent = color.value.toUpperCase();
      updateActionFields(index, edge);
    });
  });
  updateInputActivity(state.inputs);
}

// Each action shows only parameters the selected PixLite Mk3 API operation uses.
function updateActionFields(index: number, edge: Edge) {
  const prefix = `i${index}-${edge}`;
  const kind = value(`${prefix}-kind`);
  const output = value(`${prefix}-output`);
  const visible: Record<string, boolean> = {
    target: kind !== 'none', media: kind === 'playScene' || kind === 'playPlaylist',
    sceneStep: kind === 'nextScene' || kind === 'previousScene',
    repeat: ['playScene', 'playPlaylist', 'nextScene', 'previousScene'].includes(kind),
    color: kind === 'testColor',
    output: ['brighter', 'darker', 'setIntensity', 'releaseIntensity'].includes(kind),
    aux: ['brighter', 'darker', 'setIntensity', 'releaseIntensity'].includes(kind) && output === 'aux',
    intensity: kind === 'setIntensity', step: kind === 'brighter' || kind === 'darker',
    timing: ['brighter', 'darker', 'setIntensity'].includes(kind),
    hold: kind === 'brighter' || kind === 'darker', interval: kind === 'brighter' || kind === 'darker',
    fade: ['brighter', 'darker', 'setIntensity'].includes(kind),
  };
  document.querySelectorAll<HTMLElement>(`[data-action="${prefix}"] [data-field]`).forEach(node => node.hidden = !visible[node.dataset.field!]);
}

function updateActionMedia(index: number, edge: Edge) {
  captureInputs();
  renderInputs();
  scheduleInputSave();
}

function readAction(index: number, edge: Edge, previous: Json) {
  const prefix = `i${index}-${edge}`;
  const testColor = value(`${prefix}-color`);
  return {...previous, kind: value(`${prefix}-kind`), targetId: value(`${prefix}-target`),
    mediaName: value(`${prefix}-media`), repeat: value(`${prefix}-repeat`), output: value(`${prefix}-output`),
    auxOutput: Number(value(`${prefix}-aux`)), intensityPercent: Number(value(`${prefix}-intensity`)),
    stepPercent: Number(value(`${prefix}-step`)), holdDelayMs: Number(value(`${prefix}-hold`)),
    repeatIntervalMs: Number(value(`${prefix}-interval`)), fadeMs: Number(value(`${prefix}-fade`)),
    testColor, ...colorChannels(testColor)};
}

function queueInputTest(index: number, active: boolean) {
  // Preserve network ordering when a quick click releases before its press
  // request has completed. The break action must always follow the make action.
  captureInputs();
  const input = config.inputs[index];
  const action = {...(active ? input.onAction : input.offAction)};
  const previous = inputTestQueues.get(index) ?? Promise.resolve();
  const next = previous.catch(() => undefined).then(async () => {
    await api('/api/action/test', {
      method: 'POST',
      body: JSON.stringify(action),
    });
  });
  inputTestQueues.set(index, next);
  next.catch(error => toast(error, true));
}

function setInputTest(index: number, button: HTMLButtonElement, active: boolean) {
  if (button.classList.contains('testing') === active) return;
  button.classList.toggle('testing', active);
  button.textContent = active ? 'Testing… release' : 'Hold to test';
  queueInputTest(index, active);
}

function captureInputs() {
  config.inputs.forEach((input: Json, index: number) => {
    if (!input.enabled || !document.getElementById(`i${index}-name`)) return;
    const pin = Number(value(`i${index}-gpio`));
    config.inputs[index] = {...input, name: value(`i${index}-name`).trim() || `Input ${index + 1}`,
      gpio: pin < 0 ? null : pin, mode: value(`i${index}-mode`),
      debounceMs: Number(value(`i${index}-debounce`)) || 100, normallyClosed: checked(`i${index}-nc`),
      onAction: readAction(index, 'onAction', input.onAction), offAction: readAction(index, 'offAction', input.offAction)};
  });
}

function validateInputs() {
  const used = new Set<number>();
  for (const input of config.inputs.filter((item: Json) => item.enabled)) {
    if (input.gpio == null) throw new Error(`${input.name}: select ${isolatedInputs ? 'an input terminal' : 'a GPIO pin'}`);
    if (used.has(input.gpio)) throw new Error(`${inputLabel(input.gpio)} is assigned more than once`);
    used.add(input.gpio);
    if (input.debounceMs < 10 || input.debounceMs > 2000) throw new Error(`${input.name}: debounce must be 10–2000 ms`);
    for (const action of [input.onAction, input.offAction]) {
      if (action.kind !== 'none' && !config.pixlites.some((p: Json) => p.id === action.targetId)) throw new Error(`${input.name}: choose a PixLite Mk3 for each action`);
      if ((action.kind === 'playScene' || action.kind === 'playPlaylist') && !action.mediaName) throw new Error(`${input.name}: choose a scene or playlist`);
    }
  }
}

const setInputSaveStatus = (message: string) => $('input-save-status').textContent = message;

// Input edits are collected locally immediately, then written as one atomic
// configuration update after the installer pauses. This avoids excessive NVS
// writes while typing and prevents an older, slower request winning a race.
function scheduleInputSave() {
  captureInputs();
  inputSaveRevision++;
  window.clearTimeout(inputSaveTimer);
  setInputSaveStatus('Unsaved changes');
  inputSaveTimer = window.setTimeout(saveInputsNow, 800);
}

async function saveInputsNow() {
  inputSaveTimer = undefined;
  if (inputSaveActive) {
    inputSaveTimer = window.setTimeout(saveInputsNow, 200);
    return;
  }
  try {
    captureInputs();
    validateInputs();
  } catch (error) {
    setInputSaveStatus('Complete required fields');
    return;
  }

  inputSaveActive = true;
  const revision = inputSaveRevision;
  setInputSaveStatus('Saving...');
  try {
    await api('/api/config', {method: 'POST', body: JSON.stringify({inputs: config.inputs})});
    if (revision === inputSaveRevision) setInputSaveStatus('Saved');
  } catch (error) {
    setInputSaveStatus('Retrying save');
    inputSaveTimer = window.setTimeout(saveInputsNow, 3000);
  } finally {
    inputSaveActive = false;
  }
}

function renderConfig() {
  pins = config.boardCapabilities?.allowedInputPins ?? pins;
  isolatedInputs = config.boardCapabilities?.inputHardware === 'isolated';
  $('status-led-enabled').toggleAttribute('checked', config.statusLed?.enabled);
  $<HTMLInputElement>('status-led-enabled').checked = config.statusLed?.enabled;
  setValue('status-led-brightness', config.statusLed?.brightnessPercent ?? 100);
  updateLed();
  setValue('uplink', config.network.uplink);
  setValue('hostname', config.network.hostname);
  setValue('wifi-ssid', config.network.wifiSsid);
  setValue('recovery-connection', config.network.recoveryConnection || 'wifi');
  renderIpFields();
  updateHostname();
  renderSavedPixLites();
  renderInputs();
}

async function reloadConfig() {
  config = await api('/api/config');
  normalizeLegacyConfig();
  renderConfig();
  await refreshState();
}

// This adapter lets maintainers preview a new UI against an older bench unit;
// release firmware and its embedded UI always ship together on schema v5.
function normalizeLegacyConfig() {
  if (!config.pixlites) {
    config.pixlites = config.pixlite?.host ? [{
      id: 'primary', name: 'Primary PixLite Mk3', mac: config.pixlite.mac,
      host: config.pixlite.host, port: config.pixlite.port, apiVersion: config.pixlite.apiVersion,
      username: config.pixlite.username,
    }] : [];
    config.primaryPixliteId = config.pixlites[0]?.id || '';
    config.maxPixlites = 16;
  }
}

function updateLed() {
  const enabled = checked('status-led-enabled');
  $<HTMLInputElement>('status-led-brightness').disabled = !enabled;
  $('status-led-output').textContent = enabled ? `${value('status-led-brightness')}%` : 'Off';
}

function updateHostname() {
  const hostname = normalizeHostname(value('hostname'));
  $('hostname-preview').textContent = hostname ? `http://${hostname}.local/` : 'Enter a local name';
}

function renderIpFields() {
  const settings = config.network[value('uplink')];
  setValue('ip-mode', settings.mode); setValue('ip-address', settings.address);
  setValue('ip-netmask', settings.netmask); setValue('ip-gateway', settings.gateway); setValue('ip-dns', settings.dns);
}

$('theme-toggle').onclick = () => {
  const next = document.documentElement.dataset.theme === 'light' ? 'dark' : 'light';
  document.documentElement.dataset.theme = next;
  localStorage.setItem('advatekTheme', next);
};

$('discover').onclick = async () => {
  try {
    $('devices').hidden = false;
    $('discovery-message').textContent = 'Searching the local network…';
    const result = await api('/api/discover', {method: 'POST'});
    $('discovery-message').textContent = `${result.devices.length} found${result.truncated ? ' (list truncated)' : ''}`;
    $('devices').innerHTML = result.devices.map((device: Json) => `<div class="device">
      <span class="device-info"><b>${escapeHtml(device.nickname || device.name || device.ip)}</b><small>${escapeHtml(device.name)} · IP ${escapeHtml(device.ip)} · MAC ${escapeHtml(device.mac)} · firmware ${escapeHtml(device.firmware)}</small></span>
      <button data-device="${escapeHtml(device.mac)}">${device.configuredId ? 'Edit' : 'Add'}</button></div>`).join('') || '<div class="empty-state"><b>No PixLite Mk3 controllers found</b><span>Check Ethernet and that the controller is on this network, or add its IP manually.</span></div>';
    $('devices').querySelectorAll<HTMLButtonElement>('[data-device]').forEach(button => button.onclick = () => {
      const device = result.devices.find((item: Json) => item.mac === button.dataset.device);
      const saved = config.pixlites.find((item: Json) => item.id === device.configuredId);
      const discoveredName = device.nickname || device.name || device.ip;
      openPixLite({...saved, mac: device.mac, host: device.ip,
        name: !saved || isGenericPixLiteName(saved.name) ? discoveredName : saved.name});
    });
  } catch (error) { toast(error, true); }
};

$('manual-pixlite').onclick = () => openPixLite();
$('cancel-pixlite').onclick = () => $('pixlite-form').hidden = true;
$('close-media').onclick = () => $('media-panel').hidden = true;
$<HTMLFormElement>('pixlite-form').onsubmit = async event => {
  event.preventDefault();
  try {
    const result = await api('/api/connect', {method: 'POST', body: JSON.stringify({
      targetId: value('pix-target-id'), name: value('pix-name'), mac: value('pix-mac'),
      host: value('pix-host'), username: value('pix-user'), password: value('pix-password'),
    })});
    $('pixlite-form').hidden = true;
    await reloadConfig();
    await loadMedia(result.targetId);
    toast('PixLite Mk3 connected and saved');
  } catch (error) { toast(error, true); }
};

$('add-input').onclick = () => {
  captureInputs();
  const index = config.inputs.findIndex((input: Json) => !input.enabled);
  if (index < 0) return;
  config.inputs[index] = {...config.inputs[index], enabled: true, name: `Input ${index + 1}`, gpio: null,
    normallyClosed: false, mode: 'momentary', debounceMs: 100, onAction: defaultAction(), offAction: defaultAction()};
  renderInputs();
  scheduleInputSave();
};

// Delegation survives renderInputs() replacing individual editor cards.
$('input-list').oninput = scheduleInputSave;
$('input-list').onchange = scheduleInputSave;

$<HTMLInputElement>('status-led-enabled').onchange = updateLed;
$<HTMLInputElement>('status-led-brightness').oninput = updateLed;
$('save-status-led').onclick = async () => {
  try {
    const statusLed = {enabled: checked('status-led-enabled'), brightnessPercent: Number(value('status-led-brightness'))};
    await api('/api/config', {method: 'POST', body: JSON.stringify({statusLed})});
    await reloadConfig();
    toast(statusLed.enabled ? 'Status LED updated' : 'Status LED switched off');
  } catch (error) { toast(error, true); }
};

$<HTMLInputElement>('hostname').oninput = updateHostname;
$<HTMLInputElement>('hostname').onblur = () => { setValue('hostname', normalizeHostname(value('hostname'))); updateHostname(); };
$<HTMLSelectElement>('uplink').onchange = renderIpFields;
$('apply-network').onclick = async () => {
  if (!confirm('Save network settings and restart this controller?')) return;
  try {
    const uplink = value('uplink');
    const hostname = normalizeHostname(value('hostname'));
    if (!hostname) throw new Error('Enter a local name using letters, numbers or hyphens');
    const addressing = {mode: value('ip-mode'), address: value('ip-address'), netmask: value('ip-netmask'), gateway: value('ip-gateway'), dns: value('ip-dns')};
    await api('/api/network/apply', {method: 'POST', body: JSON.stringify({network: {
      uplink, hostname, wifiSsid: value('wifi-ssid'), wifiPassword: value('wifi-password'),
      recoveryConnection: value('recovery-connection'), apPassword: value('web-password'), [uplink]: addressing,
    }})});
    toast('Saved; the controller is restarting');
  } catch (error) { toast(error, true); }
};

$('export').onclick = async () => {
  try {
    const backup = await api('/api/config/export');
    const url = URL.createObjectURL(new Blob([JSON.stringify(backup, null, 2)], {type: 'application/json'}));
    const link = document.createElement('a'); link.href = url; link.download = 'advatek-trigger-backup.json'; link.click();
    URL.revokeObjectURL(url);
  } catch (error) { toast(error, true); }
};

$<HTMLInputElement>('import').onchange = async event => {
  const file = (event.target as HTMLInputElement).files?.[0];
  if (!file) return;
  const text = await file.text();
  try {
    await api('/api/config/import', {method: 'POST', body: text});
    await reloadConfig();
    toast('Backup imported');
  } catch (error) {
    if (error instanceof ApiError && error.payload.remapRequired) {
      config = JSON.parse(text); renderConfig();
      toast('Backup loaded. Select valid GPIOs before saving.', true);
    } else toast(error, true);
  }
};

$('refresh-log').onclick = async () => {
  try {
    const entries = await api('/api/log');
    $('log').textContent = entries.map((entry: Json) => `${String(entry.timeMs).padStart(10)}  ${entry.level.toUpperCase().padEnd(7)} ${entry.message}`).join('\n') || 'No entries.';
  } catch (error) { toast(error, true); }
};

async function start() {
  document.documentElement.dataset.theme = localStorage.getItem('advatekTheme') || 'dark';
  try {
    config = await api('/api/config');
    normalizeLegacyConfig();
    renderConfig();
    await refreshState();
    void primeMediaChoices();
  } catch (error) { toast(error, true); }
  setInterval(refreshState, 2000);
  setInterval(refreshInputs, 250);
}
start();
