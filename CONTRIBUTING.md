# Contributing

Thank you for helping make the Advatek Labs PixLite Contact Closure Trigger
more useful and reliable for the community.

## Before starting

- Search existing issues and pull requests for overlapping work.
- For a substantial behavior, API, configuration-schema, or board-profile
  change, open a proposal before investing in an implementation.
- Keep version 1 boundaries in mind. Features such as cloud control, PixLite
  network editing, OTA, and non-ESP32 targets require an explicit project
  decision rather than an opportunistic dependency.
- Never include PixLite protocol documents, credentials, private network
  captures, or unredacted configuration exports in the repository.

## Branch workflow

- Create feature and fix branches from `dev`.
- Open normal pull requests against `dev`.
- Merge `dev` into `main` only through a release pull request after required
  checks and hardware evidence are complete.
- Tag releases from `main`; do not build a release from an unmerged feature
  branch.

## Development setup

Required tools:

- Node.js 22
- pnpm 10.12.1
- C++17 host compiler
- Arduino CLI with Arduino-ESP32 3.3.10 for firmware compilation

```sh
pnpm install --frozen-lockfile
pnpm typecheck
pnpm test
pnpm test:native
pnpm check:docs
pnpm build
pnpm check:generated
```

PlatformIO is provided as a maintainer convenience, but Arduino CLI is the
release compilation reference.

## Architecture rules

Read [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) before changing firmware.

- Portable behavior belongs in `src/core/` and needs host coverage.
- ESP32/Arduino services belong in `src/platform/`.
- Pin numbers and Ethernet initialization belong in `src/boards/`.
- New boards are profiles, manifests, build targets, documentation, and
  evidence—not copies of shared core code.
- GPIO scanning must never wait for network I/O.
- Large or unbounded allocations must not migrate into internal ESP32 heap.
- New configuration fields require defaults, validation, serialization,
  migration consideration, API documentation, and tests.
- Secrets must never be returned by backup/export or written to diagnostics.

Comments should explain constraints, ownership, protocol quirks, and
non-obvious trade-offs. Avoid comments that merely repeat the next statement.

## Generated files

These files are generated and committed for Arduino users:

- `firmware/AdvatekTrigger/src/web/WebAsset.h`
- `generated/AdvatekTrigger-Waveshare-ESP32-S3-ETH.ino`
- `generated/AdvatekTrigger-Waveshare-ESP32-S3-ETH/`
- `generated/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO.ino`
- `generated/AdvatekTrigger-Waveshare-ESP32-S3-ETH-8DI-8RO/`

Change the source under `web/` or `firmware/`, then run `pnpm build`.
`pnpm check:generated` must pass before a pull request.

## Hardware changes

Hardware evidence should identify:

- exact PCB and module revisions;
- Arduino-ESP32 version and build settings;
- PixLite model, firmware, and negotiated API version;
- power method, link mode, and network topology;
- GPIOs and contact circuits exercised;
- memory and stack watermarks;
- expected result, observed result, and reproducible steps.

Do not mark a test passed from visual assumption alone. Record incomplete or
untested gates honestly in [HARDWARE-TESTS.md](HARDWARE-TESTS.md).

Power down before changing wiring. Never connect an externally powered signal
directly to an ESP32 contact pin.

## Pull-request checklist

- [ ] The change is scoped and described in plain language.
- [ ] Tests cover new portable behavior and regressions.
- [ ] User-facing behavior and API changes are documented.
- [ ] Configuration migration and backup compatibility were considered.
- [ ] No credentials, secrets, or private protocol documents are included.
- [ ] `pnpm typecheck`, `pnpm test`, `pnpm test:native`, and
      `pnpm check:docs` pass.
- [ ] Generated artifacts were rebuilt and `pnpm check:generated` passes.
- [ ] Hardware claims include evidence and remaining gates.
- [ ] `CHANGELOG.md` is updated when the change affects users.

By contributing, you agree that your contribution is licensed under the
project's MIT License.
