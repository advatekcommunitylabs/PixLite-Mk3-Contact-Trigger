# Hardware schematic assets

These diagrams are provided in four formats so community contributors are not
locked to one design application.

| Format | Use |
| --- | --- |
| `.drawio` | Canonical editable source; edit free at [diagrams.net](https://app.diagrams.net/) |
| `.pptx` | Native PowerPoint shapes and connectors; copy or restyle whole slides |
| `.svg` | Preferred scalable image for GitHub and web documentation |
| `.png` | Fallback for applications that cannot render SVG |

GitHub documentation embeds the SVG exports and links to the small, plain-text
`.drawio` masters. This makes diagram changes reviewable without requiring
PowerPoint. The `.pptx` remains an optional convenience copy.

The editable assets contain:

1. Up to eight direct dry-contact buttons for bench or same-enclosure testing.
2. One protected, non-isolated direct-input channel.
3. The recommended group-isolated arrangement for small immersive events.

All text, blocks, button symbols, and wiring in the PowerPoint deck remain
editable. In recent PowerPoint versions, an inserted SVG may also be converted
to shapes, but the supplied `.pptx` is the more reliable PowerPoint editing
route.

The diagrams are deliberately installation overviews rather than manufacturing
schematics. Component values and bring-up checks remain authoritative in
[Protected contact inputs](../../PROTECTED-CONTACT-INPUTS.md).

Colours follow the project interface:

- Advatek orange: `#F15A2C`
- charcoal: `#353436`
- near black: `#1F1F20`

When adapting an asset, preserve the distinction between Waveshare GND and
isolated `ISO COM`. Do not remove the voltage or isolation warnings.
