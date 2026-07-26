# PixLite Contact Trigger guides

Configure physical buttons, switches, and relay contacts to control PixLite Mk3
scenes, playlists, output modes, and intensity.

> **Advatek Labs community beta:** review the hardware acceptance record before
> using this project in a production installation.

## Start here

Choose the path that matches what you are doing:

| I want to… | Guide |
| --- | --- |
| Flash a new ESP32 from Arduino IDE | [Illustrated flashing guide](FLASHING-WITH-ARDUINO.md) |
| Commission the Pico-header development board | [ESP32-S3-ETH getting started](GETTING-STARTED.md) |
| Commission the industrial isolated-input board | [Industrial 8DI getting started](GETTING-STARTED-8DI-8RO.md) |
| Operate hardware that is already installed | [Software user guide](USER-GUIDE.md) |
| Choose and connect contact-input hardware | [Hardware guide](HARDWARE.md) |

## Supported hardware

The project provides separate Arduino downloads for:

- **Waveshare ESP32-S3-ETH / ESP32-S3-POE-ETH** development boards, using
  approved GPIO-to-GND dry-contact inputs.
- **Waveshare ESP32-S3-(POE)-ETH-8DI-8RO** industrial boards, using the
  built-in isolated `DI1`–`DI8` terminals.

PoE changes how the board is powered, not which firmware download it uses.

## Keep a copy

Every guide includes:

- **Print / Save PDF** for a clean printable or PDF version.
- **Download Markdown** for an editable, version-controlled source copy.

The GitHub repository remains the source of truth for firmware, release
downloads, hardware status, and documentation.
