# fgfind

Finds glyphs in specified font. If glyph is absent in preset font shows fonts where it is present.

Usage: `fgfind FONT GLYPHS`

## Examples

```
$ ./fgfind 'DejaVu Sans' abc
Font matches 'DejaVu Sans'
File: /usr/share/fonts/TTF/DejaVuSans.ttf
Family: DejaVu Sans
Style: Book

All glyphs are present in preset font.
```

```
$ ./fgfind Inconsolata ✚●
Font matches 'Inconsolata'
File: /usr/share/fonts/TTF/Inconsolata-Regular.ttf
Family: Inconsolata
Style: Regular

Glyph ✚ is absent in preset font.
It is present in fonts:
-----
File: /usr/share/fonts/TTF/DejaVuSansCondensed.ttf
Family: DejaVu Sans
Style: Condensed
-----
File: /usr/share/fonts/TTF/DejaVuSans-BoldOblique.ttf
Family: DejaVu Sans
Style: Bold Oblique
...
```
