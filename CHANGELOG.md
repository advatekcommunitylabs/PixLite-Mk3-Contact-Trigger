# Changelog

This project follows semantic versioning for firmware and web UI releases.
Configuration schema and board-profile versions are tracked independently.

## [Unreleased]

- Added a concise public validation log for hardware-affecting changes made
  after the initial public beta.
- Refocused public field-wiring guidance on complete off-the-shelf isolated
  input modules and removed component-level PCB, BOM, and stripboard
  construction instructions.
- Added a supplier-neutral transparent illustration of an eight-channel
  optocoupler-module category.
The current release candidate identifies as `1.0.0-beta.6`.

### Added

- Community getting-started, architecture, contribution, support,
  security, and issue-reporting documentation.
- Local documentation-link validation and compatibility-version
  contracts to CI.
- Project-specific Waveshare/PoE pinout and a beginner hardware sourcing guide.
- A stable latest-release download path and self-contained `START-HERE.md`
  inside the Arduino release zip.
- A direct-versus-isolated button decision guide with a protected direct-input
  circuit and the recommended small-event isolated topology.
- Editable diagrams.net, SVG, and PNG wiring assets for public community
  guides. The optional PowerPoint working file remains local and is not
  published.
- GPIO-assignable Next Scene and Previous Scene actions with per-PixLite Mk3 media
  caches, deterministic wraparound, and periodic file-list refresh.
- A second board target for the standard and PoE
  Waveshare ESP32-S3-ETH-8DI-8RO, including isolated DI terminal labels,
  and per-board generated Arduino downloads.
- A compact orange trigger favicon for the embedded browser interface.

### Changed

- Embedded web gzip headers are normalized so generated Arduino artifacts are
  byte-for-byte reproducible on Windows and Linux.
- Embedded HTML removes optional void-element slashes and safe attribute
  quotes, keeping the favicon and complete SPA inside the 13 KiB gzip budget.
- ESP32 Ethernet dependencies removed from the generic board-profile contract
  so portable core tests compile cleanly on a host.
- Release assets now include the hardware guide and pinout alongside the
  same-named Arduino folder.
- Release zips defensively exclude any ignored Arduino `build/` directory, and
  the flashing guide explains the Windows path-length workaround verified with
  the longer industrial sketch name.
- Board selection moved into generated target defines and a shared active-board
  adapter; PixLite Mk3, ADAR, UI, and configuration behavior remain common.
- PixLite response handling clears its reusable PSRAM buffer before every
  request and uses distinct internal error codes, preventing a transient
  transport failure from quoting stale successful JSON in diagnostics.
- Web configuration and PixLite Mk3 response parsing now use separate PSRAM
  token pools because they run on different tasks.
- JSON object lookup now walks key/value pairs explicitly. This prevents an
  uplink value such as `ethernet` or `wifi` from masking the later nested
  addressing object with the same name.
- The industrial guide now uses transparent, self-contained product renders,
  visible port annotations, standard normally-open switch symbols, and
  non-prescriptive cable guidance. Internal acceptance steps remain in the
  hardware test record. The end-user guide contains installation instructions.
- Ethernet operation now initializes an idle, unassociated Wi-Fi interface so
  BOOT recovery can reliably create its temporary access point without
  providing any automatic Wi-Fi fallback.
- Direct-Ethernet recovery now uses a one-time recovery reboot and starts its
  DHCP service on lwIP's TCP/IP thread. This replaces the unsafe live
  transition that produced a panic during pre-release hardware testing.
- Direct-Ethernet DHCP now waits for physical link, supplies an explicit
  subnet mask, and registers the lease callback required by Espressif's
  low-level DHCP server. This prevents the post-ACK null-callback panic found
  with a class-compliant Windows USB Ethernet adapter.
- BOOT recovery now ends its orange/white hold indication on solid white
  through the one-time restart and changes to cyan only when isolated
  direct-Ethernet recovery is ready for the computer to connect.
- Wi-Fi Station startup now disables Arduino's separate persistent radio
  state before applying the schema-versioned project credentials. This fixes a
  real header-board case where a previous sketch left Station at
  `WL_DISCONNECTED` without attempting association.
- `/api/state` and the diagnostic log now expose password-safe Wi-Fi status,
  disconnect reason, and connected RSSI for commissioning.
- Network saving no longer depends on a native browser confirmation dialog.
  The embedded page now uses a visible two-tap confirmation and accepted /
  restarting state that was verified in the automatic phone captive portal.

### Validation

- Both generated Arduino artifacts compiled and uploaded to their matching
  physical Waveshare boards with 16 MB flash and 8 MB PSRAM.
- Clean end-user zips for both targets were extracted and compiled using the
  documented Arduino-ESP32 3.3.10 settings.
- Both Waveshare targets passed 25 consecutive PoE-only cold boots. The
  industrial board also passed all eight isolated contact inputs.
- Both boards passed Wi-Fi Station DHCP/static addressing, PixLite Mk3
  reconnection, media polling, and `.local` access. The industrial target used
  its supplied antenna. The header target passed without an optional external
  antenna at -64 to -74 dBm in the bench location.
- Both boards passed Ethernet static-address startup and restoration to DHCP.
- Both boards rejected oversized configuration, incomplete Wi-Fi, invalid
  hostname, unconfirmed factory-reset, missing-target, and unknown-endpoint
  requests without changing configuration or input event state.
- Concurrent direct-IP reads from both ESP32s passed without failure. Windows
  `.local` resolution became unreliable when both names were queried
  concurrently, so the numeric IP shown in the SPA remains the documented
  fallback.
- A PixLite A4-S Mk3 running firmware 3.14.2/API v1.9 passed discovery,
  automatic media refresh, alphabetic scene ordering, scene and playlist
  playback, scene stepping, Test modes, and intensity actions.
- Both boards passed Wi-Fi BOOT-recovery activation, visible LED feedback,
  temporary-SSID access, continued Ethernet/PixLite Mk3 operation, memory
  thresholds, and automatic expiry.
- The header board completed 30 consecutive Wi-Fi state/media/log cycles with
  zero request failures, seven alphabetically ordered scenes, two
  alphabetically ordered playlists, and all memory/stack gates passing.
- The header board passed revised direct-Ethernet recovery with a
  class-compliant USB Ethernet adapter: link-safe arming, Windows DHCP,
  `192.168.4.1` SPA access, PixLite Mk3-operation isolation, memory thresholds,
  explicit exit, normal DHCP restoration, and automatic PixLite Mk3
  reconnection.
- The industrial board passed the same direct-Ethernet recovery flow, including
  isolated DHCP, SPA access, normal-LAN exclusion, power-cycle exit, DHCP
  restoration, and automatic PixLite Mk3/media reconnection.
- GPIO1, GPIO2, GPIO15, GPIO16, GPIO18, GPIO38, GPIO39, and GPIO40 on the
  header board each passed dry-contact make/break detection without
  cross-triggering. GPIO16 also passed normally-closed polarity and boot-held
  startup without a false action.
- Deliberately invalid Wi-Fi credentials made the header board unreachable
  without a silent Ethernet fallback. BOOT Wi-Fi recovery restored the
  credentials, operational Station connection, PixLite Mk3, and media.
- A destructive BOOT factory reset erased all saved targets and inputs, started
  factory-default Ethernet DHCP, and then restored Wi-Fi, PixLite Mk3 and the
  GPIO15 actions from a redacted backup plus re-entered secret.
- The final captive-portal UI was compiled and flashed to both physical
  targets. The header board completed an automatic phone captive-portal
  save/restart regression; both boards restored their saved configuration,
  PixLite Mk3 connection, media, and memory gates.

## [1.0.0-beta.5] - 2026-07-26

### Added

- Up to 16 saved, action-addressable PixLite Mk3 controllers.
- ADAR nickname and MAC presentation in the compact Advatek Labs interface.
- Solid-colour PixLite Mk3 Test mode with explicit Administrator requirement.
- Add/remove input workflow with 100 ms default debounce.
- Persistent status LED enable and brightness controls.
- Editable first-run `advatrigger.local` hostname.
- BOOT-button LED feedback and 20-second cancellation zone.

### Changed

- Configuration schema advanced to v5.
- GPIO choices use the PoE-accessible pin allow-list.
- Stop Playback explicitly returns the PixLite Mk3 to Live mode.
- PixLite Mk3 status parsing follows observed v1.9 response values.
- Configuration saves retry the inactive NVS slot without advancing sequence
  after a failed write.

### Validation

- Bench-tested with a Waveshare ESP32-S3-ETH and PixLite A4-S Mk3 running
  firmware 3.14.2/API v1.9.
- Full hardware-ready acceptance remains pending; see
  [HARDWARE-TESTS.md](HARDWARE-TESTS.md).
