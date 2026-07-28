# Adding another ESP32 board

Board support is additive. Do not copy the project or edit core behaviour for a
new PCB.

## Board profile contract

Add a profile under `firmware/AdvatekTrigger/src/boards/` and an entry to
`boards/manifest.json`. A profile defines:

- stable ID and display name;
- ESP32 variant, Arduino FQBN/options, flash and PSRAM requirements;
- Ethernet kind and initialization callback;
- Ethernet/SPI and all other reserved pins;
- explicit allowed/default contact GPIOs or isolated input terminals, a reason
  for every reserved pin, and maximum input count;
- recovery button, status LED, input hardware kind, and optional peripheral
  capability flags.

Add a unique `buildMacro` and artifact name to the manifest. The deterministic
generator emits one raw `.ino` and same-named Arduino folder per manifest
entry; end users must never need to edit a board-selection define.

If the board uses a different Ethernet implementation, add an ESP32 platform
adapter that exposes the same operational-uplink behaviour. Core GPIO, PixLite Mk3,
ADAR, configuration, and web code must remain unchanged.

## Required checklist

- [ ] Review the schematic and audit every strap, flash, PSRAM, USB, serial,
      Ethernet and onboard-device pin.
- [ ] Compile the modular target and its generated `.ino`.
- [ ] Verify flash/partition/PSRAM settings and memory budget.
- [ ] Exercise PSRAM-allocation failure and confirm the recovery UI remains available.
- [ ] Test Ethernet DHCP and static IPv4.
- [ ] Test operational Wi-Fi Station, mDNS, the time-limited BOOT recovery AP,
      direct-Ethernet DHCP recovery, and refusal while Ethernet has link.
- [ ] Test ADAR multicast and directed-broadcast fallback.
- [ ] Electrically test every allowed contact GPIO, NO/NC, debounce, and BOOT
      recovery timing.
- [ ] Test authenticated `/ver`, version negotiation, file list, playback,
      stop, blank, status and Pix/Aux intensity against a real PixLite Mk3.
- [ ] Test reboot persistence, both NVS slots, factory reset, and backup import.
- [ ] Import a backup from another board and confirm unavailable GPIOs require
      remapping while portable behaviour is retained.
- [ ] Add CI, compatibility manifest, wiring guide and known limitations.

The board is not “supported” until the checklist evidence is recorded in
`HARDWARE-TESTS.md`.
