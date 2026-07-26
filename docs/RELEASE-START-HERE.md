# Start here: flash the Waveshare ESP32-S3-ETH

This folder is the ready-to-open Arduino build for the Advatek Labs PixLite
Contact Closure Trigger. Do not paste or modify the firmware source.

## Upload in six steps

1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. In **Tools → Board → Boards Manager**, install `esp32` by Espressif Systems,
   version **3.3.10**.
3. Double-click `AdvatekTrigger-Waveshare-ESP32-S3-ETH.ino`.
4. Select **Tools → Board → esp32 → ESP32S3 Dev Module**.
5. Set the exact options below and select the ESP32's COM/serial port.
6. Connect the board over USB-C and click **Upload**.

| Arduino IDE option | Required value |
| --- | --- |
| Flash Size | `16MB` |
| Partition Scheme | `Huge APP (3MB No OTA/1MB SPIFFS)` |
| PSRAM | `OPI PSRAM` |
| USB Mode | `Hardware CDC and JTAG` |
| USB CDC On Boot | `Enabled` |

If upload does not begin, hold **BOOT**, briefly press **RESET**, start upload,
then release **BOOT** when Arduino IDE begins writing.

## First connection

Connect Ethernet to the same local network as the PixLite, then try:

1. `http://advatrigger.local/`
2. The numeric address printed in Arduino Serial Monitor at `115200` baud.
3. Join Wi-Fi `Advatek-Trigger-XXXXXX` and open `http://192.168.4.1`.

The web interface guides PixLite discovery and adding the first contact.

## Contact wiring

Use only GPIO1, GPIO2, GPIO15, GPIO16, GPIO18, GPIO38, GPIO39, or GPIO40.
Connect a genuinely volt-free switch between the selected GPIO and a nearby
GND. Never apply external voltage to a GPIO. This bare connection is for bench
testing or a button inside the same enclosure; installed event buttons should
use the protected or group-isolated input design published with the release.

![Waveshare project pinout](waveshare-esp32-s3-eth-project-pinout.svg)

The status LED is orange while running and flashes white on each accepted
contact edge. Every new input defaults to 100 ms debounce, adjustable in the
web interface.

This release remains a community beta. Bench-test the complete installation
before relying on it in a live environment.
