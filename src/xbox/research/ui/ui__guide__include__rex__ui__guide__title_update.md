# Title update: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/title_update.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L26)

```text
/// The title update the player turned on in the guide; 0 runs the original.
```

## Source note 2, line 28

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L28)

```text
/// Set on the executable a title update choice started, so it never hands back.
```

## Source note 3, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L33)

```text
/// STFS content type of a title update package.
```

## Source note 4, line 36

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L36)

```text
/// What a LIVE/CON/PIRS title update package says about itself.
```

## Source note 5, line 41

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L41)

```text
///< the executable version it updates (XEX version word)
```

## Source note 6, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L42)

```text
/// The content ID stored in the header (0x32C), which Xbox Unity lists as
```

## Source note 7, line 43

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L43)

```text
/// the update's hash: SHA-1 of 0x344 up to the header size rounded to 4 KB.
```

## Source note 8, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L44)

```text
/// That range holds the top hash table's hash, so it covers the whole file.
```

## Source note 9, line 46

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L46)

```text
///< the header region hashes to content_id
```

## Source note 10, line 50

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L50)

```text
/// Reads a package from its first bytes (at least the header, 0xB000 suffices
```

## Source note 11, line 51

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L51)

```text
/// for every title update seen). Nullopt, with `error`, if it isn't one.
```

## Source note 12, line 59

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L59)

```text
/// Whether `package` is the update `expected` describes, for title `title_id`.
```

## Source note 13, line 60

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L60)

```text
/// Empty when it is; otherwise what's wrong, for the player.
```

## Source note 14, line 64

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L64)

```text
/// Where an installed update lives: <local>\title_updates\<version>, holding
```

## Source note 15, line 65

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L65)

```text
/// the package as downloaded (mounted at update: without extracting).
```

## Source note 16, line 67

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L67)

```text
/// The installed package for `version`, or empty.
```

## Source note 17, line 71

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L71)

```text
/// Checks `package_file` and copies it into the update's folder, replacing
```

## Source note 18, line 72

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L72)

```text
/// any earlier copy. Empty on success, else the reason.
```

## Source note 19, line 77

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L77)

```text
/// Progress of a download: bytes so far and the total (0 when unknown).
```

## Source note 20, line 80

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L80)

```text
/// Downloads `url` to `destination`. Empty on success, else the reason.
```

## Source note 21, line 84

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L84)

```text
/// Fetches `url` into memory (small responses). Nullopt with `error` on failure.
```

## Source note 22, line 87

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L87)

```text
/// A place a title update can be downloaded from. `find_url` returns the
```

## Source note 23, line 88

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L88)

```text
/// package URL for `update` of `title_id`, or nullopt with the reason.
```

## Source note 24, line 96

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L96)

```text
/// Xbox Unity: TitleUpdateInfo.php lists a title's updates per media ID with
```

## Source note 25, line 97

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L97)

```text
/// their content IDs ("hash"); the one matching is TitleUpdate.php?tuid=.
```

## Source note 26, line 100

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L100)

```text
/// The TitleUpdateID Xbox Unity gives the update whose hash is `content_id`,
```

## Source note 27, line 101

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L101)

```text
/// from a TitleUpdateInfo.php response. Nullopt when it isn't listed.
```

## Source note 28, line 105

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L105)

```text
/// The sources tried in order: Xbox Unity, then any the title_update_sources
```

## Source note 29, line 106

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L106)

```text
/// setting lists (URL templates with {title_id}, {media_id}, {version},
```

## Source note 30, line 107

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L107)

```text
/// {content_id}).
```

## Source note 31, line 110

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L110)

```text
/// Downloads `update` from the first source that has it and installs it.
```

## Source note 32, line 111

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L111)

```text
/// Each failed source's reason is appended to `errors`. True when installed.
```

## Source note 33, line 118

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L118)

```text
/// A title update download or install from a file, running in the background
```

## Source note 34, line 119

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L119)

```text
/// so it outlives the guide. The guide's Title Updates and Active Downloads read
```

## Source note 35, line 120

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L120)

```text
/// it; the fields are written by its thread.
```

## Source note 36, line 124

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L124)

```text
///< installing a package the player chose
```

## Source note 37, line 129

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L129)

```text
/// Why it failed, one line per source tried; complete once state leaves kRunning.
```

## Source note 38, line 133

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L133)

```text
/// Starts downloading `update` (TitleUpdateSources, in order) unless a job for
```

## Source note 39, line 134

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L134)

```text
/// that version is already running, which is returned instead.
```

## Source note 40, line 138

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L138)

```text
/// Starts installing `package_file`, a package the player chose, as `update`.
```

## Source note 41, line 143

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L143)

```text
/// The latest job for `version`, or null.
```

## Source note 42, line 145

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L145)

```text
/// Every job this run, oldest first.
```

## Source note 43, line 148

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L148)

```text
/// Which executable runs. A title update build is <original>_tu<version>.exe
```

## Source note 44, line 149

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L149)

```text
/// beside the original; the player's choice (`wanted`, 0 for the original)
```

## Source note 45, line 150

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L150)

```text
/// only takes effect when that update is installed and its executable exists,
```

## Source note 46, line 151

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L151)

```text
/// and the original is always the fallback: an update is never required.
```

## Source note 47, line 153

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L153)

```text
///< start `executable` and exit
```

## Source note 48, line 155

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L155)

```text
///< the package this one mounts at update:
```

## Source note 49, line 156

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L156)

```text
///< why, for the log
```

## Source note 50, line 157

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L157)

```text
///< this update build has no update and no original
```

## Source note 51, line 162

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/title_update.h#L162)

```text
/// The executable for `version` beside `exe_path` (built as `built`).
```
