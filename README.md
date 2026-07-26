# Advatek Labs PixLite Contact Closure Trigger

An open-source ESP32 appliance that turns physical push buttons, maintained
switches, and relay contacts into PixLite Mk3 playback and intensity actions.
Installers configure it through a compact local web interface; no firmware
editing is required after the initial Arduino upload.

> **Community beta:** the Waveshare ESP32-S3-ETH and a PixLite A4-S Mk3 have
> passed initial bench bring-up. The industrial
> ESP32-S3-(POE)-ETH-8DI-8RO target is compile-supported and awaits real-board
> electrical validation. See
> [HARDWARE-TESTS.md](HARDWARE-TESTS.md) before using it in a production
> installation.

## Quick start

Choose the download that exactly matches the Waveshare hardware:

| Hardware | Recommended Arduino download | Input connection |
| --- | --- | --- |
| ESP32-S3-ETH / ESP32-S3-POE-ETH Pico-header development board | [ESP32-S3-ETH zip](https://github.com/AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip) | Approved GPIO to GND; external isolation recommended |
| ESP32-S3-ETH-8DI-8RO / ESP32-S3-POE-ETH-8DI-8RO industrial board | [Industrial 8DI zip](https://github.com/AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip) | Built-in isolated DI1–DI8 screw terminals |

PoE changes the power path, not the firmware pin map, so the standard-Ethernet
and PoE variants of each physical board share a download.

The zip is the recommended route for non-technical users: extract it, open the
same-named folder, and double-click its `.ino`. No source files need to be
copied or edited. Repository builds are also available under
[`generated/`](generated/).

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
6. If that name is unavailable, use the IP printed at 115200 baud or join
   `Advatek-Trigger-XXXXXX` and open `http://192.168.4.1`.

Continue with the [development-board guide](docs/GETTING-STARTED.md) or
[industrial 8DI guide](docs/GETTING-STARTED-8DI-8RO.md). The
[hardware and pinout guide](docs/HARDWARE.md) shows what to buy and exactly
where each supported contact is connected.

## What it supports

- Up to eight direct or board-isolated contact inputs with unique hardware
  assignment, normally-open or normally-closed operation, and independently
  adjustable 10–2000 ms debounce.
  New inputs default to 100 ms.
- Momentary Press/Release and maintained Latch On/Latch Off actions.
- Scene and playlist playback Once or Loop Forever, next/previous scene
  stepping with wraparound, Live, Blank, and solid colour Test mode.
- GPIO-driven Set/Release intensity and press-and-hold Brighter/Darker actions
  for Pixels, Aux, or combined outputs.
- Up to 16 saved PixLite Mk3 controllers, identified by nickname and MAC
  address and selected independently by each GPIO action.
- ADAR v1.1 discovery and PixLite API negotiation from v1.0 through v1.9.
- Explicit Ethernet or Wi-Fi Station uplink with DHCP or static IPv4; there is
  no silent fallback between uplinks.
- Ethernet-first commissioning, operational Wi-Fi Station, editable `.local`
  name, optional local password, redacted backup/import, diagnostics, and a
  selectable time-limited BOOT recovery over Wi-Fi or direct Ethernet.
- Persistent orange status LED with configurable brightness and a white pulse
  on each debounced contact edge.

PixLite HTTP work is serialized on a separate FreeRTOS task so slow controller
requests cannot block GPIO debounce or release detection. Latest physical event
wins, and actions that cannot be delivered expire after two seconds.

## Electrical safety

On the Pico-header development board, ESP32 pins accept **volt-free contacts
only**. Never apply external voltage directly to a GPIO. The industrial 8DI
board instead uses its built-in isolated screw terminals and supports the
passive/active arrangements documented by Waveshare.

For PoE-powered installations and switch cabling outside the enclosure, use
the isolated-input approach described in
[Protected contact inputs](docs/PROTECTED-CONTACT-INPUTS.md). The included
stripboard circuits are prototype references, not certified safety designs.
For a small immersive event, the recommended default is the group-isolated
optocoupler route; direct GPIO-to-GND wiring is reserved for bench tests or
buttons inside the controller enclosure. See the
[hardware decision guide](docs/HARDWARE.md#which-button-connection-should-i-build).

Permitted contact pins on the Pico-header PoE-accessible board layout are:
`GPIO1`, `GPIO2`, `GPIO15`, `GPIO16`, `GPIO18`, `GPIO38`, `GPIO39`, and
`GPIO40`. The firmware allow-list is the source of truth.

The industrial profile maps `DI1`–`DI8` to GPIO4–GPIO11 internally; installers
use the labeled DI terminal block rather than those ESP32 pins.

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
hand. New ESP32 boards are added as profiles rather than forks of the shared
PixLite logic.

The earlier experimental v0.8 sketch was an unverified idea reference and is
not part of this implementation or its architectural foundation.

See [Architecture](docs/ARCHITECTURE.md) for the runtime design and
[PORTING.md](PORTING.md) for the board-profile contract.

## Maintainer validation

Prerequisites are Node.js 22, pnpm 10.12.1, a C++17 compiler, Arduino CLI, and
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

| Guide | Audience |
| --- | --- |
| [Getting started](docs/GETTING-STARTED.md) | First-time Arduino and commissioning users |
| [Industrial 8DI getting started](docs/GETTING-STARTED-8DI-8RO.md) | Users of the isolated-input DIN-rail board |
| [Hardware and project pinout](docs/HARDWARE.md) | First-time buyers and installers |
| [Wiring](WIRING.md) | Installers wiring dry contacts |
| [Protected contact inputs](docs/PROTECTED-CONTACT-INPUTS.md) | Builders adding cable and ground protection |
| [Local HTTP API](docs/API.md) | Integrators and interface developers |
| [Architecture](docs/ARCHITECTURE.md) | Firmware and web contributors |
| [Contributing](CONTRIBUTING.md) | Community contributors |
| [Board porting](PORTING.md) | Maintainers adding an ESP32 PCB |
| [Hardware acceptance](HARDWARE-TESTS.md) | Testers and release maintainers |
| [Project and release summary](PROJECT-SUMMARY.md) | Advatek Labs repository and release maintainers |
| [Support](SUPPORT.md) | Users reporting a problem |
| [Security](SECURITY.md) | Installers and vulnerability reporters |

## Scope

Version 1 does not support PixLite Mk2, synchronized multi-controller playback,
scene upload, PixLite network reconfiguration, OTA, MQTT/cloud control, camera
operation, or TF-card operation. The web interface is local HTTP, not TLS; use
it only on a trusted LAN or control VLAN.

## Community and license

This is an unofficial Advatek Labs community integration. It is not an
Advatek Lighting-supported production product, and community issues are not a
substitute for urgent show-critical support.

Contributions are welcome under [CONTRIBUTING.md](CONTRIBUTING.md) and the
[Code of Conduct](CODE_OF_CONDUCT.md). Please report security issues privately
as described in [SECURITY.md](SECURITY.md).

MIT licensed. See [LICENSE](LICENSE) and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). PixLite API and ADAR protocol
documents are not redistributed.
