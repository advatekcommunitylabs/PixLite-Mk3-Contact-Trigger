# Getting started: Waveshare industrial 8DI board

This guide covers both the **Waveshare ESP32-S3-ETH-8DI-8RO** and
**ESP32-S3-POE-ETH-8DI-8RO**. PoE changes the power hardware only; both models
use the same Advatek Trigger firmware download.

The target is compile-supported but remains hardware-validation-pending until
the ordered boards complete the checklist in
[HARDWARE-TESTS.md](../HARDWARE-TESTS.md).

First-time Arduino users should follow the
[illustrated flashing guide](FLASHING-WITH-ARDUINO.md). It shows the shared
ESP32-S3 board settings and upload process with real Arduino IDE screenshots.

## Download the correct build

Download
[AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip](https://github.com/AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip),
extract it, and open the same-named `.ino` file. Do not use the
`ESP32-S3-ETH` sketch: its Ethernet and input pins are different.

If no release exists yet, use the
[generated repository sketch](../generated/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.ino).

## Arduino IDE settings

Install `esp32` by Espressif Systems version **3.3.10**, select
**ESP32S3 Dev Module**, and use:

| Option | Required value |
| --- | --- |
| Flash Size | `16MB` |
| Partition Scheme | `Huge APP (3MB No OTA/1MB SPIFFS)` |
| PSRAM | `OPI PSRAM` |
| USB Mode | `Hardware CDC and JTAG` |
| USB CDC On Boot | `Enabled` |

Connect USB-C, select its COM/serial port, and choose **Upload**. If automatic
download mode fails, hold **BOOT**, briefly press **RESET**, begin the upload,
then release **BOOT** when writing starts.

## Power and Ethernet

- The `ESP32-S3-POE-ETH-8DI-8RO` accepts IEEE 802.3af PoE through its network
  port.
- The non-PoE `ESP32-S3-ETH-8DI-8RO` needs USB-C or its documented 7–36 V DC
  supply. An ordinary Ethernet cable does not power it.
- Use USB for the first flash and diagnostic check, even when commissioning the
  PoE version.

At a healthy boot, Serial Monitor at `115200` reports 16 MB flash, 8 MB PSRAM,
W5500 initialization, and the DHCP address. Open `http://advatrigger.local/`,
or the displayed numeric address. Ethernet is the commissioning connection;
Wi-Fi Station can be selected later as the operational uplink.

The SPA also selects the BOOT recovery connection. For direct-Ethernet
recovery, unplug the installed network first, hold BOOT for 5–14 seconds,
release, and only then connect one computer directly. Open
`http://192.168.4.1/`; recovery expires after 15 minutes.

## Isolated input terminals

This board maps its eight isolated field inputs as follows:

| Enclosure terminal | Firmware GPIO |
| --- | --- |
| DI1 | GPIO4 |
| DI2 | GPIO5 |
| DI3 | GPIO6 |
| DI4 | GPIO7 |
| DI5 | GPIO8 |
| DI6 | GPIO9 |
| DI7 | GPIO10 |
| DI8 | GPIO11 |

![Industrial board input mapping](waveshare-esp32-s3-eth-8di-8ro-inputs.svg)

Use the input screw terminals and Waveshare's passive/active input diagram.
For a dry contact, wire only within the isolated input terminal group; do not
route the switch to an ESP32 GPIO header or ESP32 ground. The hardware also
supports documented 5–36 V active inputs, but verify the Waveshare polarity
diagram and terminal labels before applying voltage.

The web interface presents `DI1` through `DI8`; GPIO numbers remain visible for
diagnostics and backup portability. Each newly added input defaults to 100 ms
debounce.

## Unused onboard hardware

The current firmware deliberately leaves the eight relays, RS485, buzzer, and
TF-card slot inactive. No extra Arduino libraries are required.

## First acceptance checks

1. Confirm the boot banner names `waveshare-esp32-s3-eth-8di-8ro`.
2. Confirm 16 MB flash, 8 MB PSRAM, Ethernet link, and DHCP.
3. Verify the orange RGB LED on GPIO38 and BOOT recovery on GPIO0.
4. Add one PixLite and confirm its media list.
5. Configure DI1 with 100 ms debounce and a harmless test action.
6. Operate DI1 as a passive dry contact and confirm one LED flash per edge.
7. Repeat for DI2–DI8 before treating the profile as hardware-ready.

Do not connect loads to the relay terminals for this bring-up.
