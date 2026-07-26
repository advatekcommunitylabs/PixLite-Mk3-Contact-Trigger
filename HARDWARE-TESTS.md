# Hardware acceptance record

- Target: Waveshare ESP32-S3-ETH (ESP32-S3R8, W5500, optional PoE module fitted)
- Firmware/web UI: 1.0.0-beta.6
- Board profile: waveshare-esp32-s3-eth 1.2.0
- Configuration schema: 5
- PixLite: A4-S Mk3, firmware 3.14.2, API v1.9
- Test date: 2026-07-26
Status: **Bench bring-up passed; full hardware acceptance remains incomplete**

## Industrial 8DI/8RO target awaiting hardware

Target profiles now also include the Waveshare
`ESP32-S3-ETH-8DI-8RO` and `ESP32-S3-POE-ETH-8DI-8RO` as one firmware target:

- profile: `waveshare-esp32-s3-eth-8di-8ro` 1.0.0;
- status: compile-supported, ordered hardware not yet received;
- isolated inputs: DI1–DI8 / GPIO4–GPIO11;
- W5500: IRQ 12, MOSI 13, MISO 14, SCLK 15, CS 16, reset 39;
- RGB: GPIO38; BOOT recovery: GPIO0;
- relays, RS485, buzzer, and TF card intentionally unused.

The standard and PoE products use the same profile. Required bring-up includes
USB flashing, 16 MB flash/8 MB PSRAM, Ethernet DHCP/static, all eight isolated
passive inputs, supported active-input polarity, BOOT recovery, RGB behavior,
PoE cold boots on the PoE model, and the complete PixLite regression flow.

The original end-to-end GPIO results below were captured on beta.3. Beta.4 was
subsequently flashed and passed the regression checks recorded next; its
physical GPIO flow still needs one repeat before all beta.3 evidence can be
carried forward.

## Beta.6 Arduino-release flashing check (2026-07-26)

- The generated Pico-header development-board sketch was opened directly in
  Arduino IDE 2.3.10 with Espressif Arduino-ESP32 3.3.10.
- `ESP32S3 Dev Module`, 16 MB Flash, Huge APP, OPI PSRAM, Hardware CDC/JTAG,
  and USB CDC on boot were selected before upload.
- The IDE compiled the single-file convenience artifact, wrote and verified
  the image over USB, and reset the physical board successfully.
- The post-upload banner reported firmware 1.0.0-beta.6, board profile
  `waveshare-esp32-s3-eth` 1.2.0, 16,777,216-byte flash, 8,388,608-byte PSRAM,
  and normal memory mode.
- The public [illustrated Arduino flashing guide](docs/FLASHING-WITH-ARDUINO.md)
  uses screenshots captured during this upload. Unit-specific network
  addresses are not included.

## Beta.5 regression (2026-07-26)

- Schema v4 migrated to v5 without losing hostname, PixLite identity,
  Operator selection, 7% LED brightness, or input-slot configuration.
- The saved generic name was refreshed from the ADAR nickname to
  `PixLite A4-S Mk3`; the UI shows that name, IP, and MAC. The public record
  redacts the unit-specific MAC as `******E0E9C1`. A save/restart regression exposed fragmented-NVS write
  failure; inactive-slot retry plus sequence rollback was added, after which
  the nickname and all other settings persisted across a software restart.
- The embedded SPA identifies itself as Advatek Labs, has no separate “A”
  mark, and completed desktop plus 375 px responsive browser review without
  horizontal overflow or console warnings.
- GPIO choices no longer expose camera-connector implementation wording.
  The flow adds one input at a time, defaults to 100 ms debounce, and shows
  only fields needed by the selected action.
- PixLite v1.9 rejected Test mode under Operator as unauthorized, as expected.
  With the explicit Administrator account, Test Color entered `testData` /
  `setColor` at RGB `16,4,0`; Live mode then restored normal output. The saved
  target was returned to Operator after the test.
- Runtime after the final flash: 192,312-byte minimum internal heap,
  163,828-byte largest block, 7,572-byte network-task stack watermark, and
  normal PSRAM mode.

## Beta.4 regression (2026-07-25)

- Schema v3 migrated to v4 without losing hostname, PixLite MAC/IP, LED
  brightness, input names/actions/debounce, or hardware bindings.
- The embedded SPA loaded from `http://advatrigger.local/`, began with zero
  visible input cards, and added an unassigned input at 100 ms debounce.
- ADAR found the A4-S as configured target `primary` on the private test LAN.
- Boot automatically negotiated API v1.9 and loaded both `.scn` files.
- Scene-once and Stop were accepted; status moved from playback back to live.
- Runtime after boot: 200,580-byte minimum internal heap, 163,828-byte largest
  block, 9,240-byte network-task stack watermark, normal PSRAM mode.
- Remaining beta.4 regression: save an input and repeat a physical GPIO edge.

## Bench setup

- ESP32 connected over USB on COM4 and by wired Ethernet to a DHCP router.
- Test computer connected to the same router by Wi-Fi.
- ESP32 address: private DHCP lease, redacted from the public record.
- PixLite address: private LAN address, redacted from the public record.
- PixLite MAC: `******E0E9C1` (unit-specific prefix redacted).
- Arduino-ESP32 3.3.10, ESP32S3 Dev Module, 16 MB flash, Huge APP,
  OPI PSRAM, Hardware CDC/JTAG, and USB CDC on boot.
- One dry-contact jumper/button connected between GPIO16 and GND.
- Exact PCB and PoE-module revision markings still need recording.

## Results

| Area | Result / evidence | Remaining gate |
| --- | --- | --- |
| Identity | **Pass.** Canary and production firmware identified ESP32-S3 revision 0.2, 16,777,216-byte flash and 8,388,608-byte PSRAM. RGB GPIO21 exercised red/green/blue/white and BOOT GPIO0 transitions were observed. | Record the PCB revision markings. |
| W5500 / DHCP | **Pass for initial DHCP bring-up.** W5500 initialized on the first attempt, linked at 100 Mbps full duplex and received a private DHCP lease. | Static IPv4, cable removal/recovery and repeated reset testing. |
| PoE | PoE module is fitted but treated as power-only by firmware. | USB-first isolation, 25 PoE-only cold boots, resets, lease renewal and static addressing. |
| Wi-Fi station | Not exercised. | DHCP/static, incorrect credentials and reconnect. |
| Recovery connections | The earlier general setup-AP result is superseded; normal commissioning now uses Ethernet. | Test Wi-Fi AP recovery and direct-Ethernet DHCP at `192.168.4.1`, LAN-link refusal, timeout, and return to the selected uplink. |
| Local address | **Pass on this Windows computer.** `http://advatrigger.local/` resolved to the ESP32's private DHCP address; firmware logged the claimed mDNS name. The UI showed both addresses and normalized `Front Of House.local` to the preview `http://front-of-house.local/`. | Confirm from macOS and on representative customer routers; retain numeric-IP fallback for networks blocking mDNS. |
| ADAR | **Pass for multicast.** Discovered “PixLite A4-S Mk3”, firmware 3.14.2, MAC `******E0E9C1`, on the private test LAN; MAC selection survived firmware reboot. | Directed-broadcast fallback, malformed/deduplicated responses, DHCP IP change and 64-device cap on hardware. |
| PixLite connection/auth | **Pass for operator/no-password.** `/ver` negotiated API v1.9 and status polling remained responsive. | Password-protected operator access, explicit admin fallback and invalid-password behavior. |
| Media/playback | **Partial pass.** Loaded two nonzero `.scn` files. Scene Once, Loop Forever, deterministic Stop via `modeLive`, Blank and status polling passed. | No `.pl` file was present, so playlist once/loop remains pending. |
| GPIO16 momentary | **Pass.** With 100 ms debounce, normally-open Press entered looping playback and Release returned to Live. Repeated manual closures produced no stuck state. | Normally-closed wiring and an instrumented bounce-count test. |
| GPIO16 maintained | **Pass.** Latch On held one scene in continuous playback without retrigger/ramp state; Latch Off returned to Live. | Boot with contact held. |
| Other contact pins | Not electrically exercised. | GPIO1, 2, 15, 18, 38, 39 and 40, including camera-shared warnings. |
| Intensity | **Partial pass.** Pixels 50%, Aux 1, Pixels + configured Aux, High/Low priority and cleanup were accepted. A physical GPIO16 Brighter hold reached the 255 clamp while input scanning and status polling remained responsive; release stopped the ramp and returned Pixels to Low priority. | Darker hold, exact 400/250 ms timing capture, every Aux output and release during a deliberately delayed HTTP request. |
| Aux 1 observation | Direct and firmware-issued v1.9 requests both accepted `fc:128`; this A4-S reported an effective programmed factor of 121 for Aux 1. Release returned `progPri` to `Lo`. | Confirm whether the 121 result is caused by the PixLite Aux configuration and document expected scaling. |
| LED behavior/UI | **Pass.** Idle colour is orange at RGB 255/48/0 and contact edges flash white without blocking scans. The live web control changed brightness to 30%, displayed and persisted it, and restored 100%. API tests also covered disabled/off and 25% brightness. | Long-duration observation and optional user preference tuning. |
| Configuration/migration | **Pass for schema v3 to v4 on this unit.** PixLite identity, hostname, LED setting and all input data survived migration/reflash. | Frozen-v1/v2 device migration, corrupt-newest-slot fallback and cross-board remapping on hardware. |
| Backup | **Partial pass.** Exported schema-v3 JSON was 4,910 bytes, set `secretsOmitted:true`, and contained none of the password/hash/salt fields. | Import/restore on this device and import requiring GPIO remapping on another profile. |
| Recovery | BOOT electrical state was confirmed by the canary. | Five-second authentication recovery and fifteen-second factory reset, followed by configuration restoration. |
| Arbitration | Host tests pass latest-event-wins and two-second offline expiry. GPIO scanning remained responsive during normal PixLite traffic. | Hardware test with queued conflicting events, disconnect expiry and delayed HTTP release. |
| Stress / burn-in | Not executed. | Recovery-AP + Ethernet traffic, operational Wi-Fi Station, configuration-import stress and a 24-hour trigger/network burn-in. |
| Memory gates | **Pass during this session.** Minimum internal heap 199,616 bytes, largest internal block 167,924 bytes and network-task stack watermark 8,020 bytes. PSRAM free was approximately 8.31 MB. | Record minima over the 24-hour burn-in. |

## Build and protocol evidence

- Generated Arduino sketch matches canonical modular source.
- Canonical firmware size: 1,255,871 bytes of the 3,145,728-byte application
  partition.
- Static data/BSS: 61,444 bytes of 327,680 bytes.
- Embedded UI: 12,249 bytes gzip.
- TypeScript/UI/repository contracts: 26 tests passed.
- A live flow test exposed that PixLite v1.9 rejects `progInt/Lo` when a
  duration is included. The codec now omits both factor and duration for `Lo`;
  the regression check passes and the corrected firmware is flashed.
- Final bench state: PixLite Live, no enabled inputs, hostname `advatrigger`,
  and the user's 7% LED brightness setting retained.

## Release decision

This unit is suitable for continued bench development, but the release must
not be labelled hardware-ready until the remaining PoE, seven GPIO,
operational Wi-Fi, recovery AP, playlist, static-IP, stress and burn-in gates
are completed.
