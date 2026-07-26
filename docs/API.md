# Local ESP32 API

Base URL: `http://<device>/api`. The interface is local HTTP and does not offer
TLS. JSON is UTF-8. When UI protection is enabled, send the password in
`X-Advatek-Password`; a missing or invalid value returns `401`.

## Reads

| Method | Path | Purpose |
| --- | --- | --- |
| GET | `/state` | Versions, board/uplink/AP state, PixLite state, inputs, Ethernet and memory diagnostics |
| GET | `/inputs` | Lightweight live input states and debounced edge counters for the SPA activity indicators |
| GET | `/config` | Configuration with secrets omitted |
| GET | `/devices` | Last ADAR result and truncation flag |
| GET | `/media?targetId=<id>` | Separate `scenes` and `playlists` arrays for one saved PixLite |
| GET | `/log` | In-memory diagnostic ring |
| GET | `/config/export` | Downloadable, versioned, redacted backup |

## Mutations

| Method | Path | Purpose |
| --- | --- | --- |
| POST | `/discover` | Run ADAR v1.1 multicast then broadcast discovery |
| POST | `/connect` | Add/update a target by `targetId`, `mac`, or `host`, authenticate, negotiate and load media |
| POST | `/pixlites/remove` | Forget one target after its input actions have been retargeted |
| POST | `/action/test` | Execute one action-shaped JSON object |
| POST | `/config` | Apply and atomically save a complete or partial configuration |
| POST | `/config/import` | Import a redacted backup; omitted secrets are retained |
| POST | `/network/apply` | Save network configuration and restart |
| POST | `/factory-reset` | Erase configuration; body must contain `FACTORY RESET` |

The network object uses `recoveryConnection: "wifi"` or `"ethernet"`.
Ethernet recovery is a 15-minute DHCP service at `192.168.4.1` and is refused
when the W5500 already has link.

`/connect` accepts `targetId`, `name`, `mac`, `host`, `username` (`oper` or
explicit `admin`) and `password`. A known ID, MAC, or host updates an existing
entry; otherwise it adds one, up to the fixed limit of 16. Discovery updates a
known MAC to its latest DHCP address.

Action objects address a saved PixLite by its stable `targetId`. The reserved
legacy value `primary` resolves to the configured primary target:

```json
{
  "kind": "playScene",
  "targetId": "primary",
  "mediaName": "Example.scn",
  "repeat": "forever",
  "output": "pixels",
  "auxOutput": 1,
  "intensityPercent": 100,
  "stepPercent": 5,
  "holdDelayMs": 400,
  "repeatIntervalMs": 250,
  "fadeMs": 250,
  "testColor": "#ff0000",
  "testRed": 255,
  "testGreen": 0,
  "testBlue": 0
}
```

Action kinds: `none`, `playScene`, `playPlaylist`, `nextScene`,
`previousScene`, `stop`, `blank`, `brighter`, `darker`, `setIntensity`,
`releaseIntensity`, and `testColor`.
New actions default to `"repeat": "forever"` (shown as **Loop until stopped**);
use `"once"` when the selected media should play only once.
`nextScene` and `previousScene` use the selected PixLite's cached `.scn` list,
skip playlists, and wrap at either end. With no currently playing scene,
forward starts at the first scene and backward starts at the last scene. The
cache refreshes from each configured PixLite every 60 seconds.
`stop` explicitly returns the PixLite to Live mode. `testColor` sends a global
8-bit RGB custom colour and remains active until another mode action runs.
PixLite Mk3 requires the saved target to use its Administrator account for
Test mode; an Operator request is rejected as unauthorized.

Backups omit Wi-Fi, PixLite, recovery, and web-interface credentials.
Importing on the same device retains existing omitted secrets.
An imported GPIO that is unavailable or duplicated on the active board is
rejected with a remapping error.

Configuration schema v5 separates portable input behavior from
`hardwareBinding` (`boardId`, `profileVersion`, and eight `inputGpios`). A v1
through v4 NVS record is migrated automatically. Schema v5 stores at most 16
PixLites, preserves old `primary` actions, and adds the action `testColor`.
Legacy GPIO33/34 bindings become unassigned and must be explicitly remapped.
`statusLed` contains persistent `enabled` and `brightnessPercent` settings.

`/state` includes the active `hostname` and numeric `ip`. The first-run
hostname is `advatrigger`, advertised as `http://advatrigger.local/` through
mDNS. Network configuration accepts a 1-31 character hostname label containing
letters, numbers and interior hyphens; the `.local` suffix is added by clients.

`/state.memory` reports flash size, current/minimum internal heap, largest
internal block, total/free PSRAM, network-task stack watermark, reset reason,
and uptime. `/state.ethernet` reports W5500 initialization attempts, link,
DHCP/static selection, link speed, duplex, and the last initialization failure.
Both `/state.inputs` and `/inputs` include `active` and the monotonically
increasing `eventSequence`. Clients can use the sequence difference to retain
make and break events that occur between HTTP polls.

Configuration request bodies are capped at 12 KB and PixLite responses at
32 KB. Config, device, media, and log reads use streamed HTTP responses.
