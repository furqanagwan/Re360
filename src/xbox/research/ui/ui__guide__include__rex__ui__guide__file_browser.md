# File browser: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/file_browser.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 13

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L13)

```text
// Drives, folders and files as rows for a GuideListPage: the console's file
```

## Source note 2, line 14

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L14)

```text
// pickers had no Windows dialog, so the Guide browses the PC itself. The model
```

## Source note 3, line 15

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L15)

```text
// only lists and navigates; the host checks whatever is chosen. Hidden and
```

## Source note 4, line 16

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L16)

```text
// system entries are left out, folders come first, then matching files with
```

## Source note 5, line 17

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L17)

```text
// their sizes; each row's details are its full path.
```

## Source note 6, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L24)

```text
// kChosen: the file, or the folder to use
```

## Source note 7, line 28

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L28)

```text
// `extensions`: the files kFile lists, lower case with the dot (".iso").
```

## Source note 8, line 31

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L31)

```text
// Lists `folder`, or the drives when it is empty, with focus on `focus`
```

## Source note 9, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L32)

```text
// when it is listed.
```

## Source note 10, line 34

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L34)

```text
// A on `row`: opens a drive or folder, or chooses a file (kFile) or the
```

## Source note 11, line 35

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L35)

```text
// open folder's Use This Folder row (kFolder).
```

## Source note 12, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L37)

```text
// B: the parent folder, then the drives, focusing where it came from.
```

## Source note 13, line 38

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L38)

```text
// False at the drives, where B leaves the browser.
```

## Source note 14, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L44)

```text
// The details pane with no rows.
```

## Source note 15, line 47

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L47)

```text
// The top level's drive roots; Open uses fixed, removable and network
```

## Source note 16, line 48

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L48)

```text
// drives unless a host or test replaces it.
```

## Source note 17, line 62

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/file_browser.h#L62)

```text
// "7.3 GB", "512.0 MB" or "12 KB", as the Guide shows sizes.
```
