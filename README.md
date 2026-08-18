# PixLite Mk3 Contact Trigger

An open-source community beta for turning physical push buttons, maintained
switches and relay contacts into PixLite Mk3 scene, playlist, intensity and
test mode actions. The ESP32 controller is configured through a compact local
web interface; no firmware editing is required after the initial Arduino
upload.

## Start here

> [!TIP]
> **New users should begin with the
> [step-by-step guide site](https://advatekcommunitylabs.github.io/PixLite-Mk3-Contact-Trigger/).**
> It helps you choose the correct hardware download, flash the ESP32, connect
> switches, and configure the controller. Every guide can also be printed,
> saved as a PDF, or downloaded as Markdown.

Already have a flashed controller? Open the
[software user guide](https://advatekcommunitylabs.github.io/PixLite-Mk3-Contact-Trigger/user-guide.html).
Maintainers and contributors can continue to the
[repository documentation](#documentation).

> **Community beta:** use the matching board-specific download and follow the
> installation guide before connecting field hardware. Advatek Technical
> Support does not cover third-party hardware or Advatek Community Labs
> projects.

## Quick start

Choose the download that exactly matches the Waveshare hardware:

| Hardware | Matching Arduino download | Input connection |
| --- | --- | --- |
| Waveshare ESP32-S3-ETH / ESP32-S3-POE-ETH development board | [ESP32-S3-ETH zip](https://github.com/advatekcommunitylabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip) | Approved GPIO to GND; add suitable protection when wiring leaves the enclosure |
| Waveshare ESP32-S3-ETH-8DI-8RO / ESP32-S3-POE-ETH-8DI-8RO industrial board | [Industrial 8DI zip](https://github.com/advatekcommunitylabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip) | Built-in isolated DI1–DI8 screw terminals |

PoE changes only the power path. The standard-Ethernet and PoE variants of each
physical board share one firmware pin map and download.

The zip is the simplest route for non-technical users: extract it, open the
same-named folder, and double-click its `.ino`. No source files need to be
copied or edited. Repository builds are also available under
[`generated/`](generated/).

### PixLite Mk3 SHOWTime prerequisite

Scene and playlist playback uses PixLite Mk3 SHOWTime. A suitable
**industrial-grade microSD card must be installed in the PixLite Mk3** before
SHOWTime media can be stored or played. Follow the
[official SHOWTime guidance](https://www.advateklighting.com/en-us/software/showtime)
for card preparation. The microSD/TF slot on either ESP32 trigger board is
unrelated and is not used by this firmware.

New to Arduino? Follow the
[illustrated flashing guide](docs/FLASHING-WITH-ARDUINO.md) for screenshots of
every board, port, build-option, and upload step.

1. Install [Arduino IDE](https://www.arduino.cc/en/software) and Espressif
   **Arduino-ESP32 3.3.10**.
2. Extract and open the downloaded Arduino folder.
3. Select **ESP32S3 Dev Module** and configure:

   | Arduino IDE option | Value |
   | --- | --- |
   | Flash Size | `16MB` |
   | Partition Scheme | `Huge APP (3MB No OTA/1MB SPIFFS)` |
   | PSRAM | `OPI PSRAM` |
   | USB Mode | `Hardware CDC and JTAG` |
   | USB CDC On Boot | `Enabled` |

4. Select the board's COM/serial port and click **Upload**.
5. Connect Ethernet and open `http://advatrigger.local/`.
6. If that name is unavailable, use the numeric IP printed at 115200 baud or
   shown at the top of the web interface.

The controller does not automatically fall back from Wi-Fi to Ethernet. If the
configured Wi-Fi network is unavailable, hold **BOOT for 5–14 seconds** and
release to start the recovery method selected in the web interface:

- **Wi-Fi AP:** join `Advatek-Trigger-XXXXXX`. A phone should open the setup
  page automatically; if it does not, use a full browser and open
  `http://192.168.4.1/`.
- **Direct Ethernet:** unplug the installed LAN first, enter BOOT recovery, and
  wait through the solid-white restart indication until the controller pulses
  cyan before connecting one computer directly. Power the controller
  separately over USB-C or its
  supported DC input because a normal computer Ethernet port does not provide
  PoE. Open `http://192.168.4.1/`.

Recovery access expires after 15 minutes.

Continue with the [development-board guide](docs/GETTING-STARTED.md) or
[industrial 8DI guide](docs/GETTING-STARTED-8DI-8RO.md). The
[hardware and pinout guide](docs/HARDWARE.md) describes compatible parts and
exactly where each supported contact is connected.

## What it supports

- Up to eight direct or board-isolated contact inputs with unique hardware
  assignment, normally-open or normally-closed operation, and independently
  adjustable 10–2000 ms debounce.
  New inputs default to 100 ms.
- Momentary Press/Release and maintained Latch On/Latch Off actions.
- Scene and playlist playback Once or Loop Forever, next/previous scene
  stepping with wraparound, Live, Blank, solid colour Test mode, and the
  PixLite Mk3 RGB colour-fade test.
- GPIO-driven Set/Release intensity and press-and-hold Brighter/Darker actions
  for Pixels, Aux, or combined outputs.
- Up to 16 saved PixLite Mk3 controllers, identified by nickname and MAC
  address and selected independently by each GPIO action.
- ADAR v1.1 discovery and PixLite Mk3 API negotiation from v1.0 through v1.9.
- Explicit Ethernet or Wi-Fi Station uplink with DHCP or static IPv4; there is
  no silent fallback between uplinks.
- Ethernet-first commissioning, operational Wi-Fi Station, editable `.local`
  name, optional local password, redacted backup/import, diagnostics, and a
  selectable time-limited BOOT recovery over Wi-Fi or direct Ethernet.
- Persistent orange status LED with configurable brightness and a white pulse
  on each debounced contact edge.

PixLite Mk3 HTTP work is serialized on a separate FreeRTOS task so slow controller
requests cannot block GPIO debounce or release detection. Latest physical event
wins, and actions that cannot be delivered expire after two seconds.

## Electrical safety

On the Waveshare ESP32-S3-ETH / ESP32-S3-POE-ETH development board, ESP32
pins accept **volt-free contacts only**. Never apply external voltage directly
to a GPIO. The industrial 8DI board instead uses its built-in isolated screw
terminals and supports the passive/active arrangements documented by
Waveshare.

For PoE-powered installations and switch cabling outside the enclosure, use
the isolated-input approach described in
[Off-the-shelf isolated contact inputs](docs/PROTECTED-CONTACT-INPUTS.md).
The public guides use complete, documented input modules for field wiring.
Direct GPIO-to-GND wiring is intended for bench tests or buttons inside the
controller enclosure. See the
[hardware decision guide](docs/HARDWARE.md#choose-the-input-approach).

Third-party boards, modules, suppliers, and product links document
compatibility. Advatek does not endorse the listed products. Listings and
product designs can change.
Integrators are responsible for checking local electrical codes, applicable
standards, manufacturer instructions, and whether qualified electrical
personnel are required for their installation.

Permitted contact pins on the Waveshare ESP32-S3-ETH / ESP32-S3-POE-ETH board
layout are:
`GPIO1`, `GPIO2`, `GPIO15`, `GPIO16`, `GPIO18`, `GPIO38`, `GPIO39`, and
`GPIO40`. The firmware allow-list is the source of truth.

The industrial profile maps `DI1`–`DI8` to GPIO4–GPIO11 internally. Installers
connect switches to the labeled DI terminal block.

## Project structure

```text
firmware/AdvatekTrigger/
  src/core/       portable input, protocol, configuration and policy code
  src/platform/   ESP32 networking, NVS, HTTP, tasks and diagnostics
  src/boards/     board profiles and Ethernet initialization
  src/web/        generated gzipped interface asset
web/              editable Vite/TypeScript interface
boards/           machine-readable board support manifest
generated/        Arduino-friendly release sketches
tests/            host, UI and repository contract tests
tools/            deterministic web embedding and sketch generation
```

The modular source is canonical. The large `.ino` is a deterministic,
generated convenience artifact for Arduino IDE users and must not be edited by
hand. New ESP32 boards use profiles and the shared PixLite Mk3 logic.

The earlier experimental v0.8 sketch was an unverified idea reference and is
not part of this implementation or its architectural foundation.

See [Architecture](docs/ARCHITECTURE.md) for the runtime design and
[PORTING.md](PORTING.md) for the board-profile contract.

## Maintainer validation

Prerequisites are Node.js 24, pnpm 10.12.1, a C++17 compiler, Arduino CLI, and
Arduino-ESP32 3.3.10.

```sh
pnpm install --frozen-lockfile
pnpm typecheck
pnpm test
pnpm test:native
pnpm check:docs
pnpm build
pnpm check:generated
```

CI additionally compiles the modular firmware, generated sketch, and hardware
canary and enforces firmware, static-DRAM, and compressed-interface budgets.

## Documentation

The guides below are also published as a responsive
[GitHub Pages documentation site](https://advatekcommunitylabs.github.io/PixLite-Mk3-Contact-Trigger/)
with dark/light themes and per-guide PDF/Markdown export controls.

| Guide | Audience |
| --- | --- |
| [Illustrated Arduino flashing guide](docs/FLASHING-WITH-ARDUINO.md) | Non-technical users installing a release |
| [Getting started](docs/GETTING-STARTED.md) | First-time Arduino and commissioning users |
| [Industrial 8DI getting started](docs/GETTING-STARTED-8DI-8RO.md) | Users of the isolated-input DIN-rail board |
| [Software user guide](docs/USER-GUIDE.md) | Operators using a flashed and commissioned controller |
| [Hardware and project pinout](docs/HARDWARE.md) | First-time buyers and installers |
| [Wiring](WIRING.md) | Installers wiring dry contacts |
| [Protected contact inputs](docs/PROTECTED-CONTACT-INPUTS.md) | Builders adding cable and ground protection |
| [Local HTTP API](docs/API.md) | Integrators and interface developers |
| [Architecture](docs/ARCHITECTURE.md) | Firmware and web contributors |
| [Contributing](CONTRIBUTING.md) | Community contributors |
| [Board porting](PORTING.md) | Maintainers adding an ESP32 PCB |
| [Project and release summary](PROJECT-SUMMARY.md) | Advatek Community Labs repository and release maintainers |
| [Support](SUPPORT.md) | Users reporting a problem |
| [Security](SECURITY.md) | Installers and vulnerability reporters |

## Scope

Version 1 does not support PixLite Mk2, synchronized multi-controller playback,
scene upload, PixLite Mk3 network reconfiguration, OTA, MQTT/cloud control, camera
operation, or TF-card operation. The web interface uses local HTTP without TLS.
Use it only on a trusted LAN or control VLAN.

## Community and license

This is an Advatek Community Labs integration. Advatek Lighting Technical
Support does not cover this third-party hardware or any Advatek Community Labs
project. GitHub Issues are the community support route and are not a substitute
for urgent or show-critical support.

Advatek does not endorse one compatible third-party product or supplier over
another. Community members and the Advatek team are welcome to share tested
profiles, improvements, documentation, and repeatable hardware evidence.

Contributions are welcome under [CONTRIBUTING.md](CONTRIBUTING.md). Please
report security issues privately as described in [SECURITY.md](SECURITY.md).

MIT licensed. See [LICENSE](LICENSE) and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). PixLite Mk3 API and ADAR protocol
documents are not redistributed.
