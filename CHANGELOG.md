# Changelog

This project follows semantic versioning for firmware and web UI releases.
Configuration schema and board-profile versions are tracked independently.

## [Unreleased]

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

### Changed

- Embedded web gzip headers are normalized so generated Arduino artifacts are
  byte-for-byte reproducible on Windows and Linux.
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

### Validation

- Both generated Arduino artifacts compiled and uploaded to their matching
  physical Waveshare boards with 16 MB flash and 8 MB PSRAM.
- Clean end-user zips for both targets were extracted and compiled using the
  documented Arduino-ESP32 3.3.10 settings.
- The industrial board passed all eight isolated contact inputs and 25
  consecutive PoE-only cold boots.
- A PixLite A4-S Mk3 running firmware 3.14.2/API v1.9 passed discovery,
  automatic media refresh, alphabetic scene ordering, scene and playlist
  playback, scene stepping, Test modes, and intensity actions.
- Remaining gates are documented operational Wi-Fi, static addressing, BOOT
  recovery, the other seven development-board GPIOs, stress, and the
  eight-hour dual-board burn-in.

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
