# Host message boxes and activities

The standalone scene target supplies `MessageBoxScene`,
`ActiveDownloadsScene`, `GuideListPage` and `GuideFileBrowser` without a ReXGlue runtime. A host supplies a legally
obtained skin/options document, resources and sound/input callbacks. No
Microsoft assets ship here. Keep documents alive until their models are
destroyed. Models own their contexts and cannot be moved by value, so retained
element context pointers stay valid; move their `unique_ptr` instead.

Message boxes accept up to four labels, a safe initial choice, disabled action
states, focus and activation. The default visual is XuiMessageBox3. Creation
fails if required controls are absent; a host should keep a fallback. Hidden
skin buttons are explicitly enabled before checking whether they can focus.
A missing/disabled choice never returns an action. The host handles Back,
file picking, guest notifications and shutdown.

Active Downloads uses the console options scene's row template. It displays
up to 11 host snapshots with title, status and details. Updating the list
preserves focus within bounds. Only a snapshot with a cancel callback invokes
cancellation; callbacks must not block the UI thread. Completed records can
stay visible. The complete Guide's `GuideHost::activities` adapter reads
snapshots on the UI thread, alongside title-update jobs. It uses the displayed
snapshot for selection, and reads terminal job state before non-atomic error
strings to avoid racing a title-update worker.

`GuideListPage` is a Guide page outside the running guide: the HUD frame
(`GuideAssets::backdrop`) opened full height with `ClosedToFull`, hosting the
Options scene (`options_notifications`) as a list, as Manage Storage looks in
the in-game Guide. Rows are `btn_Count` clones with left text, right-aligned
secondary text and per-row details for the right pane. Eleven rows show at a
time; `Move` scrolls the window and stops at the ends, and the A/B legends are
set per page. Draw `root()` centred on the 852x480 canvas. The host hides the
legend letters (`HideGuideButtonLetters`) when it draws the glyphs itself.

`GuideFileBrowser` supplies a list page's rows for browsing the PC: drives,
then folders and files matching the host's extensions (hidden and system
entries left out, folders first, sizes on the right, full paths in the
details). A opens a folder or chooses a file, or the open folder in folder
mode; B goes up a level with focus on where it came from, then reports the
drives' level so the host can leave. It only lists and navigates; the host
checks what is chosen.

ReXGlue uses these models for checked first-run source choices and browsing
(`GuideListPage`, `GuideFileBrowser`), media recovery (`MessageBoxScene`) and
copy progress. SDK source validation and file I/O remain outside this repo.
The SDK records copy completion history for the in-game Guide. Painted
interactive/controller/title/media-removal checks remain pending.

Validation on 2026-10-07: standalone Debug and Release build/CTest pass, with
five synthetic cases / 48 assertions. Private locally installed Fuzion Frenzy
Flash checks confirm the three-choice message-box, two-choice recovery and
Active Downloads templates, without copying or distributing assets. SDK UI
checks select the console Disc and Retry controls. Final adapter results are
recorded in ReXGlue's release evidence. Upstream source: extraction pin
3d5ea575a0f6a6ceb9f0c39c097d5c06c59b72aa; this is original component work,
not an imported upstream subsystem. Guide #1 remains open for its owner gates.
