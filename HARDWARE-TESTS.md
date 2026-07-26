# Hardware acceptance record

- Targets: Waveshare ESP32-S3-ETH and ESP32-S3-POE-ETH-8DI-8RO
  (ESP32-S3R8, W5500)
- Firmware/web UI: 1.0.0-beta.6
- Board profiles: waveshare-esp32-s3-eth 1.2.0;
  waveshare-esp32-s3-eth-8di-8ro 1.1.0
- Configuration schema: 5
- PixLite Mk3: A4-S, firmware 3.14.2, API v1.9
- Test date: 2026-07-26
Status: **Bench bring-up passed; full hardware acceptance remains incomplete**

## Industrial 8DI/8RO real-board bring-up

Target profiles now also include the Waveshare
`ESP32-S3-ETH-8DI-8RO` and `ESP32-S3-POE-ETH-8DI-8RO` as one firmware target:

- profile: `waveshare-esp32-s3-eth-8di-8ro` 1.1.0;
- status: physical PoE-board bring-up passed; extended acceptance remains open;
- isolated inputs: DI1–DI8 / GPIO4–GPIO11;
- W5500: IRQ 12, MOSI 13, MISO 14, SCLK 15, CS 16, reset 39;
- RGB: GPIO38; BOOT recovery: GPIO0;
- relays, RS485, buzzer, and TF card intentionally unused.

The standard and PoE products use the same profile. On 2026-07-26, the physical
PoE model passed USB flashing, 16 MB flash/8 MB PSRAM checks, PoE-only W5500
Ethernet at 100 Mbps full duplex, DHCP, Windows mDNS, ADAR discovery, PixLite Mk3
API v1.9 negotiation, and immediate media listing.

Passive-contact testing established the exact field wiring: leave input `COM`
floating and close `DGND` to the chosen `DIx`. A `COM`–`DIx` link produced no
input transition, while `DGND`–`DIx` did. Two complete manual sweeps then
registered DI1 through DI8 on GPIO4 through GPIO11 in order, with every release
returning to the inactive state.

The test canary now exposes a read-only `/api/inputs` endpoint so an installer
can verify isolated inputs while the board is powered only from PoE. During the
same session, production diagnostics exposed an incorrectly scaled FreeRTOS
stack watermark: the original 12 KB network task had less than 200 bytes
remaining. The reporting unit was corrected and the task increased to 20 KB;
the corrected production build must pass the ≥3 KB gate before release.

The industrial board's GPIO38 addressable LED was also verified to use RGB
byte order, unlike the smaller development board's GRB order. Board profile
1.1.0 records that difference; production idle colour changed from the
incorrect bright green to the intended orange, while logical white trigger
flashes remain unchanged.

Remaining industrial-board gates include static IPv4, direct-Ethernet/BOOT
recovery, repeated PoE cold boots, active wet-input polarity, and the full
burn-in.

### Industrial production regression (2026-07-26)

- Profile 1.1.0 migrated the saved PixLite Mk3 and DI1 configuration without an
  erase. The board rebooted PoE-only, linked at 100 Mbps full duplex, resolved
  as `advatrigger.local`, negotiated PixLite Mk3 API v1.9, and automatically loaded
  seven scenes without a remove/re-add or manual media refresh.
- The corrected ESP-IDF stack measurement reported a 17,228-byte minimum
  network-task watermark during the trigger flow. Minimum internal heap was
  221,240 bytes and the largest internal block was 192,500 bytes.
- DI1 used the verified passive-contact wiring (`DGND` to `DI1`, input `COM`
  left unused), maintained mode, and 100 ms debounce. Two close/open cycles
  produced exactly four event-sequence increments.
- First close selected and looped `Stars.scn`; first open returned to Live.
  The second close selected and looped `Amber Glow.scn`, proving that the
  per-PixLite Mk3 scene cursor survives the intervening Live/Stop action. The
  final open returned to Live with the PixLite Mk3 online and no reported error.
- Logical orange initially appeared bright green. Changing only the industrial
  board's addressable-LED order from GRB to RGB produced the intended orange;
  the smaller board retains its previously validated GRB order.

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

- Schema v4 migrated to v5 without losing hostname, PixLite Mk3 identity,
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
- PixLite Mk3 API v1.9 rejected Test mode under Operator as unauthorized, as expected.
  With the explicit Administrator account, Test Color entered `testData` /
  `setColor` at RGB `16,4,0`; Live mode then restored normal output. The saved
  target was returned to Operator after the test.
- Runtime after the final flash: 192,312-byte minimum internal heap,
  163,828-byte largest block, 7,572-byte network-task stack watermark, and
  normal PSRAM mode.

## Beta.4 regression (2026-07-25)

- Schema v3 migrated to v4 without losing hostname, PixLite Mk3 MAC/IP, LED
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
- PixLite Mk3 address: private LAN address, redacted from the public record.
- PixLite Mk3 MAC: `******E0E9C1` (unit-specific prefix redacted).
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
| PixLite Mk3 connection/auth | **Pass for operator/no-password.** `/ver` negotiated API v1.9 and status polling remained responsive. | Password-protected operator access, explicit admin fallback and invalid-password behavior. |
| Media/playback | **Partial pass.** Loaded two nonzero `.scn` files. Scene Once, Loop Forever, deterministic Stop via `modeLive`, Blank and status polling passed. | No `.pl` file was present, so playlist once/loop remains pending. |
| GPIO16 momentary | **Pass.** With 100 ms debounce, normally-open Press entered looping playback and Release returned to Live. Repeated manual closures produced no stuck state. | Normally-closed wiring and an instrumented bounce-count test. |
| GPIO16 maintained | **Pass.** Latch On held one scene in continuous playback without retrigger/ramp state; Latch Off returned to Live. | Boot with contact held. |
| Other contact pins | Not electrically exercised. | GPIO1, 2, 15, 18, 38, 39 and 40, including camera-shared warnings. |
| Intensity | **Partial pass.** Pixels 50%, Aux 1, Pixels + configured Aux, High/Low priority and cleanup were accepted. A physical GPIO16 Brighter hold reached the 255 clamp while input scanning and status polling remained responsive; release stopped the ramp and returned Pixels to Low priority. | Darker hold, exact 400/250 ms timing capture, every Aux output and release during a deliberately delayed HTTP request. |
| Aux 1 observation | Direct and firmware-issued v1.9 requests both accepted `fc:128`; this A4-S reported an effective programmed factor of 121 for Aux 1. Release returned `progPri` to `Lo`. | Confirm whether the 121 result is caused by the PixLite Mk3 Aux configuration and document expected scaling. |
| LED behavior/UI | **Pass.** Idle colour is orange at RGB 255/48/0 and contact edges flash white without blocking scans. The live web control changed brightness to 30%, displayed and persisted it, and restored 100%. API tests also covered disabled/off and 25% brightness. | Long-duration observation and optional user preference tuning. |
| Configuration/migration | **Pass for schema v3 to v4 on this unit.** PixLite Mk3 identity, hostname, LED setting and all input data survived migration/reflash. | Frozen-v1/v2 device migration, corrupt-newest-slot fallback and cross-board remapping on hardware. |
| Backup | **Partial pass.** Exported schema-v3 JSON was 4,910 bytes, set `secretsOmitted:true`, and contained none of the password/hash/salt fields. | Import/restore on this device and import requiring GPIO remapping on another profile. |
| Recovery | BOOT electrical state was confirmed by the canary. | Five-second authentication recovery and fifteen-second factory reset, followed by configuration restoration. |
| Arbitration | Host tests pass latest-event-wins and two-second offline expiry. GPIO scanning remained responsive during normal PixLite Mk3 traffic. | Hardware test with queued conflicting events, disconnect expiry and delayed HTTP release. |
| Stress / burn-in | Not executed. | Recovery-AP + Ethernet traffic, operational Wi-Fi Station, configuration-import stress and a 24-hour trigger/network burn-in. |
| Memory gates | **Pass during this session.** Minimum internal heap 199,616 bytes, largest internal block 167,924 bytes and network-task stack watermark 8,020 bytes. PSRAM free was approximately 8.31 MB. | Record minima over the 24-hour burn-in. |

## Build and protocol evidence

- Generated Arduino sketch matches canonical modular source.
- Generated firmware size: 1,258,751 bytes for the development board and
  1,259,087 bytes for the industrial board, from the 3,145,728-byte
  application partition.
- Static data/BSS: 61,540 bytes of 327,680 bytes.
- Embedded UI: 13,296 bytes gzip.
- TypeScript/UI/repository contracts: 31 tests passed.
- A live flow test exposed that PixLite Mk3 API v1.9 rejects `progInt/Lo` when a
  duration is included. The codec now omits both factor and duration for `Lo`;
  the regression check passes and the corrected firmware is flashed.
- Final industrial-board bench state: PixLite Mk3 Live, DI1 enabled as a maintained
  Next-scene/Live switch at 100 ms debounce, hostname `advatrigger`, and the
  orange status LED enabled at 100%.

## Release decision

These units are suitable for continued bench development, but the release must
not be labelled hardware-ready until the remaining repeated PoE cold-boot,
operational Wi-Fi, recovery, playlist, static-IP, stress and burn-in gates are
completed.
