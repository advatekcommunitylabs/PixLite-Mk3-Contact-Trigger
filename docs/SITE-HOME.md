# PixLite Mk3 Contact Trigger guides

Configure physical buttons, switches, and relay contacts to control PixLite Mk3
scenes, playlists, output modes, and intensity.

> **Advatek Community Labs beta:** follow the board-specific guide and evaluate
> the installation for its intended environment before use.

Advatek Technical Support does not cover this third-party hardware or Advatek
Labs community projects. Use the repository's GitHub Issues for reproducible
community support requests.

## Start here

Choose the path that matches what you are doing:

| I want to… | Guide |
| --- | --- |
| Flash a new ESP32 from Arduino IDE | [Illustrated flashing guide](FLASHING-WITH-ARDUINO.md) |
| Commission the Waveshare ESP32-S3-ETH / ESP32-S3-POE-ETH development board | [ESP32-S3-ETH getting started](GETTING-STARTED.md) |
| Commission the industrial isolated-input board | [Industrial 8DI getting started](GETTING-STARTED-8DI-8RO.md) |
| Operate hardware that is already installed | [Software user guide](USER-GUIDE.md) |
| Choose and connect contact-input hardware | [Hardware guide](HARDWARE.md) |

## Supported hardware

The project provides separate Arduino downloads for:

- **Waveshare ESP32-S3-ETH / ESP32-S3-POE-ETH** development boards, using
  approved GPIO-to-GND dry-contact inputs.
- **Waveshare ESP32-S3-ETH-8DI-8RO / ESP32-S3-POE-ETH-8DI-8RO** industrial
  boards, using the
  built-in isolated `DI1`–`DI8` terminals.

PoE changes only the power source. Each standard-Ethernet and PoE pair uses the
same firmware download.

PixLite Mk3 SHOWTime media playback requires an industrial-grade microSD
installed in the PixLite Mk3. The ESP32 boards' own TF/microSD slots are not
used by this firmware.

Third-party hardware references document compatibility. Advatek does not
endorse the listed products. Integrators must follow local electrical codes, applicable
standards, manufacturer instructions, and requirements for qualified
electrical work.

## Keep a copy

Every guide includes:

- **Print / Save PDF** for a clean printable or PDF version.
- **Download Markdown** for an editable, version-controlled source copy.

The GitHub repository remains the source of truth for firmware, release
downloads, hardware status, and documentation.
