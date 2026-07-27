# Hardware schematic assets

These diagrams are provided in three public formats so community contributors are not
locked to one design application.

| Format | Use |
| --- | --- |
| `.drawio` | Canonical editable source; edit free at [diagrams.net](https://app.diagrams.net/) |
| `.svg` | Preferred scalable image for GitHub and web documentation |
| `.png` | Fallback for applications that cannot render SVG |

GitHub documentation embeds the SVG exports and links to the small, plain-text
`.drawio` masters. This makes diagram changes reviewable without requiring
proprietary software.

The editable assets contain:

1. Up to eight direct dry-contact buttons for bench or same-enclosure testing.
2. One protected, non-isolated direct-input channel.
3. An example group-isolated arrangement for small immersive events.

All text, blocks, button symbols, and wiring remain editable in the diagrams.net
masters. Recent PowerPoint versions can also insert SVGs and may offer
conversion to editable shapes.

The diagrams are deliberately installation overviews rather than manufacturing
schematics. Component values and bring-up checks remain authoritative in
[Protected contact inputs](../../PROTECTED-CONTACT-INPUTS.md).

Colours follow the project interface:

- Advatek orange: `#F15A2C`
- charcoal: `#353436`
- near black: `#1F1F20`

When adapting an asset, preserve the distinction between Waveshare GND and
isolated `ISO COM`. Do not remove the voltage or isolation warnings.
