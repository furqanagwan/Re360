# Title repository standard

How a repository for recompiled games (a *title repository*) is laid out and
what its README says. One repository can hold one game or a series, like
[furqanagwan/007](https://github.com/furqanagwan/007) (Quantum of Solace,
Blood Stone, 007 Legends). The SDK stays generic: title-specific
configuration, investigation notes and evidence live in the title repository,
and fixes that help every title go to the SDK.

## Rules

- **Never commit game material.** No disc images, extracted files, XEX or
  STFS content, generated C++, builds, saves, logs, screenshots or captures of
  the game. The `.gitignore` is an allowlist (ignore `/*`, then un-ignore each
  tracked file), so new files stay private until they're deliberately added.
- **No publisher artwork in the repository.** Box art, banners, game icons
  and logos are the publisher's. The repository's own artwork in `assets/` is
  original. A game's README may show its marketplace banner by linking to a
  public database that hosts it (such as
  [x360db](https://github.com/xenia-manager/x360db)), credited, never by
  committing the image. One exception: each game's title icon (its XDBF
  title image, the icon its window and taskbar button show) may be kept in
  `assets/icons/` so the README can show the games side by side, since a
  title's own icon is often not hosted anywhere; the README's Legal section
  says so. Library tiles made with
  [`rexglue library-art`](library-art.md) contain the box art, so they stay
  local too.
- **Identify every release exactly**, from the disc and the XEX headers, never
  from memory: the Redump name of the image, region, languages, title ID, media
  ID, executable version, disc number and title updates. Record "not yet
  identified" until the material is in hand.
- **Status claims need evidence**: a linked record (`docs/<ID>.md`) naming the
  SDK commit, the build, the hardware and what was and wasn't tested.
- **Commit identity**: the maintainer's GitHub no-reply address only, never a
  work address, as for the SDK.

## Layout

```text
README.md                 The series README (below): games, download, how to play
.gitignore                Allowlist; everything else stays local
<Title>/
  README.md               The game's page (below)
  <title>.toml            Its codegen configuration (function entries, hooks,
                          patches, mods, cheats, add-ons), added to the private
                          manifest's `includes` as "../<title>.toml"
  shader_replacements/    Optional hand-written replacement shaders
                          (<HASH>[_<MOD>].<vs|ps_rtv|ps_rov>.hlsl), see
                          [shader replacements](shader-replacements.md); HLSL
                          only, never DXBC dumped from the game
assets/
  logo.svg                Original square logo
  social-preview.svg/.png Original 1280x640 image for Settings → Social preview
docs/
  README.md               Notes for contributors
  building.md             How to build each game from source
  <PREFIX>-NNN.md         One record per issue: goal, evidence, result
```

Private work (disc images, the `rexglue init` project, logs) sits in the
game's folder next to its README and `.toml`, and is ignored. A title's
configuration is named after the game only: variants (a frame-rate patch, a
mod) are switchable patches inside it, not separate files.

## Series README template

What a player needs, in this order: what the games are, where to download,
how to play. Build steps go in a building page in the title repository's
`docs/`.

````markdown
<p align="center"><a href="<Title>/README.md"><img src="assets/icons/<title>.png" width="96" alt="<Title>"></a> ...one per game</p>

# <Series or title> — Xbox 360 recompilation

<One sentence: what these games are and that they're statically recompiled for
Windows PC with ReXGlue.>

> [!IMPORTANT]
> This repository contains no game files. You need your own legally obtained
> copy of each game.

## The games

| Game | Released | Status |
| --- | --- | --- |
| <Title>, linking to `<Title>/README.md` | <year> · <developer> | <status> |

Status is one of: **Planned** (not started), **Investigating** (recompiles,
doesn't reach gameplay yet), **In-game** (reaches gameplay, not yet validated
end to end), **Playable** (validated through the game, with the limits listed).

## Download

<The Releases page, or that none is published yet and where the build steps are.>

## How to play

<What a player needs (Windows, GPU, their own copy), how to start the game,
and how to open XboxUI.>

## Issues and records

## Repository layout

## Legal

Trademarks and copyrights belong to their owners; the project isn't
affiliated with or endorsed by them.

## Credits
````

## Game page template

````markdown
<p align="center"><img src="<linked marketplace banner>" alt="<Title> marketplace banner"></p>

# <Title>

<One sentence: the game, recompiled with ReXGlue. A short overview in your own
words.>

**Status: <status>.** <One line on how far it gets.>

## The game

Release date, developer, publisher, genre, players, rating: collated from
public databases (LaunchBox, x360db) and named under Sources. Where sources
differ, use the more specific one and say so in the record.

## The release this is built from

Redump name, region, languages, title ID, media ID, executable version, XEX
SHA-256, title updates: from the disc and the XEX headers.

## What works

What works, what hasn't been tested and what's known broken, each with a link
to its record in `docs/`.

## Patches and mods / Cheat codes / Add-ons

What the title's `.toml` gives XboxUI, as lists.

## Configuration

## Sources
````

## Why

- **Allowlist `.gitignore`:** a title folder holds gigabytes of private
  material next to the few files worth sharing. An allowlist makes adding game
  material a deliberate act, not an accident.
- **Exact release identity:** recompiled code is only valid for the XEX it was
  generated from; the same title ID can cover several builds.
- **Status levels:** they keep "it boots" from reading as "it's playable".
