# Guest code patches

A title's codegen config can change guest instructions before they are
recompiled. This is how a community patch, such as one from Xenia Canary's
[game-patches](https://github.com/xenia-canary/game-patches) repository,
reaches a recompiled title. It follows
[ADR-009](adr/ADR-009-title-compatibility-profiles.md) section 7: code changes
live in the codegen config and are compiled in, never applied at runtime.

## Format

The entry has the same shape as a Canary `.patch.toml` entry, so one can be
copied across:

```toml
[[patch]]
name = "Unlock FPS"
enabled = false            # Canary's is_enabled is accepted too; default true
[[patch.be32]]
address = 0x8229B808
value = 0x60000000         # nop
[[patch.be8]]
address = 0x820F2283
value = 0x01
```

- `be8`, `be16`, `be32` and `be64` write a big-endian value of that width.
- Patches are keyed by `name`. A later file with the same name replaces the
  writes it lists and the `enabled` flag it sets. A title config can therefore
  ship a patch switched off, and a small file that includes it turns it on:

  ```toml
  includes = ["title.toml"]
  [[patch]]
  name = "Unlock FPS"
  enabled = true
  ```

## Rules

- **Code only.** Every write must lie inside one executable section. The
  running title loads the original image into guest memory, so a patch to
  data would never be seen; codegen refuses it.
- **Checked before anything is written.** A malformed enabled patch, one that
  reaches outside the code, or two enabled patches writing the same byte stop
  codegen with an error naming the patch. Disabled patches are not checked.
- **Applied first.** Patches are written into codegen's copy of the image
  before instructions are decoded, so discovery, analysis and the generated
  C++ all see the patched code. The XEX on disk is never changed.
- **Recorded.** Codegen logs each patch as applied or disabled. The names are
  compiled into the title (`PPCImageInfo::code_patches`) and logged at startup
  as `Guest code patches compiled in: ...`. Changing a patch changes the config,
  which regenerates the code.

## Switchable patches

`switchable = true` compiles both versions in, so the player can turn the
patch on and off while the title runs (the Xbox guide's Settings > Patches or
Mods, [xbox-guide.md](xbox-guide.md#settings-pages)):

```toml
[[patch]]
name = "Unlock FPS"
enabled = false            # the state at first start
switchable = true
category = "patch"         # or "mod": which guide page lists it ("cheat" reads as "mod")
```

- The image is left original. Each word the patch changes is emitted as
  `if (REX_PATCH_ACTIVE(i)) { patched } else { original }`, reading a flag in
  `g_rex_patch_active[]`. The patches are listed in
  `PPCImageInfo::switchable_patches` (name, flag, category). A write of the
  bytes already there switches nothing and emits no branch.
- **Register sets.** A switchable patch can also set a register just before an
  instruction runs, as a trainer's detour does, and may test the link register
  so it applies only after one call:

  ```toml
  [[patch.set]]
  address = 0x82233698     # before stw r11,380(r31): the new health
  register = "r11"
  value = 30000
  lr = 0x82233674          # only when that call was the last one made
  ```

  Codegen emits `if (REX_PATCH_ACTIVE(i) && ctx.lr == lr) r11 = value;` there.
  `lr` needs the link register kept (not `skip_lr`). Sets are for switchable
  patches only, and a switchable patch may consist of sets alone.

  `when_nonzero` narrows a set further. Each entry is a register (`"r26"`) or
  the big-endian word at a register plus an offset (`"r26+316"`, read only
  when the register is non-zero), and all must be non-zero. Quantum of
  Solace's One-Hit Kills uses it to kill only when the attacker is the player
  (`r26`, the attacker, has a client at +316). The trainer's unconditional
  version also killed the scripted bomber when a van shoved him at the start
  of Mission 6, and the opening cutscene crashed looking for him
  ([RG-007-015](https://github.com/furqanagwan/007/blob/main/docs/RG-007-015.md)).

  ```toml
  [[patch.set]]
  address = 0x82233698
  register = "r11"
  value = 0
  lr = 0x82233484
  when_nonzero = ["r26", "r26+316"]
  ```
- **No control flow.** Analysis sees only the original code, so a switchable
  patch may not change or introduce a branch, call, return, trap or `sc`
  (primary opcodes 3, 16, 17, 18, 19 and `tw`); codegen refuses it. Make such a
  patch fixed instead.
- The player's choices are saved by name in the `code_patch_states` cvar
  (`Name=1;Name=0`) and applied at startup; a patch it does not name starts
  in its `enabled` state.

## Title cheat codes

`[[cheat]]` lists a cheat the title's developers built in: a code the
player types into the game's own menu. Nothing is patched; the Xbox guide's
Settings > Cheats lists the codes so a player need not look them up
([xbox-guide.md](xbox-guide.md#settings-pages)).

```toml
[[cheat]]
name = "007 Pack"
code = "g3tb0nd"
description = "The Walther PPK in single-player and multiplayer."
where = "Extras > Cheat Codes"
```

`name` and `code` are required; a later entry with the same name replaces the
earlier one. Codegen compiles them into `PPCImageInfo::title_cheats`. Record
where each code came from next to it.

## Title add-ons

`[[dlc]]` lists an add-on the title had in the Xbox 360 marketplace, by its
marketplace media ID. The Xbox guide's Games & Apps > Manage Game lists each one
with its catalogue name, publisher, description and banner, and installs it
from a package on this PC ([xbox-guide.md](xbox-guide.md#what-it-does)).

```toml
[[dlc]]
id = "D4C83E1F-243B-4A68-AD70-ABF3C0FBF372"  # SKYFALL Content Pack
# requires_title_update = 2       # the title update version it needs
# package_name = "SKYFALL Pack"   # the display name in its package, if different
```

`id` is required and compared without case; a later entry with the same id
replaces the earlier one. Set `requires_title_update` only when it is
established: the guide then refuses to install the add-on until a build
carries that title update. The guide matches installed and found packages to
entries by display name (`package_name`, else the catalogue's title).

`rexglue dlc-find <title ID>` prints a title's entries. The catalogue
(`catalog.xboxlive.com`, `FindGames`, add-ons are media type 18) has no filter
by game, so it pages through every add-on, about 28,600 in two to three
seconds. At build time `rexglue dlc-catalog` (run by `rexglue_configure_target`
from the `REXGLUE_TITLE_DLC_IDS` codegen writes) fetches only the listed IDs
with their banner and tile in the builder's language (`REXGLUE_DLC_LOCALE`,
default the PC's) and the executable embeds them. The file is kept until the
list or locale changes. Offline, the catalogue holds the IDs only, the build
goes on, and the guide names add-ons by their packages. Nothing fetched is
committed.

A patch is title-specific behavior (ADR-005). It belongs in the title's own
repository and config, never in the SDK. Record where it came from, its author
and any known problems next to it, as Canary does.

## Tests

`unit_tests [guide][dlc]`: the add-on catalogue reads back what was written,
including an entry with the ID only, and a truncated, mislabelled or
over-counted one is refused. `[codegen][patch]` also covers `[[dlc]]` (ids
compared without case, a later entry replacing an earlier one).

`unit_tests [codegen][patch]`: widths and byte order, Canary's `is_enabled`,
bad entries, an including file switching a patch on, writes into code only,
refusals for data, out-of-image and past-the-section writes, overlaps, and
patching before decoding; switchable patches keeping the image original,
listing both words, skipping unchanged words and refusing branches; register
sets; `[[cheat]]` entries keyed by name.
