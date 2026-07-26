# Advatek PixLite Mk3 Contact Closure Trigger

## GitHub project summary

The Advatek PixLite Mk3 Contact Closure Trigger is an open-source ESP32 appliance
that connects physical push buttons, relay contacts, and maintained switches to
a PixLite Mk3. It gives installers and operators a compact local web interface
for discovering a PixLite Mk3, selecting scenes or playlists, assigning GPIOs, and
configuring playback and intensity actions without writing custom firmware.

The supported targets are the Waveshare ESP32-S3-ETH development board and the
Waveshare ESP32-S3-(POE)-ETH-8DI-8RO industrial board. Both use W5500 Ethernet,
16 MB flash, and 8 MB PSRAM; each standard/PoE pair shares one firmware
artifact because PoE only changes the power path. The firmware is structured
as reusable core, ESP32 platform, and board-profile layers so future ESP32
boards can be added without forking PixLite Mk3, ADAR, trigger, configuration, or
web-interface code.

Suggested GitHub description:

> Open-source ESP32 contact-closure controller for triggering PixLite Mk3
> scenes, playlists, playback and intensity from physical switches.

Suggested repository topics:

`advatek`, `pixlite`, `esp32`, `esp32-s3`, `arduino`, `w5500`, `ethernet`,
`poe`, `contact-closure`, `show-control`

## GitHub publication checklist

Before making the repository public:

1. Publish the private staging repository as
   `AdvatekLabs/PixLite-Mk3-Contact-Trigger`.
2. Use `main` as the protected default/release branch, `dev` as the protected
   integration branch, and require the CI workflow for pull requests.
3. Enable Issues and the included structured bug, hardware-test, and feature
   templates.
4. Enable private vulnerability reporting so
   [SECURITY.md](SECURITY.md) has a confidential intake path.
5. Confirm Actions may publish release assets with the scoped
   `contents: write` permission used by the release workflow.
6. Review `HARDWARE-TESTS.md` and `CHANGELOG.md` immediately before publishing.
7. Publish the first artifact as a clearly labelled beta, not a hardware-ready
   stable release.
8. Verify the release zip opens as a same-named Arduino folder and compile it
   once from a clean Arduino IDE installation.

## Project status

**Community beta—bench bring-up passed; full hardware acceptance pending.**

The modular firmware and per-board Arduino sketches compile with Arduino-ESP32
3.3.10 and meet the current flash, static-RAM, and embedded-interface budgets.
A real Waveshare ESP32-S3-ETH and PixLite A4-S Mk3 have passed the initial
bench flow. The Waveshare ESP32-S3-(POE)-ETH-8DI-8RO profile is
compile-supported and awaits ordered-hardware validation. The project must not
be described as hardware-ready until every remaining gate in
[HARDWARE-TESTS.md](HARDWARE-TESTS.md) is complete.

## What it does

- Supports up to eight direct GPIO contacts or eight onboard-isolated
  industrial DI terminals, depending on the selected board artifact.
- Supports normally-open and normally-closed contacts.
- Presents inputs as an add/remove list instead of showing eight unused
  channels; newly added contacts default to 100 ms debounce.
- Provides Momentary Press/Release and Maintained Latch On/Latch Off behavior.
- Triggers PixLite Mk3 scenes and playlists once or continuously.
- Steps forward or backward through each PixLite Mk3's cached scene list with
  wraparound.
- Stops playback, blanks outputs, sets or releases intensity overrides, and
  provides tap/hold brighter and darker controls.
- Uses latest-event-wins arbitration so a recent physical action supersedes
  stale queued playback.
- Discovers PixLite Mk3 controllers over the local network using ADAR v1.1.
- Saves up to 16 PixLite Mk3 controllers by stable ID and MAC address so DHCP
  address changes can be rediscovered.
- Supports explicit Ethernet or Wi-Fi Station operation with DHCP or static
  IPv4. It never silently changes uplink type.
- Provides Ethernet-first commissioning, operational Wi-Fi Station, a compact
  local web UI, diagnostics, configuration backup/import, and a time-limited
  selectable BOOT recovery over Wi-Fi or direct Ethernet.
- Stores configuration in two CRC-checked NVS slots.
- Keeps portable behavior separate from board-specific GPIO assignments.

### Current boundaries

Version 1 can target up to 16 PixLite Mk3 controllers from its GPIO actions.
PixLite Mk3 HTTP work is serialized and it does not provide synchronized
multi-controller playback. It also does not support PixLite Mk2, scene upload,
PixLite Mk3 network reconfiguration, OTA, cloud control, MQTT, cameras, or TF
cards.

The web interface is local HTTP, not TLS. Deploy it only on a trusted local
network or an appropriately isolated control VLAN.

## Supported hardware

| Component | Supported configuration |
| --- | --- |
| Board artifact | Supported products | Inputs |
| --- | --- | --- |
| `AdvatekTrigger-Waveshare-ESP32-S3-ETH` | ESP32-S3-ETH and ESP32-S3-POE-ETH development boards | GPIO1, 2, 15, 16, 18, 38, 39 and 40 |
| `AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO` | ESP32-S3-ETH-8DI-8RO and ESP32-S3-POE-ETH-8DI-8RO industrial boards | Isolated terminals DI1–DI8 |

Both profiles require 16 MB flash, 8 MB OPI PSRAM, onboard W5500 Ethernet, and
Arduino-ESP32 3.3.10. PixLite Mk3 API versions v1.0–v1.9 are supported.

GPIO33–37 are internally used by octal PSRAM and are unavailable. Camera and
TF-card operation are outside the project scope.

The selected contact pins remain reachable with the PoE daughterboard fitted.
GPIO17 and GPIO47 are intentionally excluded because the module obstructs
practical field wiring. The onboard LED remains Advatek orange in operation and
briefly flashes white for each debounced contact edge.
Its enabled state and 1–100% brightness are persistent web settings; disabling
the LED also suppresses trigger flashes.

Stop Playback returns the controller to live mode. This is intentional:
PixLite Mk3's `modeCtrl/Stop` does not stop continuous (`cnt: 0`) playback.

Read [WIRING.md](WIRING.md) before connecting switches or relay contacts.

## Getting started

### What you need

- A Waveshare ESP32-S3-ETH board.
- The optional Waveshare PoE module or a suitable USB-C power supply.
- A data-capable USB-C cable for the initial flash.
- A PixLite Mk3 on the same local network.
- Arduino IDE with internet access for installing the ESP32 board package.
- Volt-free push buttons, switches, or relay contacts.

Do not connect external voltage to a contact input.

### 1. Install Arduino support

1. Install the current Arduino IDE.
2. Open **Tools → Board → Boards Manager**.
3. Search for `esp32` by Espressif Systems.
4. Install version **3.3.10**.

### 2. Download the firmware

For an end-user release, use the prominent **Download latest Arduino build**
link in the repository README. It resolves to the newest release without
requiring a user to browse source folders. Download one of:

- `AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip`—recommended; extract it and open
  the contained sketch.
- `AdvatekTrigger-Waveshare-ESP32-S3-ETH.ino`—place it inside a folder with the
  same base name before opening it in Arduino IDE.
- `AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip`—for both standard and
  PoE industrial 8DI/8RO boards.

The repository copies are under [`generated/`](generated/).

The release zip also contains a self-contained `START-HERE.md` and the
project-specific Waveshare pinout. The fuller
[hardware guide](docs/HARDWARE.md) provides sourcing links and the
medium-protection isolated-input design.

### 3. Configure Arduino IDE

Select **ESP32S3 Dev Module**, then use:

| Arduino IDE option | Value |
| --- | --- |
| Flash Size | `16MB` |
| Partition Scheme | `Huge APP (3MB No OTA/1MB SPIFFS)` |
| PSRAM | `OPI PSRAM` |
| USB Mode | `Hardware CDC and JTAG` |
| USB CDC On Boot | `Enabled` |

Leave unsupported camera and TF-card peripherals disconnected.

### 4. Flash the board

1. Connect the board directly to the computer over USB-C.
2. Select its serial port in Arduino IDE.
3. Click **Upload**.
4. If automatic download mode does not start, hold **BOOT**, briefly press
   reset, start the upload, then release **BOOT** when writing begins.
5. Open Serial Monitor at `115200` baud for startup diagnostics.

### 5. Commission the controller

1. Power the board by USB first. Test PoE only after USB-powered Ethernet has
   been confirmed.
2. Connect the W5500 Ethernet port to the same network as the PixLite Mk3.
3. Open `http://advatrigger.local/` or the DHCP address printed in Serial
   Monitor.
4. Keep Ethernet selected, or configure Wi-Fi Station as the operational
   uplink.
5. Choose DHCP or static addressing.
6. Run PixLite Mk3 discovery, select the controller, and enter operator
   credentials if required.
7. Confirm that scenes and playlists load.
8. Configure each input name, GPIO, contact polarity, debounce, switch mode,
   and Press/Release or Latch On/Latch Off actions.
9. Use the action-test controls before connecting live field switches.
10. Export a redacted configuration backup after commissioning.

When Ethernet or Wi-Fi Station is on the same local network as the computer,
the first-run address is `http://advatrigger.local/`. The Status section also
shows the assigned IPv4 address. Change **Local name** under **Network**
to a unique value before installing multiple triggers; the resulting address
is `http://<name>.local/`.

### 6. Wire the contacts

Connect one side of each dry contact to its assigned GPIO and the other side to
GND. Every enabled input must have a unique permitted GPIO.
Debounce is independently adjustable from 10–2000 ms per input and defaults to
100 ms.

For long cables, outdoor runs, or electrically noisy installations, use
appropriately isolated input hardware and transient protection. Firmware
debounce is not surge protection.

## Recovery

- Hold **BOOT** for 5–14 seconds after startup, then release while the LED
  flashes: clear local authentication and start the selected recovery
  connection for 15 minutes. Direct Ethernet supplies DHCP at
  `192.168.4.1` only when the cable was disconnected during recovery entry.
- Hold **BOOT** for 15–19 seconds, then release while the LED is red: erase
  configuration and restart with factory defaults.
- Keep holding for 20 seconds to cancel the pending recovery/reset action.

Exported backups omit Wi-Fi, PixLite Mk3, recovery, and web-interface passwords.

## Maintainer setup

Prerequisites:

- Node.js 24
- pnpm 10.12.1
- Arduino CLI or Arduino IDE
- Arduino-ESP32 3.3.10
- A C++17 host compiler for native core tests

Install and validate:

```sh
pnpm install --frozen-lockfile
pnpm typecheck
pnpm test
pnpm test:native
pnpm build
pnpm check:generated
```

Compile the canonical firmware:

```sh
arduino-cli compile \
  --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi,USBMode=hwcdc,CDCOnBoot=cdc" \
  firmware/AdvatekTrigger
```

`pnpm build` builds and embeds the Vite interface, then creates both the raw
sketch and its same-named Arduino folder. Generated sketches must not be edited
by hand.

## Deployment and release guide

### Pre-release gate

Before publishing a release:

1. Complete the automated CI build and test suite.
2. Confirm these budgets:
   - Firmware binary no larger than 1.5 MB.
   - Embedded UI no larger than 13 KB gzip.
   - Static DRAM data+BSS no larger than 64 KB.
3. Complete the real-hardware matrix in
   [HARDWARE-TESTS.md](HARDWARE-TESTS.md).
4. Confirm at least 25 PoE-only cold boots.
5. Confirm all eight permitted GPIOs, BOOT recovery, DHCP/static Ethernet,
   operational Wi-Fi Station, ADAR discovery, authenticated PixLite Mk3 connection,
   playback, intensity, backup restoration, and the 24-hour burn-in.
6. Record the Waveshare revision, PoE module, PixLite Mk3 model/firmware, memory
   watermarks, timings, and evidence.
7. Update `compatibility.json`, release notes, and known limitations.

Do not remove the beta warning until these hardware gates pass.

### Create a GitHub release

The release workflow runs for tags matching `v*`.

```sh
git tag -a v1.0.0-beta.6 -m "Advatek Trigger v1.0.0-beta.6"
git push origin v1.0.0-beta.6
```

The workflow:

1. Installs the pinned web build dependencies.
2. Builds the compact Vite interface.
3. Embeds the source commit in the generated sketch.
4. Generates the raw `.ino` and Arduino-folder form.
5. Creates `AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip`.
6. Generates SHA-256 checksums.
7. Publishes the sketch, zip, compatibility manifest, getting-started guide,
   wiring guide, hardware-acceptance record, license, and checksums to GitHub
   Releases.

The workflow can also be run manually from **Actions → Release artifacts →
Run workflow** to inspect artifacts without creating a tagged release.

### Recommended release notes

Each release should state:

- Firmware, UI, configuration-schema, and board-profile versions.
- Supported Arduino-ESP32 version and exact Arduino IDE settings.
- Supported hardware and PixLite Mk3 API range.
- Whether hardware acceptance is complete.
- New features, fixes, migrations, and breaking changes.
- Wiring changes and any GPIOs requiring remapping.
- Known limitations and recovery instructions.
- SHA-256 verification instructions.

### Verify a downloaded artifact

Linux or macOS:

```sh
sha256sum -c SHA256SUMS.txt
```

Windows PowerShell:

```powershell
Get-FileHash .\AdvatekTrigger-Waveshare-ESP32-S3-ETH.ino -Algorithm SHA256
```

Compare the reported hash with the corresponding entry in
`SHA256SUMS.txt`.

## Repository structure

```text
firmware/AdvatekTrigger/
  src/core/       portable behavior, protocols, configuration and validation
  src/platform/   ESP32 tasks, storage, networking, HTTP and diagnostics
  src/boards/     board profiles and hardware initialization
  src/web/        generated compressed interface asset
web/              Vite/TypeScript interface source
boards/           board support manifest
generated/        raw and Arduino-folder release sketches
tests/            host, UI and repository contract tests
tools/            deterministic web embedding and sketch generation
```

See [PORTING.md](PORTING.md) before adding another ESP32 PCB. New boards should
normally be contributed as profiles rather than maintained as firmware forks.

## Documentation

- [README.md](README.md)—concise repository landing page
- [Getting started](docs/GETTING-STARTED.md)—Arduino upload and commissioning
- [WIRING.md](WIRING.md)—contact and reserved-pin wiring
- [Protected contact inputs](docs/PROTECTED-CONTACT-INPUTS.md)—stripboard and
  isolation options
- [Local API](docs/API.md)—ESP32 HTTP API
- [Architecture](docs/ARCHITECTURE.md)—source layers and runtime ownership
- [CONTRIBUTING.md](CONTRIBUTING.md)—community development workflow
- [PORTING.md](PORTING.md)—adding another ESP32 board
- [HARDWARE-TESTS.md](HARDWARE-TESTS.md)—release acceptance evidence
- [SUPPORT.md](SUPPORT.md)—reproducible issue reports
- [SECURITY.md](SECURITY.md)—deployment and vulnerability reporting
- [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)—attributions

## License and contributions

The project is licensed under the MIT License. Contributions should follow
[CONTRIBUTING.md](CONTRIBUTING.md), preserve the modular architecture, avoid
secrets in fixtures or backups, include appropriate tests and documentation,
and keep generated artifacts in sync.

The PixLite Mk3 API and ADAR protocol documents are not redistributed by this
repository.
