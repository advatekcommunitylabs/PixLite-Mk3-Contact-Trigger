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

All text, blocks, button symbols, and wiring remain editable in the diagrams.net
masters. Recent PowerPoint versions can also insert SVGs and may offer
conversion to editable shapes.

The public guides deliberately avoid manufacturing schematics and
component-level construction. For field wiring, use a complete documented
module selected using
[Off-the-shelf isolated contact inputs](../../PROTECTED-CONTACT-INPUTS.md).

`off-the-shelf-8-channel-optocoupler.png` is a generated, supplier-neutral
category illustration. It is not a wiring reference, product recommendation,
or representation of a specific purchasable model.

Colours follow the project interface:

- Advatek orange: `#F15A2C`
- charcoal: `#353436`
- near black: `#1F1F20`

When adapting an asset, retain the voltage and isolation warnings.
