# Using the Advatek Labs Contact Trigger

This short guide covers normal software use after the controller has been
flashed, wired, and connected to the same local network as its PixLite Mk3
controllers.

## Open the controller

In a web browser, open:

- `http://advatrigger.local/`, or
- the **Device IP** recorded during setup.

The **Status** section shows the current IP address, local name, Ethernet
state, connected PixLite Mk3 controllers, configured inputs, firmware version, and memory
health. A green connection indicator and **Ready** setup status mean the
controller is ready to receive contact triggers.

If the `.local` address does not open, use the numeric Device IP shown in the
interface or by Serial Monitor at `115200` baud.

![Contact Trigger status dashboard showing the device address, Ethernet health, connected PixLite Mk3, and setup progress](assets/user-guide/01-status-and-pixlite-mk3-annotated.svg)

## Check or add a PixLite Mk3

Open **PixLite Mk3 controllers** to see each saved controller and whether it is online.

1. Select **Discover PixLite Mk3 controllers**.
2. Find the controller by its nickname, IP address, and MAC address.
3. Select **Add** or **Edit**.
4. Enter its Operator password if one is configured, then select
   **Connect and save**.
5. Select **View media** to confirm that its scenes and playlists are listed.

PixLite Mk3 SHOWTime requires a suitable industrial-grade microSD installed in
the PixLite Mk3. If both media lists are empty, check the PixLite Mk3 card and
SHOWTime files before troubleshooting the ESP32 trigger. The ESP32 board's own
TF/microSD slot is not used.
See the
[official SHOWTime guidance](https://www.advateklighting.com/en-us/software/showtime)
for the PixLite Mk3 card requirement.

Use an Administrator account only when an input must activate PixLite Mk3
**Test mode**. Up to 16 PixLite Mk3 controllers can be saved, and each input action can target
a different one.

If a saved PixLite Mk3 changes IP address, **Discover PixLite Mk3 controllers** matches it by MAC
address and updates the connection. If its media list is empty after files
have changed, reconnect it with **Edit** and **Connect and save**, then open
**View media** again.

![Saved PixLite Mk3 controller and alphabetically ordered scenes and playlists](assets/user-guide/02-pixlite-mk3-media-annotated.svg)

## Configure a button or switch

Open **Inputs**, then select **Add input**.

1. Give the input a meaningful name, such as `Entrance button`.
2. Select its wired GPIO or `DI` terminal.
3. Choose **Momentary button** or **Maintained switch**.
4. Leave **Debounce** at `100 ms` initially. Increase it if one physical
   operation occasionally creates two triggers.
5. Configure the action for contact close and contact open.
6. Hold **Hold to test** to send the close action, then release it to send the
   open action. This controls the real PixLite Mk3; use it only when both
   configured actions are safe to run.

Input changes save automatically after a short pause. Wait for **Saved** beside
the input count before closing the page or removing power. **Complete required
fields** means a pin, PixLite Mk3, or media selection is still missing.

For a normally-closed field contact, enable **Normally closed** so the displayed
and triggered states follow its logical operation.

![Input editor showing terminal assignment, switch type, debounce, edge actions, autosave state, and Hold-to-test](assets/user-guide/03-input-configuration-annotated.svg)

## Choose actions

The actions available for either edge are:

| Action | Result |
| --- | --- |
| Play scene / playlist | Plays the selected media once or loops until stopped |
| Next / Previous scene | Steps through that PixLite Mk3's scene list and wraps at the end |
| Live mode | Stops playback and returns the PixLite Mk3 to live input |
| Blank outputs | Blanks playback output |
| Test mode · solid colour | Shows the selected colour; requires Administrator access |
| Test mode · RGB colour fade | Runs the PixLite Mk3 built-in RGB Fade test; requires Administrator access |
| Intensity brighter / darker | A tap changes one step; holding repeats the change |
| Set intensity | Applies the chosen output intensity |
| Release override | Returns intensity control to the PixLite Mk3 |
| No action | Does nothing on that edge |

New playback actions default to **Loop until stopped**. A common momentary
setup is **Play scene** on Press and **Live mode** on Release. A common
maintained setup is a looping scene on Latch on and **Live mode** on Latch off.

The web interface configures intensity behaviour. The connected button or
switch controls intensity during operation.

![Input action configured to step through PixLite Mk3 scenes with a selected playback repeat mode](assets/user-guide/04-scene-stepping-annotated.svg)

![RGB colour-fade Test mode selected as a physical input action](assets/user-guide/05-test-colour-fade-annotated.svg)

![GPIO-driven intensity action showing output target, step size, repeat timing, and fade](assets/user-guide/06-intensity-action-annotated.svg)

## Understand the status LED

- **Steady orange:** the controller is running.
- **White pulse:** a debounced contact edge was accepted.
- **Alternating orange and white during a BOOT hold:** authentication recovery
  is ready; release BOOT between 5 and 14 seconds.
- **Steady white after BOOT is released:** recovery is starting. Wait for blue
  or cyan before connecting to the recovery network.
- **Slow blue pulse:** the temporary Wi-Fi recovery access point is active.
- **Slow cyan pulse:** direct-Ethernet recovery is active.
- **Flashing red after release:** the selected recovery connection could not
  start. The controller returns to steady orange after five seconds.
- **Steady red during a BOOT hold:** factory reset is pending; continue holding
  for 20 seconds to cancel without resetting.

Under **System**, the status LED can be dimmed or switched off. Select
**Apply LED setting** to save the change.

## Network name and address

The current numeric address is always shown in **Status**. Under **Network**,
give each installed controller a unique **Local name**, for example
`foh-trigger`, which becomes:

`http://foh-trigger.local/`

Changing the uplink, local name, or IP settings restarts the controller. Keep a
note of the new address before saving. Select **Save network and restart**
once, then select **Tap again to save and restart**. The button changes to
**Settings accepted - restarting** before the page disconnects.

![Network settings showing the operational connection, editable local name, addressing, and BOOT recovery method](assets/user-guide/07-network-and-recovery-annotated.svg)

Wi-Fi does not automatically fall back to Ethernet if its configured network
is unavailable. To recover access, hold **BOOT for 5–14 seconds** and release:

- If **Wi-Fi AP** is selected under **Network → Advanced network settings**,
  join `Advatek-Trigger-XXXXXX`. A phone should open the captive setup page
  automatically. If it does not, open `http://192.168.4.1/` in the phone's
  full browser.
- If **Direct Ethernet DHCP** is selected, unplug the normal LAN before holding
  BOOT. The controller restarts once and holds the LED white while recovery
  starts. Wait for the LED to pulse cyan, then connect one computer directly
  and open `http://192.168.4.1/`.
  Power the controller separately because a normal computer Ethernet port does
  not provide PoE.

Recovery access lasts 15 minutes. The firmware will not start its recovery DHCP
server while the normal Ethernet link is connected.

## Back up the setup

After commissioning, select **Export redacted backup** under
**Backup and recovery**. Store the downloaded JSON file with the installation
records.

Backups include PixLite Mk3, input, action, and network addressing settings, but
omit all passwords. After importing a backup, re-enter any required Wi-Fi,
PixLite Mk3, or interface passwords.

## If something is not working

1. Check **Status** for network, PixLite Mk3, and input state.
2. Confirm the intended PixLite Mk3 shows online under **PixLite Mk3 controllers**.
3. Select **View media** and confirm the configured file still exists.
4. Check that the input editor says **Saved**.
5. Open **Diagnostics**, select **Refresh log**, and keep the result for a
   support report.

![Diagnostics view showing board identity, firmware versions, Ethernet state, memory health, and the event log](assets/user-guide/08-diagnostics-annotated.svg)

## Use the interface on a phone

The same interface adapts to narrow phone screens. Status, input controls, and
network recovery remain available without a separate app.

### Mobile status

![Mobile status dashboard](assets/user-guide/09-mobile-status-annotated.svg)

### Mobile input editing

![Mobile input editor with full-width Hold-to-test and Remove controls](assets/user-guide/10-mobile-input-annotated.svg)

### Mobile network recovery

![Mobile network and recovery controls](assets/user-guide/11-mobile-network-annotated.svg)

For access recovery and factory-reset instructions, see the
[getting-started recovery section](GETTING-STARTED.md#recovery). For a
reproducible fault report, follow [SUPPORT.md](../SUPPORT.md).
