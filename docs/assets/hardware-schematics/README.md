# Hardware schematic assets

The direct-button diagram is provided in three public formats so community
contributors are not locked to one design application.

| Format | Use |
| --- | --- |
| `.drawio` | Canonical editable source; edit free at [diagrams.net](https://app.diagrams.net/) |
| `.svg` | Preferred scalable image for GitHub and web documentation |
| `.png` | Fallback for applications that cannot render SVG |

GitHub documentation embeds the SVG exports and links to the small, plain-text
`.drawio` masters. This makes diagram changes reviewable without requiring
proprietary software.

The editable diagram shows up to eight direct dry-contact buttons for bench or
same-enclosure testing.

All text, blocks, button symbols, and wiring remain editable in the diagrams.net
masters. Recent PowerPoint versions can also insert SVGs and may offer
conversion to editable shapes.

The public guides deliberately avoid manufacturing schematics and
component-level construction. For field wiring, use a complete documented
module selected using
[Off-the-shelf isolated contact inputs](../../PROTECTED-CONTACT-INPUTS.md).

`dongker-8-channel-optocoupler-cutout.png` is a background-removed version of
the real product photograph from the
[DONGKER B08LVXX6MV Amazon UK listing](https://www.amazon.co.uk/dp/B08LVXX6MV).
The generated SVG places project annotations around that photograph without
changing the photographed terminals, markings, or components. The product is
shown as a compatibility example, not a supplier recommendation.

Colours follow the project interface:

- Advatek orange: `#F15A2C`
- charcoal: `#353436`
- near black: `#1F1F20`

When adapting an asset, retain the voltage and isolation warnings.
