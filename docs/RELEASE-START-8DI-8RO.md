# Start here: flash the Waveshare industrial 8DI board

This Arduino folder supports both:

- `ESP32-S3-ETH-8DI-8RO`
- `ESP32-S3-POE-ETH-8DI-8RO`

PoE changes only the power path. Do not open the similarly named
`AdvatekTrigger-Waveshare-ESP32-S3-ETH.ino`; that file is for the smaller Pico
header development board and uses different pins.

## Upload

1. Install Arduino IDE.
2. Install `esp32` by Espressif Systems version **3.3.10**.
3. Open `AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.ino`.
4. Select **ESP32S3 Dev Module** and the board's COM/serial port.
5. Set the options below and click **Upload**.

| Arduino IDE option | Required value |
| --- | --- |
| Flash Size | `16MB` |
| Partition Scheme | `Huge APP (3MB No OTA/1MB SPIFFS)` |
| PSRAM | `OPI PSRAM` |
| USB Mode | `Hardware CDC and JTAG` |
| USB CDC On Boot | `Enabled` |

## Connect

Connect Ethernet to the same network as the computer, then open
`http://advatrigger.local/` or the DHCP address shown at `115200` baud. Normal
commissioning does not create a Wi-Fi setup access point.

For a passive button or dry contact, leave input `COM` unused and wire the
contact between `DGND` and its chosen `DI1`–`DI8` terminal. Remove power before
changing terminal wiring.

BOOT recovery can temporarily provide Wi-Fi or direct-Ethernet access at
`http://192.168.4.1/`; follow the full guide before using recovery mode.

The input selector shows `DI1`–`DI8`. Use the isolated input screw terminals,
not ESP32 header pins. Relays, RS485, buzzer, and TF card are intentionally
unused by this release.

The eight inputs have passed real-board electrical sweeps. This target remains
a community beta until the longer-duration acceptance gates in
`HARDWARE-TESTS.md` are complete.
