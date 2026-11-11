# System update: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/include/rex/ui/xui/system_update.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 30

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L30)

```text
/// The named resources of an unencrypted XEX2 image (uncompressed, basic or
```

## Source note 2, line 31

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L31)

```text
/// LZX-compressed), as the console's system XEXs are.
```

## Source note 3, line 35

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L35)

```text
/// XUIZ packages from the console's system XEXs, keyed "module/resource":
```

## Source note 4, line 36

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L36)

```text
/// "hud/hud", "huduiskin/skin", "xam/shrdres", "gamerprofile/gp"...
```

## Source note 5, line 39

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L39)

```text
/// The system XEXs the guide reads, as they are named in the update package;
```

## Source note 6, line 40

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L40)

```text
/// vk is the on-screen keyboard (RG-GDK-059).
```

## Source note 7, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L42)

```text
/// The console's system fonts (.xtt) the guide's text uses, when present:
```

## Source note 8, line 43

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L43)

```text
/// the Latin and Japanese/Korean font of the PC backward-compatibility
```

## Source note 9, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L44)

```text
/// files, the update's light Segoe and the Chinese ones.
```

## Source note 10, line 48

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L48)

```text
/// The system XEXs by module name ("hud" -> `$flash_hud.xex` bytes), and
```

## Source note 11, line 49

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L49)

```text
/// the fonts as "font/<name>" -> .xtt bytes.
```

## Source note 12, line 52

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L52)

```text
/// `path` is a `$SystemUpdate` folder, the `su20076000_00000000` package in
```

## Source note 13, line 53

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L53)

```text
/// it, or a folder holding the `$flash_<module>.xex` (or `<module>.xex`)
```

## Source note 14, line 54

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L54)

```text
/// files, such as the Flash folder of an Xbox PC backward-compatibility game.
```

## Source note 15, line 57

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L57)

```text
/// Reads the system XEXs the guide needs from `path` (as for Load).
```

## Source note 16, line 59

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L59)

```text
/// Reads several sources; a module or font comes from the first that has it.
```

## Source note 17, line 62

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L62)

```text
/// Builds the update from system XEXs; fails when a package the guide needs
```

## Source note 18, line 63

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L63)

```text
/// is missing.
```

## Source note 19, line 66

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L66)

```text
/// The guide bundle: the system XEXs in one blob, which the title build
```

## Source note 20, line 67

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L67)

```text
/// embeds into the executable (rexglue guide-bundle, rexglue_configure_target)
```

## Source note 21, line 68

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L68)

```text
/// so the guide needs nothing at run time.
```

## Source note 22, line 72

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L72)

```text
/// Adds the XUIZ resources of one system XEX. Used by Load and by tests.
```

## Source note 23, line 75

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L75)

```text
/// Converts a "font/<name>" module to TrueType. Used by FromModules and tests.
```

## Source note 24, line 80

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/system_update.h#L80)

```text
/// A system font as TrueType ("xenonjklatin"), or empty.
```
