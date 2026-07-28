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

On Windows, keep the extracted folder in a short path such as
`Documents\AdvatekTrigger`. If Arduino ends with only
`Error during build: exit status 1`, shorten the folder path and compile again.

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

PixLite Mk3 SHOWTime scene and playlist playback requires an industrial-grade
microSD installed in the PixLite Mk3. The ESP32 board's own TF/microSD slot is
unrelated and remains unused.

For a passive button or dry contact, leave input `COM` unused and wire the
contact between `DGND` and its chosen `DI1`–`DI8` terminal. Remove power before
changing terminal wiring.

BOOT recovery can temporarily provide Wi-Fi or direct-Ethernet access at
`http://192.168.4.1/`. Wi-Fi recovery should open setup automatically on a
phone; otherwise use that address in a full browser. Direct Ethernet restarts
once; keep the normal LAN
unplugged until the status LED pulses cyan, then connect one computer directly.
Follow the full guide before using recovery mode. Power-cycle the enclosed
controller to leave direct-Ethernet recovery after reconnecting normal LAN.

The input selector shows `DI1`–`DI8`. Use the isolated input screw terminals,
not ESP32 header pins. Relays, RS485, buzzer, and TF card are intentionally
unused by this release.

This target is an Advatek Labs community beta. Third-party hardware references
document compatibility. Advatek does not endorse the listed products. Follow local electrical
codes, applicable standards, and manufacturer instructions. Hardware-affecting
modifications made after public launch are recorded in `HARDWARE-TESTS.md`.
