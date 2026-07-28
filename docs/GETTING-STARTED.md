# Getting started

This guide takes a new Waveshare ESP32-S3-ETH from an empty board to one tested
dry-contact input. The firmware is a community beta. Follow this guide and
evaluate the completed installation for its intended environment before live
use.

For the Waveshare ESP32-S3-ETH-8DI-8RO or ESP32-S3-POE-ETH-8DI-8RO industrial
board with built-in isolated inputs, use
the separate [industrial 8DI getting-started guide](GETTING-STARTED-8DI-8RO.md)
and its board-specific Arduino download.

If this is your first ESP32 upload, begin with the
[illustrated Arduino flashing guide](FLASHING-WITH-ARDUINO.md), then return
here for wiring and commissioning.

## What you need

- Waveshare ESP32-S3-ETH / ESP32-S3R8.
- Data-capable USB-C cable for the initial upload.
- Optional Waveshare PoE module. Begin with USB power during first bring-up.
- PixLite Mk3 on the same local network.
- Suitable industrial-grade microSD installed in the PixLite Mk3 for SHOWTime
  scene and playlist playback.
- Arduino IDE and internet access to install the Espressif board package.
- A volt-free push button, switch, relay contact, or temporary jumper.

Do not connect an externally powered signal to an ESP32 input. The ESP32
board's TF/microSD slot is not used by this firmware.
The illustrated [hardware and pinout guide](HARDWARE.md) includes sourcing
links, board orientation, and the optional isolated-input parts.

## 1. Install the Arduino board package

1. Install Arduino IDE.
2. Open **Tools → Board → Boards Manager**.
3. Search for `esp32` by Espressif Systems.
4. Install version **3.3.10**.

The board is uploaded as a generic ESP32-S3 because Waveshare does not require
a separate Arduino board definition.

## 2. Open the generated sketch

Download the
[v1.0.0-beta.6 same-named Arduino folder zip](https://github.com/AdvatekLabs/PixLite-Mk3-Contact-Trigger/releases/download/v1.0.0-beta.6/AdvatekTrigger-Waveshare-ESP32-S3-ETH.zip).
It is the easiest and least error-prone release artifact:

1. Extract the downloaded zip.
2. Open its `AdvatekTrigger-Waveshare-ESP32-S3-ETH` folder.
3. Double-click `AdvatekTrigger-Waveshare-ESP32-S3-ETH.ino`.

There is no code to paste or edit. If a GitHub release has not yet been
published, open the
[generated repository sketch](../generated/AdvatekTrigger-Waveshare-ESP32-S3-ETH/AdvatekTrigger-Waveshare-ESP32-S3-ETH.ino)
instead.

The generated sketch contains the same modular firmware and compressed web
interface as the maintainer build. Do not edit it directly.

## 3. Configure Arduino IDE

Choose **ESP32S3 Dev Module** and set:

| Option | Required value |
| --- | --- |
| Flash Size | `16MB` |
| Partition Scheme | `Huge APP (3MB No OTA/1MB SPIFFS)` |
| PSRAM | `OPI PSRAM` |
| USB Mode | `Hardware CDC and JTAG` |
| USB CDC On Boot | `Enabled` |
| Upload port | The COM/serial port belonging to the ESP32-S3 |

Leave unsupported camera and TF-card peripherals disconnected.

## 4. Upload

1. Power the board over USB-C.
2. Select its serial port.
3. Click **Upload**.
4. If the board does not enter download mode automatically, hold **BOOT**,
   briefly press reset, begin the upload, then release **BOOT** once writing
   starts.
5. Open Serial Monitor at `115200` baud.

A healthy boot reports the firmware, board profile, 16 MB flash, usable PSRAM,
W5500 initialization, link state, and an IP address. Do not continue if flash
or PSRAM is reported as degraded.

## 5. Open the interface

With Ethernet connected to the same network as the computer and PixLite Mk3, try:

1. `http://advatrigger.local/`
2. The numeric IP printed in Serial Monitor.

The interface displays the active numeric IP. Change **Local name** before
installing multiple units; `front-of-house`, for example, becomes
`http://front-of-house.local/`.

`.local` depends on mDNS support in the computer and network. The numeric IP
remains the reliable fallback.

## 6. Add a PixLite Mk3

1. Open **PixLite Mk3 controllers**.
2. Select **Discover PixLite Mk3 controllers**.
3. Confirm the expected nickname, IP address, and MAC address.
4. Connect it with Operator credentials where possible.
5. Use Administrator credentials only for operations that require them, such
   as PixLite Mk3 Test mode.
6. Open **Media** and verify the expected scenes and playlists.

If Media is empty, first confirm that the PixLite Mk3 has a suitable
industrial-grade microSD installed and that SHOWTime media has been placed on
it. This requirement belongs to the PixLite Mk3; the ESP32 trigger board's own
TF/microSD slot remains unused.

Discovery stores the MAC address so a later ADAR scan can update a changed DHCP
address. A controller can also be entered manually by IP.

## 7. Add the first input

1. Open **Inputs** and select **Add input**.
2. Give it an installation-specific name.
3. Select one permitted, unused GPIO.
4. Choose **Momentary** or **Maintained**.
5. Leave debounce at its 100 ms default for the first test.
6. Select a PixLite Mk3 and action for each edge.
7. Wait for the input status to show **Saved**.

**Next scene** and **Previous scene** move through the selected PixLite Mk3's
`.scn` list, skip playlists, and wrap at the ends. Choose Once or Loop for these
actions.

Use the action-test control before wiring a live switch. Normal-operation
intensity controls use GPIO actions.

## 8. Wire and test

Power down before changing field wiring.

```text
Assigned GPIO ───── dry contact ───── GND
```

For the first bench test, GPIO16 is a convenient known-tested choice. Confirm
Press and Release, or Latch On and Latch Off, while watching the PixLite Mk3 and
diagnostics. The status LED should pulse white once for each accepted,
debounced edge.

Read [WIRING.md](../WIRING.md) for every permitted and reserved pin. Use
[isolated contact-input hardware](PROTECTED-CONTACT-INPUTS.md) when switch
cables leave the enclosure or may experience ground differences or
interference.

![Waveshare ESP32-S3-ETH project pinout](waveshare-esp32-s3-eth-project-pinout.svg)

## 9. Finish commissioning

- Set a unique local name.
- Keep Ethernet selected, or deliberately choose Wi-Fi and enter the
  operational network SSID and password. Uplinks never change automatically.
  Ethernet and Wi-Fi DHCP/static addressing have been validated on this
  target. The tested board maintained Wi-Fi at -64 to -74 dBm without an
  optional external antenna in the bench location. RF conditions vary, so
  installers should verify signal margin at the final position and fit a
  manufacturer-compatible antenna when the installation requires it.
- Add an optional interface password if the local network requires it.
- Export a redacted backup.
- Reboot and confirm the PixLite Mk3, GPIO mapping, hostname, and LED preference
  persist.
- Record device IPs, MAC addresses, firmware versions, and wiring labels.

Backups deliberately omit Wi-Fi, PixLite Mk3, recovery, and interface passwords.

If a `.local` address does not resolve, open the numeric device IP displayed at
the top of the interface. This fallback is particularly important on Windows
networks where mDNS is unavailable or unreliable.

For normal operation after commissioning, continue with the short
[software user guide](USER-GUIDE.md).

## Recovery

Wi-Fi and Ethernet are explicit operating modes. If the configured Wi-Fi
network is unavailable, the controller does **not** automatically switch to
Ethernet. Use the BOOT recovery method selected under **Network → Advanced
network settings**:

- Hold **BOOT** for 5–14 seconds and release while the LED flashes to clear
  local authentication and start the selected 15-minute recovery connection.
- **Wi-Fi recovery AP:** join `Advatek-Trigger-XXXXXX`. The setup page should
  open automatically. If it does not, use a full browser and open
  `http://192.168.4.1/`.
- **Direct Ethernet:** disconnect the board from the network before holding
  BOOT. Releasing BOOT restarts the controller once into isolated recovery
  mode. The LED stays white while recovery starts. Wait for it to pulse cyan,
  then connect one computer directly to the board. Its DHCP server assigns the
  computer an address; open `http://192.168.4.1/`.
  Power the board separately over USB-C or its supported DC input because a
  normal computer Ethernet port does not provide PoE.
  Firmware refuses this mode if Ethernet already has link, protecting the
  existing LAN from an unintended DHCP server.
- Hold for 15–19 seconds and release while the LED is red to erase all
  configuration.
- Keep holding for 20 seconds to cancel the pending recovery/reset operation.

The status LED alternates orange and white while authentication recovery is
armed. After release it stays white while recovery starts, then pulses blue
for Wi-Fi recovery or cyan for direct Ethernet recovery. A flashing red
indication means the selected recovery
connection could not start; normal steady orange returns after five seconds.

Factory reset is destructive. Export a backup before testing it.

Network changes use an in-page confirmation that also works in phone captive
portals: select **Save network and restart**, then select **Tap again to save
and restart**. Wait for **Settings accepted - restarting** before leaving the
page.

## Common problems

| Symptom | Check |
| --- | --- |
| Upload port missing | Use a data-capable USB cable and enable USB CDC on boot |
| Flash or PSRAM degraded | Recheck the exact Arduino IDE options |
| No Ethernet address | Check W5500 link, DHCP, and Serial diagnostics |
| `.local` name fails | Use the numeric IP shown in Serial/UI |
| PixLite Mk3 not discovered | Confirm both devices share a subnet; try manual IP |
| Scene list empty | Confirm the PixLite Mk3 has an industrial-grade microSD, SHOWTime files exist, and credentials are accepted |
| Multiple trigger flashes | Increase that input's debounce in the web UI |
| Pin remapping required | Select a permitted unused GPIO and save again |

For a reproducible report, follow [SUPPORT.md](../SUPPORT.md).
