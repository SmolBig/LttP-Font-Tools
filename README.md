# LttP Font Tools (v2.2)
---

These are a few simple command-line utilities which are essentially a port of Smallhacker's work from https://github.com/Smallhacker/hylian-font.

---

These tools, along with an image editor, can be used to edit the font in the LttP [1.0][J] ROM or (hopefully) any [Rando](http://vt.alttp.run/) ROM based on it. Note that when using rando you need to patch the font after randomizing the ROM.

The extractor will pull the font table out of the ROM and save it as a PNG file.

The injector takes the base ROM and your edited PNG and combines them into a new ROM. Keep in mind that there are only 4 valid colors (including black). The injector will use the nearest palette color to what it finds when recomposing the PNG into data.

The make_ips tool is like the injector, but it generates an IPS patch instead of creating a patched ROM. This should make it easier to distribute font changes.
