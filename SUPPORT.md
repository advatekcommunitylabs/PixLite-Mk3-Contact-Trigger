# Support

This is an Advatek Labs community project. **Advatek Technical Support does not
cover this third-party hardware or any Advatek Labs community project.**

GitHub Issues are the right place for reproducible firmware, interface,
documentation, and hardware-integration problems. Community assistance is not
a substitute for urgent or show-critical support.

Third-party products mentioned by the project document compatibility. Advatek
does not endorse the listed products. Installation and electrical-code
compliance remain the integrator's responsibility.

## Before opening an issue

1. Confirm the board and PixLite Mk3 family are listed in `compatibility.json`.
2. Recheck the exact Arduino IDE settings in
   [Getting started](docs/GETTING-STARTED.md).
3. Read Serial Monitor at `115200` baud and the web **Diagnostics** section.
4. Try the numeric device IP if the `.local` name does not resolve.
5. Export a redacted backup if configuration is relevant.
6. Search existing issues.

## Include in a bug report

- Firmware and web UI versions.
- Board/profile and Arduino-ESP32 version.
- PixLite Mk3 model, firmware, negotiated API version, nickname, and MAC suffix.
- Ethernet or Wi-Fi, DHCP or static, and whether the setup AP works.
- Exact action, switch mode, debounce, GPIO, and wiring approach.
- Steps to reproduce, expected result, and observed result.
- Relevant redacted diagnostics and memory figures.
- Whether the issue survives a reboot.

Remove Wi-Fi, AP, web, and PixLite Mk3 passwords. Do not post private network
captures or the Advatek protocol documents.

## Hardware and safety

State whether the input is a direct short dry contact, protected input, or
optically isolated input. Include a wiring diagram when possible. Do not
continue testing hardware that is hot, damaged, intermittently powered, or
receiving external voltage on an ESP32 GPIO.

Security vulnerabilities should not be filed publicly. Follow
[SECURITY.md](SECURITY.md).
