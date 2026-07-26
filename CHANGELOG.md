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
- Editable diagrams.net, SVG, PNG, and native PowerPoint wiring assets for
  public community guides.
- GPIO-assignable Next Scene and Previous Scene actions with per-PixLite media
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
- Board selection moved into generated target defines and a shared active-board
  adapter; PixLite, ADAR, UI, and configuration behavior remain common.

### Pending validation

- Complete isolated-input stripboard build and cable tests.
- Complete the remaining PoE, GPIO, recovery, Wi-Fi, playlist, static-IP,
  stress, and burn-in acceptance gates.

## [1.0.0-beta.5] - 2026-07-26

### Added

- Up to 16 saved, action-addressable PixLite Mk3 controllers.
- ADAR nickname and MAC presentation in the compact Advatek Labs interface.
- Solid-colour PixLite Test mode with explicit Administrator requirement.
- Add/remove input workflow with 100 ms default debounce.
- Persistent status LED enable and brightness controls.
- Editable first-run `advatrigger.local` hostname.
- BOOT-button LED feedback and 20-second cancellation zone.

### Changed

- Configuration schema advanced to v5.
- GPIO choices use the PoE-accessible pin allow-list.
- Stop Playback explicitly returns the PixLite to Live mode.
- PixLite status parsing follows observed Mk3 v1.9 response values.
- Configuration saves retry the inactive NVS slot without advancing sequence
  after a failed write.

### Validation

- Bench-tested with a Waveshare ESP32-S3-ETH and PixLite A4-S Mk3 running
  firmware 3.14.2/API v1.9.
- Full hardware-ready acceptance remains pending; see
  [HARDWARE-TESTS.md](HARDWARE-TESTS.md).
