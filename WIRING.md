# Waveshare ESP32-S3 wiring

## Choose the board-specific input

| Board | Where switches connect |
| --- | --- |
| Waveshare ESP32-S3-(POE)-ETH Pico-header board | Approved GPIO and nearby GND, subject to the protection guidance below |
| Waveshare ESP32-S3-(POE)-ETH-8DI-8RO | Built-in isolated DI1–DI8 terminal group; follow the Waveshare passive/active wiring diagram |

The industrial board maps DI1–DI8 internally to GPIO4–GPIO11. Do not connect
its field switches to ESP32 header ground. Its relays, RS485, buzzer, and
TF card are unused by the current firmware.

That unused TF card is on the ESP32 trigger board. PixLite Mk3 SHOWTime
playback separately requires a suitable industrial-grade microSD installed in
the PixLite Mk3 itself.

Third-party hardware references are compatibility examples rather than
Advatek endorsements. Integrators must follow local electrical codes,
applicable standards, manufacturer instructions, and requirements for
qualified electrical personnel.

## Pico-header development board

The inputs are for **volt-free/dry contacts only**.

For a board-orientated drawing and a first-build parts list, start with the
[hardware and project pinout guide](docs/HARDWARE.md).

![Waveshare ESP32-S3-ETH project pinout](docs/waveshare-esp32-s3-eth-project-pinout.svg)

Connect one side of each push button, relay contact, or maintained switch to an
assigned GPIO and the other side to a board GND. Firmware enables the ESP32
internal pull-up. Do not apply an external voltage to an input.

Each input has an independently adjustable 10–2000 ms debounce setting in the
web interface. The default is 100 ms.

Permitted input GPIOs:

| Logical input | Default GPIO | Other permitted GPIOs |
| --- | ---: | --- |
| 1–8 | 1, 2, 15, 16, 18, 38, 39, 40 | Any unused value in the same list |

Every enabled input must have a unique GPIO. The interface rejects duplicate or
reserved assignments.

Reserved hardware pins:

| Function | GPIO |
| --- | ---: |
| W5500 reset | 9 |
| W5500 interrupt | 10 |
| W5500 MOSI | 11 |
| W5500 MISO | 12 |
| W5500 clock | 13 |
| W5500 chip select | 14 |
| BOOT / recovery | 0 |
| Status RGB LED | 21 |

GPIO33–37 are internally occupied by octal PSRAM and must never be used.
GPIO4–7 are reserved for the TF slot; GPIO19/20 for USB; GPIO43/44 for UART;
and GPIO3/45/46 are strapping pins.

GPIO1/2/15/18/38/39/40 are also wired to the camera connector. **Do not connect a
camera while any of these pins is assigned to a contact input.** The camera and
TF-card features are outside this firmware.

The optional Waveshare PoE daughterboard supplies power only. It requires no
GPIO and no different firmware. The input allow-list deliberately uses the
outer header positions that remain accessible with the module fitted; GPIO17
and GPIO47 are excluded because the module obstructs practical field wiring.

The onboard RGB LED is steady Advatek orange while firmware is running. Every
debounced contact edge flashes it white for 120 ms before it returns to orange.

The Stop Playback action explicitly returns the PixLite Mk3 to live mode.
PixLite Mk3's
`modeCtrl/Stop` operation is not used because it does nothing when a loop is a
continuous playback mode.

Also keep USB/serial, strapping, and onboard-device pins unavailable. The
profile allow-list—not a broad “all GPIOs” list—is the source of truth.

For long cables or electrically noisy installations, use appropriate isolated
contact-input hardware and transient protection. The built-in debounce handles
mechanical bounce; it is not electrical surge protection.

For a PoE-powered installation, the repository includes both a buildable
non-isolated protected stripboard design and a group-isolated optocoupler
approach for field cabling. See
[Protected and isolated contact-input options](docs/PROTECTED-CONTACT-INPUTS.md).
