# Public hardware change validation log

This log records validation for **hardware-affecting modifications merged after
the initial public beta** so contributors can see what changed, what was
physically retested, and what remains unknown. Supported targets and release
compatibility are identified in [`compatibility.json`](compatibility.json).

## What belongs in this log

Add an entry when a public contribution changes or adds any of the following:

- a board profile, supported product, pin assignment, or reserved-pin rule;
- Ethernet, Wi-Fi, power-up, reset, recovery, or status-LED behaviour;
- contact-input polarity, debounce, input scanning, or physical feedback;
- flash, PSRAM, internal-memory, task-stack, or partition requirements;
- configuration migration that changes a hardware binding;
- a generated Arduino artifact in a way that can affect real hardware; or
- a hardware compatibility claim in public documentation.

Documentation-only edits, wording changes, and software changes with no
hardware-facing effect do not need an entry.

## Required entry format

Create one dated section per merged hardware-affecting change:

```md
## YYYY-MM-DD: Short change name

- Release or commit:
- Pull request:
- Contributor:
- Affected board profile(s):
- Hardware used:
- PixLite Mk3 model, firmware, and API version:
- Power and network arrangement:
- Change and hardware risk:
- Physical tests completed:
- Result: Pass / Partial / Fail
- Measurements or diagnostic evidence:
- Known limitations and tests not completed:
```

Redact credentials, full device MAC addresses, private network details, and
machine-specific paths. Link to public issue or pull-request evidence where
useful.

## Retest expectations

Test in proportion to the change. A profile or networking modification normally
requires:

- a clean compile and USB upload using the documented Arduino settings;
- boot-banner confirmation of the intended board profile, flash, and PSRAM;
- Ethernet initialization, link, DHCP, and cable-recovery checks;
- PixLite Mk3 discovery, connection, media refresh, and one harmless action;
- every affected physical input, including open, closed, and boot-held states;
- status-LED and BOOT-recovery checks when those paths are affected;
- configuration persistence and migration checks when bindings change; and
- memory and task-stack readings when runtime allocation or task code changes.

New board profiles require the complete porting checklist in
[`PORTING.md`](PORTING.md).

## Status vocabulary

- **Pass:** the stated physical tests passed on the identified hardware.
- **Partial:** useful testing passed, but named checks remain outstanding.
- **Fail:** the modification did not meet its stated hardware expectation.

Do not turn a partial result into a broad compatibility claim. Record both the
successful scope and the untested scope.

## Post-public entries

No post-public hardware-affecting modifications have been recorded yet.

## Support boundary

This Advatek Labs community validation log records project testing. It provides
no warranty or certification. Advatek Technical Support does not cover
third-party hardware or Advatek Labs community projects. Integrators remain responsible
for manufacturer instructions, local electrical codes, applicable standards,
and qualified installation where required.
