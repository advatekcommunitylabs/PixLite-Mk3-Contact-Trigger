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

## Check or add a PixLite Mk3

Open **PixLite Mk3 controllers** to see each saved controller and whether it is online.

1. Select **Discover PixLite Mk3 controllers**.
2. Find the controller by its nickname, IP address, and MAC address.
3. Select **Add** or **Edit**.
4. Enter its Operator password if one is configured, then select
   **Connect and save**.
5. Select **View media** to confirm that its scenes and playlists are listed.

Use an Administrator account only when an input must activate PixLite Mk3
**Test mode**. Up to 16 PixLite Mk3 controllers can be saved, and each input action can target
a different one.

If a saved PixLite Mk3 changes IP address, **Discover PixLite Mk3 controllers** matches it by MAC
address and updates the connection. If its media list is empty after files
have changed, reconnect it with **Edit** and **Connect and save**, then open
**View media** again.

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

The web interface configures intensity behaviour; normal intensity changes are
then driven by the connected button or switch rather than an on-screen slider.

## Understand the status LED

- **Steady orange:** the controller is running.
- **White pulse:** a debounced contact edge was accepted.
- **Flashing during a BOOT hold:** authentication recovery is ready.
- **Red during a BOOT hold:** factory reset is pending.

Under **System**, the status LED can be dimmed or switched off. Select
**Apply LED setting** to save the change.

## Network name and address

The current numeric address is always shown in **Status**. Under **Network**,
give each installed controller a unique **Local name**, for example
`foh-trigger`, which becomes:

`http://foh-trigger.local/`

Changing the uplink, local name, or IP settings restarts the controller. Keep a
note of the new address before selecting **Save network and restart**.

Wi-Fi does not automatically fall back to Ethernet if its configured network
is unavailable. To recover access, hold **BOOT for 5–14 seconds** and release:

- If **Wi-Fi AP** is selected under **Network → Advanced network settings**,
  join `Advatek-Trigger-XXXXXX` and open `http://192.168.4.1/`.
- If **Direct Ethernet DHCP** is selected, unplug the normal LAN before holding
  BOOT, then connect one computer directly and open `http://192.168.4.1/`.
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

For access recovery and factory-reset instructions, see the
[getting-started recovery section](GETTING-STARTED.md#recovery). For a
reproducible fault report, follow [SUPPORT.md](../SUPPORT.md).
