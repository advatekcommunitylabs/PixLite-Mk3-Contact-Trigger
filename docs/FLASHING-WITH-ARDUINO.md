# Flashing with Arduino IDE

This illustrated guide installs the Advatek Labs community beta on either
supported Waveshare ESP32-S3 board. You do not need to edit or paste any code:
download the folder for your hardware, open its `.ino` file, select the options
shown below, and choose **Upload**.

> **Choose the exact hardware download.** The two boards use different
> Ethernet and input pins. The wrong build may compile successfully and fail
> when uploaded to the hardware.

## Before you start

You need:

- Arduino IDE 2.x;
- a data-capable USB-C cable;
- one supported Waveshare board;
- the correct Arduino ZIP from the `v1.0.0-beta.6` release.

| Your hardware | Download |
| --- | --- |
| Waveshare ESP32-S3-ETH or ESP32-S3-POE-ETH development board | [`AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip`](https://github.com/AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip) |
| Waveshare ESP32-S3-ETH-8DI-8RO or ESP32-S3-POE-ETH-8DI-8RO industrial board | [`AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip`](https://github.com/AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.zip) |

PoE changes only the power source. Each standard-Ethernet and PoE pair uses the
same firmware ZIP.
The screenshots below use the Waveshare ESP32-S3-ETH development-board
download. The
industrial download has a longer filename, but the Arduino settings and upload
controls are the same.

## 1. Install the ESP32 board package

1. Open Arduino IDE.
2. Select the **Boards Manager** icon in the left sidebar.
3. Search for `esp32`.
4. Find **esp32 by Espressif Systems**.
5. Select and install version **3.3.10**.

Do not install the similarly named **Arduino ESP32 Boards by Arduino** package
for this project. If Arduino offers a newer Espressif version, use 3.3.10 until
the compatibility manifest is updated.

![Arduino Boards Manager showing esp32 by Espressif Systems 3.3.10 installed](assets/arduino-flash-guide/00-install-esp32-core-annotated.svg)

## 2. Extract and open the sketch

1. Extract the downloaded ZIP.
2. Open the extracted, same-named folder.
3. Double-click the `.ino` file inside it.

The folder and `.ino` must keep the same name. Arduino IDE should open one
large generated sketch like the screenshot below. This is intentional: the
readable modular source is maintained in the repository, while the generated
file makes releases easy to flash.

On Windows, extract near the top of a normal user folder, such as
`Documents\AdvatekTrigger`. Arduino's toolchain can fail without a useful error
when the industrial board's longer filename is placed inside an excessively
long path.

![Generated Advatek Trigger sketch open in Arduino IDE](assets/arduino-flash-guide/01-open-sketch-annotated.svg)

## 3. Connect the board and select its port

Connect USB-C directly to the ESP32-S3 board. In the board selector:

1. Select **ESP32S3 Dev Module**.
2. Select the serial port that appeared when the board was connected.

Windows normally calls the port `COM3`, `COM4`, or similar. macOS normally
shows a `/dev/cu.usbmodem...` name. The number or name will differ from the
screenshot. Ignore an unavailable port remembered from an earlier board; if
unsure, disconnect and reconnect USB and select the newly appearing port.

Do **not** select `Waveshare ESP32-S3-Zero`; it is a different board.

![Arduino board selector showing ESP32S3 Dev Module and a connected COM port](assets/arduino-flash-guide/02-select-board-and-port-annotated.svg)

For the industrial **ESP32-S3-POE-ETH-8DI-8RO**, disconnect PoE for the first
upload and use only a data-capable USB-C cable to the computer. The
[industrial PoE connection guide](GETTING-STARTED-8DI-8RO.md#identify-the-poe-board-connections)
shows the exact USB-C, PoE, BOOT, RESET, and isolated-input locations using
official Waveshare product images.

## 4. Set the required build options

Open **Tools** and set these values exactly:

| Arduino IDE option | Required value |
| --- | --- |
| Board | `ESP32S3 Dev Module` |
| Flash Size | `16MB (128Mb)` |
| Partition Scheme | `Huge APP (3MB No OTA/1MB SPIFFS)` |
| PSRAM | `OPI PSRAM` |
| USB Mode | `Hardware CDC and JTAG` |
| USB CDC On Boot | `Enabled` |
| Erase All Flash Before Sketch Upload | `Disabled` |

Leave the other options at their defaults. Arduino IDE remembers many options
per board, but changing board definitions can reset Flash Size, Partition
Scheme, or PSRAM. Recheck this table before every first upload to a new device.

![Arduino Tools menu showing the required ESP32-S3 flash, partition, PSRAM and USB settings](assets/arduino-flash-guide/03-tools-settings-annotated.svg)

## 5. Compile and upload

Select the right-arrow **Upload** button in the top-left corner.

The first build can take several minutes while Arduino compiles the ESP32 core
and indexes the generated convenience sketch. Wait until the IDE reports
**Done uploading**, or the output ends with **Hash of data verified** and
**Hard resetting via RTS pin**, before unplugging the board.

Immediately after installing or changing the ESP32 package, allow Arduino IDE
to finish indexing before judging the automatically displayed board name.
Your manual selection must remain **ESP32S3 Dev Module**.

![Arduino IDE compiling the Advatek Trigger sketch after Upload is selected](assets/arduino-flash-guide/04-upload-firmware-annotated.svg)

![Arduino IDE output showing the firmware write reached 100 percent, its hash was verified, and the board was reset](assets/arduino-flash-guide/05-upload-complete-annotated.svg)

If Arduino cannot put the board into download mode automatically:

1. Hold the board's **BOOT** button.
2. Briefly press and release **RESET**.
3. Select **Upload**.
4. Release **BOOT** when Arduino begins writing.

This does not erase saved configuration unless **Erase All Flash Before Sketch
Upload** was deliberately enabled.

## 6. Check the first boot

Open **Tools → Serial Monitor** and select `115200 baud`. A healthy boot reports:

- firmware `1.0.0-beta.6`;
- the correct Waveshare board profile;
- 16 MB flash;
- usable 8 MB PSRAM;
- W5500 Ethernet initialization;
- an Ethernet address supplied by DHCP.

Do not continue if the log reports the wrong board profile or degraded PSRAM.

![Serial Monitor boot lines confirming beta.6, the Waveshare ESP32-S3-ETH profile, 16 MB flash and normal 8 MB PSRAM](assets/arduino-flash-guide/06-first-boot-annotated.svg)

Connect Ethernet to the same local network as the computer and PixLite Mk3, then
open:

```text
http://advatrigger.local/
```

If `.local` name resolution is unavailable, use the numeric Ethernet address
shown in Serial Monitor. The web interface also displays the current address
and lets you assign a unique local name.

## Common upload problems

| Symptom | What to check |
| --- | --- |
| No serial port appears | Use a data-capable USB cable, another USB port, and reconnect the board |
| `Sketch too big` | Select 16 MB Flash and the Huge APP partition |
| Device starts in degraded mode | Select OPI PSRAM and upload again |
| Upload waits for a connection | Use the BOOT/RESET sequence above |
| Upload succeeds but Ethernet fails | Confirm the downloaded ZIP matches the physical board |
| Industrial sketch ends with only `Error during build: exit status 1` | Move the extracted folder to a shorter path such as `Documents\AdvatekTrigger` and compile again |
| Browser cannot open `.local` | Use the numeric DHCP address from Serial Monitor |

After the web interface opens, continue with the
[Waveshare ESP32-S3-ETH development-board guide](GETTING-STARTED.md) or the
[industrial 8DI guide](GETTING-STARTED-8DI-8RO.md).
