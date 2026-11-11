// Copyright (c) 2026 ReXGlue contributors. BSD 3-Clause License; see LICENSE.
#include <rex/ui/overlay/game_source.h>
#include <rex/ui/guide/file_browser.h>
#include <rex/ui/guide/guide_list_page.h>
#include <rex/ui/guide/message_box.h>
#include <algorithm>
#include <cmath>

#include <chrono>
#include <cstring>
#include <fmt/format.h>
#include <imgui.h>
#include <rex/cvar.h>
#include <rex/string.h>
#include <rex/filesystem/devices/optical_disc_reader.h>
#include <windows.h>
#include <shobjidl.h>
#include <wrl/client.h>

namespace rex::ui {
namespace {

constexpr ImU32 kGuideDim = IM_COL32(0, 0, 0, 255);
std::string Utf8(const std::filesystem::path& path) {
  return rex::string::to_utf8(path.u16string());
}

}

class GameSourceConsoleBox {
 public:
  static std::unique_ptr<GameSourceConsoleBox> Create(const GameSourceVisualsProvider& provider,
                                                      std::string title, std::string body,
                                                      std::span<const std::string> choices,
                                                      size_t initial = 0) {
    if (!provider)
      return nullptr;
    auto visuals = provider();
    if (!visuals || !visuals->assets)
      return nullptr;
    auto panel = std::unique_ptr<GameSourceConsoleBox>(new GameSourceConsoleBox);
    panel->visuals_ = std::move(*visuals);
    xui::SceneContext context;
    context.skin = &panel->visuals_.assets->skin;
    context.package = "huduiskin";
    context.play_sound = panel->visuals_.play_sound;
    panel->box_ = guide::MessageBoxScene::Create(context, std::move(title), std::move(body),
                                                 choices, initial);
    if (!panel->box_)
      return nullptr;
    panel->count_ = choices.size();
    guide::HideGuideButtonLetters(&panel->box_->root());
    panel->box_->root().Set("Position", xui::Value{xui::Vec3{}});
    const float width = panel->box_->root().width(), height = panel->box_->root().height();
    if (!std::isfinite(width) || !std::isfinite(height) || width <= 0 || height <= 0 ||
        width > 4096 || height > 4096)
      return nullptr;
    return panel;
  }
  std::optional<size_t> Draw(ImGuiIO& io, bool enabled) {
    auto& root = box_->root();
    const ImVec2 available = ImGui::GetContentRegionAvail();
    const auto cursor = ImGui::GetCursorScreenPos();
    const float scale = std::min(available.x / root.width(), available.y / root.height());
    const ImVec2 origin(cursor.x + (available.x - root.width() * scale) * 0.5f,
                        cursor.y + (available.y - root.height() * scale) * 0.5f);
    std::optional<size_t> chosen;
    const size_t initial = box_->focused_choice();
    ImGui::PushID("Console choices");
    for (size_t i = 0; i < count_; ++i) {
      box_->SetEnabled(i, enabled);
      const auto id = "Button" + std::to_string(i);
      auto* button = root.FindById(id);
      xui::Vec3 position{};
      for (auto* element = button; element && element != &root; element = element->parent()) {
        const auto p = element->position();
        position.x += p.x;
        position.y += p.y;
      }
      ImGui::SetCursorScreenPos(
          ImVec2(origin.x + position.x * scale, origin.y + position.y * scale));
      const ImVec2 size(std::max(1.0f, button->width() * scale),
                        std::max(1.0f, button->height() * scale));
      if (first_ && initial == i)
        ImGui::SetKeyboardFocusHere();
      const bool pressed = ImGui::InvisibleButton(id.c_str(), size, ImGuiButtonFlags_EnableNav);
      if (ImGui::IsItemFocused() || ImGui::IsItemHovered() || pressed)
        box_->Focus(i);
      if (pressed && enabled)
        chosen = box_->Activate();
      if (first_ && initial == i)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::PopID();
    if (first_)
      box_->Focus(initial);
    first_ = false;
    root.Advance(std::clamp(double(io.DeltaTime), 0.0, 0.25) * xui::kFramesPerSecond);
    xui::Render(ImGui::GetWindowDrawList(), root, origin, scale, 1, visuals_.resources);
    ImGui::SetCursorScreenPos(origin);
    ImGui::Dummy(ImVec2(root.width() * scale, root.height() * scale));
    return chosen;
  }

 private:
  GameSourceVisuals visuals_;
  std::unique_ptr<guide::MessageBoxScene> box_;
  size_t count_ = 0;
  bool first_ = true;
};

class GameSourceGuidePage {
 public:
  static std::unique_ptr<GameSourceGuidePage> Create(GameSourceVisuals visuals) {
    if (!visuals.assets || !visuals.assets->has_options)
      return nullptr;
    auto panel = std::unique_ptr<GameSourceGuidePage>(new GameSourceGuidePage);
    panel->visuals_ = std::move(visuals);
    const auto& assets = *panel->visuals_.assets;
    xui::SceneContext frame, options;
    for (auto [context, package] : {std::pair{&frame, "xam/xam"}, std::pair{&options, "hud/hud"}}) {
      context->skin = &assets.skin;
      context->package = package;
      context->play_sound = panel->visuals_.play_sound;
    }
    panel->page_ =
        guide::GuideListPage::Create(assets.backdrop, frame, assets.options_notifications, options);
    if (!panel->page_)
      return nullptr;
    guide::HideGuideButtonLetters(&panel->page_->root());
    return panel;
  }
  guide::GuideListPage& page() { return *page_; }
  const GameSourceVisuals& visuals() const { return visuals_; }

  std::optional<size_t> Draw(ImGuiIO& io, bool enabled) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float scale = std::min(viewport->Size.x / guide::GuideListPage::kSceneWidth,
                                 viewport->Size.y / guide::GuideListPage::kSceneHeight);
    const ImVec2 origin(
        viewport->Pos.x + (viewport->Size.x - guide::GuideListPage::kSceneWidth * scale) * 0.5f,
        viewport->Pos.y + (viewport->Size.y - guide::GuideListPage::kSceneHeight * scale) * 0.5f);
    auto& root = page_->root();
    std::optional<size_t> chosen;
    ImGui::PushID("Console choices");
    for (size_t i = 0; i < guide::GuideListPage::kVisibleRows; ++i) {
      auto* row = page_->slot(i);
      if (!row)
        continue;
      xui::Vec3 position{};
      for (auto* element = row; element && element != &root; element = element->parent()) {
        const auto p = element->position();
        position.x += p.x;
        position.y += p.y;
      }
      const size_t index = page_->first_visible() + i;
      ImGui::SetCursorScreenPos(
          ImVec2(origin.x + position.x * scale, origin.y + position.y * scale));
      const bool pressed = ImGui::InvisibleButton(
          ("Button" + std::to_string(index)).c_str(),
          ImVec2(std::max(1.0f, row->width() * scale), std::max(1.0f, row->height() * scale)));

      const bool pointed = ImGui::IsItemHovered() && (io.MouseDelta.x != 0 || io.MouseDelta.y != 0);
      if (pointed || pressed)
        page_->Focus(index);
      if (pressed && enabled)
        chosen = page_->Activate();
    }
    ImGui::PopID();
    root.Advance(std::clamp(double(io.DeltaTime), 0.0, 0.25) * xui::kFramesPerSecond);
    xui::Render(ImGui::GetWindowDrawList(), root, origin, scale, 1, visuals_.resources);
    return chosen;
  }

 private:
  GameSourceVisuals visuals_;
  std::unique_ptr<guide::GuideListPage> page_;
};

class GameSourceConsoleDownloads {
 public:
  static std::unique_ptr<GameSourceConsoleDownloads> Create(
      const GameSourceVisualsProvider& provider) {
    if (!provider)
      return nullptr;
    auto visuals = provider();
    if (!visuals || !visuals->assets || !visuals->assets->has_options)
      return nullptr;
    auto panel = std::unique_ptr<GameSourceConsoleDownloads>(new GameSourceConsoleDownloads);
    panel->visuals_ = std::move(*visuals);
    xui::SceneContext context;
    context.skin = &panel->visuals_.assets->skin;
    context.package = "hud/hud";
    context.play_sound = panel->visuals_.play_sound;
    panel->scene_ =
        guide::ActiveDownloadsScene::Create(panel->visuals_.assets->options_notifications, context);
    if (!panel->scene_)
      return nullptr;
    guide::HideGuideButtonLetters(&panel->scene_->root());
    auto& root = panel->scene_->root();
    root.Set("Position", xui::Value{xui::Vec3{}});
    if (!std::isfinite(root.width()) || !std::isfinite(root.height()) || root.width() <= 0 ||
        root.height() <= 0 || root.width() > 4096 || root.height() > 4096)
      return nullptr;
    return panel;
  }
  void Draw(ImGuiIO& io, guide::GuideActivity activity) {
    scene_->Update({std::move(activity)});
    auto& root = scene_->root();
    const auto origin = ImGui::GetCursorScreenPos();
    const float scale = std::max(1.0f, ImGui::GetContentRegionAvail().x) / root.width();
    auto* row = scene_->row(0);
    xui::Vec3 position{};
    for (auto* element = row; element && element != &root; element = element->parent()) {
      const auto p = element->position();
      position.x += p.x;
      position.y += p.y;
    }
    ImGui::SetCursorScreenPos(ImVec2(origin.x + position.x * scale, origin.y + position.y * scale));
    if (ImGui::InvisibleButton(
            "Cancel source copy",
            ImVec2(std::max(1.0f, row->width() * scale), std::max(1.0f, row->height() * scale)),
            ImGuiButtonFlags_EnableNav))
      scene_->Activate();
    root.Advance(std::clamp(double(io.DeltaTime), 0.0, 0.25) * xui::kFramesPerSecond);
    xui::Render(ImGui::GetWindowDrawList(), root, origin, scale, 1, visuals_.resources);
    ImGui::SetCursorScreenPos(origin);
    ImGui::Dummy(ImVec2(root.width() * scale, root.height() * scale));
  }

 private:
  GameSourceVisuals visuals_;
  std::unique_ptr<guide::ActiveDownloadsScene> scene_;
};

GameSourceDialog::GameSourceDialog(ImGuiDrawer* drawer, system::GameSourceIdentity expected,
                                   std::filesystem::path config,
                                   std::function<void(std::filesystem::path)> completed,
                                   std::filesystem::path initial, std::string error,
                                   std::string executable, std::filesystem::path extraction_root,
                                   LaunchPadSource pad_source, GameSourceVisualsProvider visuals,
                                   std::function<void(guide::GuideActivity)> activity_completed)
    : ImGuiDialog(drawer),
      expected_(std::move(expected)),
      config_(std::move(config)),
      completed_(std::move(completed)),
      initial_source_(initial),
      executable_(std::move(executable)),
      extraction_root_(std::move(extraction_root)),
      error_(std::move(error)),
      pad_source_(std::move(pad_source)),
      visuals_(std::move(visuals)),
      activity_completed_(std::move(activity_completed)) {
  const auto text = rex::string::to_utf8(initial.u16string());
  std::memcpy(path_.data(), text.data(), std::min(text.size(), path_.size() - 1));
}

GameSourceDialog::~GameSourceDialog() {
  if (pad_source_)
    ReleaseLaunchPad(GetIO());
  *check_cancel_ = true;
  if (copy_progress_)
    copy_progress_->cancel = true;
}

void GameSourceDialog::ShowConsolePage(ConsoleScreen screen, std::string title,
                                       std::vector<guide::GuideListRow> rows, size_t initial,
                                       std::string legend_b, std::string empty_details) {
  console_initialized_ = true;
  console_screen_ = screen;
  console_choices_.clear();
  for (const auto& row : rows)
    console_choices_.push_back(row.text);
  if (!console_page_) {
    auto visuals = visuals_ ? visuals_() : std::nullopt;
    console_waiting_for_guide_ = visuals && visuals->loading;
    if (visuals && !visuals->loading)
      console_page_ = GameSourceGuidePage::Create(std::move(*visuals));
  }
  console_mode_ = bool(console_page_);
  if (!console_page_) {
    console_title_ = std::move(title);
    console_rows_ = std::move(rows);
    console_initial_choice_ = initial;
    console_legend_b_ = std::move(legend_b);
    console_empty_details_ = std::move(empty_details);
    return;
  }
  console_page_->page().Show(std::move(title), std::move(rows), initial, "Select",
                             std::move(legend_b), std::move(empty_details));
}

void GameSourceDialog::ShowConsoleScreen(ConsoleScreen screen, std::string title, std::string body,
                                         std::vector<std::string> choices, size_t initial) {
  std::vector<guide::GuideListRow> rows;
  for (auto& choice : choices)
    rows.push_back({std::move(choice), {}, body});
  ShowConsolePage(screen, std::move(title), std::move(rows), initial,
                  screen == ConsoleScreen::kSource ? "Exit" : "Back", std::move(body));
}

void GameSourceDialog::BeginSourceCheck(std::filesystem::path path) {
  path_.fill(0);
  const auto text = Utf8(path);
  if (text.size() >= path_.size()) {
    ShowConsoleScreen(ConsoleScreen::kError, "Game Files", "The selected path is too long.",
                      {"Choose Another Source", "Leave Game"}, 0);
    return;
  }
  std::memcpy(path_.data(), text.data(), text.size());
  validated_.clear();
  error_.clear();
  checking_path_ = std::move(path);
  check_origin_ = console_screen_;
  const auto expected = expected_;
  *check_cancel_ = false;
  checking_ = std::async(std::launch::async, [path = checking_path_, expected,
                                              executable = executable_, cancel = check_cancel_] {
    return system::InspectGameSource(path, executable, expected,
                                     [cancel] { return cancel->load(); });
  });
  ShowConsolePage(ConsoleScreen::kChecking, "Checking Game Files", {}, 0, "Cancel",
                  text + "\r\n\r\nChecking the game title and executable. Please wait.");
}

void GameSourceDialog::CollectOpticalDrives() {
  optical_drives_.clear();
  const DWORD drives = GetLogicalDrives();
  for (int i = 0; i < 26; ++i) {
    if (!(drives & (1u << i)))
      continue;
    const std::wstring drive{wchar_t(L'A' + i), L':', L'\\'};
    if (GetDriveTypeW(drive.c_str()) == DRIVE_CDROM)
      optical_drives_.push_back(std::string(1, char('A' + i)) + ":");
  }
}

void GameSourceDialog::ShowSourceScreen() {
  ShowConsolePage(
      ConsoleScreen::kSource, "Choose Game Files",
      {{"Disc Image (ISO)",
        {},
        "Play from an Xbox 360 disc image (.iso) on this PC.\r\n\r\nYou can play it directly or "
        "extract it to this PC."},
       {"Extracted Game Folder",
        {},
        "Play from a folder of game files extracted from your Xbox 360 disc."},
       {"Disc Drive", {}, "Read the game from a disc in a drive connected to this PC."}},
      0, "Exit");
}

void GameSourceDialog::ShowDriveScreen() {
  std::vector<guide::GuideListRow> rows;
  for (const auto& drive : optical_drives_)
    rows.push_back({drive, "Drive", "Read the Xbox 360 disc in drive " + drive + "."});
  ShowConsolePage(ConsoleScreen::kDrives, "Choose Disc Drive", std::move(rows), 0, "Back",
                  "No disc drive was found.");
}

void GameSourceDialog::ShowBrowseScreen() {
  ShowConsolePage(ConsoleScreen::kBrowse,
                  browse_folder_ ? "Choose Game Folder" : "Choose Disc Image (ISO)",
                  browser_->rows(), browser_->focus(), "Back", browser_->empty_details());
}

void GameSourceDialog::HandleConsoleCancel() {
  switch (console_screen_) {
    case ConsoleScreen::kSource:
      Close();
      break;
    case ConsoleScreen::kBrowse:
      if (browser_->Up())
        ShowBrowseScreen();
      else
        ShowSourceScreen();
      break;
    case ConsoleScreen::kChecking:
      if (checking_.valid())
        *check_cancel_ = true;
      else
        Close();
      break;
    case ConsoleScreen::kExtracting:
      break;  // A on the copy cancels it, as in Active Downloads
    case ConsoleScreen::kVerified:
    case ConsoleScreen::kError:
      ReturnFromCheck();
      break;
    case ConsoleScreen::kDrives:
    case ConsoleScreen::kSaveFailed:
    case ConsoleScreen::kCopied:
      ShowSourceScreen();
      break;
  }
  if (console_page_ && console_page_->visuals().play_sound)
    console_page_->visuals().play_sound("sharedres://btn_Back.xma", "");
}

void GameSourceDialog::ReturnFromCheck() {
  switch (check_origin_) {
    case ConsoleScreen::kBrowse:
      browser_->Open(browser_->folder(), checking_path_);
      ShowBrowseScreen();
      break;
    case ConsoleScreen::kDrives:
      ShowDriveScreen();
      break;
    default:
      ShowSourceScreen();
      break;
  }
}

void GameSourceDialog::CompleteConsoleSelection(bool remember) {
  const auto path = Utf8(validated_);
  if (!cvar::SetFlagByName("game_source", path)) {
    ShowConsoleScreen(ConsoleScreen::kError, "Game Files", "Unable to use these game files.",
                      {"Choose Another Source", "Leave Game"}, 0);
  } else if (remember && !cvar::TrySaveConfig(config_)) {
    ShowConsoleScreen(
        ConsoleScreen::kSaveFailed, "Game Files",
        "These game files couldn't be remembered. Use them this time only, or choose others.",
        {"Use Once", "Choose Another Source", "Leave Game"}, 0);
  } else {
    selected_ = validated_;
    Close();
  }
}

void GameSourceDialog::StartExtraction() {
  copy_progress_ = std::make_shared<CopyProgress>();
  const auto destination = extraction_root_ / ("game-" + expected_.executable_checksum);
  const auto source = validated_;
  const auto expected = expected_;
  const auto progress = copy_progress_;
  const auto executable = executable_;
  extracting_ =
      std::async(std::launch::async, [source, destination, expected, progress, executable] {
        return system::ExtractGameSource(
            source, destination, expected,
            [progress](uint64_t done, uint64_t total) {
              progress->done = done;
              progress->total = total;
            },
            [progress] { return progress->cancel.load(); }, executable);
      });
  if (console_mode_)
    console_screen_ = ConsoleScreen::kExtracting;
}

void GameSourceDialog::HandleConsoleChoice(size_t choice) {
  if (choice >= console_choices_.size())
    return;
  const std::string& chosen = console_choices_[choice];
  switch (console_screen_) {
    case ConsoleScreen::kSource:
      if (choice == 0) {
        Browse(false);
      } else if (choice == 1) {
        Browse(true);
      } else {
        CollectOpticalDrives();
        check_origin_ = ConsoleScreen::kSource;
        if (optical_drives_.empty())
          ShowConsoleScreen(ConsoleScreen::kError, "Disc Drive", "No disc drive was found.",
                            {"Choose Another Source", "Leave Game"}, 0);
        else
          ShowDriveScreen();
      }
      break;
    case ConsoleScreen::kBrowse: {
      const auto result = browser_->Select(choice);
      if (result.kind == guide::GuideFileBrowser::Result::Kind::kOpened)
        ShowBrowseScreen();
      else if (result.kind == guide::GuideFileBrowser::Result::Kind::kChosen)
        BeginSourceCheck(result.path);
      break;
    }
    case ConsoleScreen::kDrives:
      if (choice < optical_drives_.size())
        BeginSourceCheck(std::filesystem::path("\\\\.\\" + optical_drives_[choice]));
      break;
    case ConsoleScreen::kChecking:
      break;
    case ConsoleScreen::kExtracting:
      if (copy_progress_)
        copy_progress_->cancel = true;
      break;
    case ConsoleScreen::kVerified:
    case ConsoleScreen::kCopied:
      if (chosen == "Use This Source")
        CompleteConsoleSelection(true);
      else if (chosen == "Extract to This PC")
        StartExtraction();
      else if (chosen == "Choose Another Source")
        ShowSourceScreen();
      else
        Close();
      break;
    case ConsoleScreen::kError:
      if (choice == 0)
        ShowSourceScreen();
      else
        Close();
      break;
    case ConsoleScreen::kSaveFailed:
      if (choice == 0)
        CompleteConsoleSelection(false);
      else if (choice == 1)
        ShowSourceScreen();
      else
        Close();
      break;
  }
}

void GameSourceDialog::Browse(bool folder) {
  browse_folder_ = folder;
  browser_ = std::make_unique<guide::GuideFileBrowser>(
      folder ? guide::GuideFileBrowser::Pick::kFolder : guide::GuideFileBrowser::Pick::kFile,
      std::vector<std::wstring>{L".iso"});
  browser_->Open({});
  ShowBrowseScreen();
}

void GameSourceDialog::BrowseNative(bool folder) {
  const HRESULT initialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  struct Uninitialize {
    bool own;
    ~Uninitialize() {
      if (own)
        CoUninitialize();
    }
  } cleanup{SUCCEEDED(initialized)};
  Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
  if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&dialog)))) {
    error_ = "Unable to open the file browser. Type the location below.";
    return;
  }
  DWORD options = 0;
  dialog->GetOptions(&options);
  dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST |
                     (folder ? FOS_PICKFOLDERS : FOS_FILEMUSTEXIST));
  if (!folder) {
    const COMDLG_FILTERSPEC filter[] = {{L"Xbox 360 disc images", L"*.iso"}};
    dialog->SetFileTypes(1, filter);
  }
  dialog->SetTitle(folder ? L"Choose an extracted game folder" : L"Choose a game disc image");
  if (FAILED(dialog->Show(nullptr)))
    return;
  Microsoft::WRL::ComPtr<IShellItem> item;
  PWSTR value = nullptr;
  if (SUCCEEDED(dialog->GetResult(&item)) &&
      SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &value))) {
    const auto text = Utf8(std::filesystem::path(value));
    if (text.size() < path_.size()) {
      path_.fill(0);
      std::memcpy(path_.data(), text.data(), text.size());
      validated_.clear();
      error_.clear();
    } else {
      error_ = "The selected path is too long.";
    }
    CoTaskMemFree(value);
  }
}

void GameSourceDialog::HandleConsoleInput(const LaunchPadState& pad) {
  const bool cancel_pressed = (pad.cancel && !previous_pad_.cancel) ||
                              ImGui::IsKeyPressed(ImGuiKey_Escape, false) ||
                              ImGui::IsKeyPressed(ImGuiKey_Backspace, false);
  const bool activate_pressed = (pad.activate && !previous_pad_.activate) ||
                                ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
                                ImGui::IsKeyPressed(ImGuiKey_Space, false);
  previous_pad_ = pad;
  // The stick or d-pad steps once, then repeats while held, as in the Guide.
  int direction = pad.y > 0.5f ? -1 : pad.y < -0.5f ? 1 : 0;
  const auto now = std::chrono::steady_clock::now();
  if (direction != previous_pad_direction_) {
    previous_pad_direction_ = direction;
    next_pad_navigation_ = now + std::chrono::milliseconds(400);
  } else if (direction && now >= next_pad_navigation_) {
    next_pad_navigation_ = now + std::chrono::milliseconds(110);
  } else {
    direction = 0;
  }
  if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, true))
    direction = -1;
  else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow, true))
    direction = 1;
  int rows = 1;
  if (ImGui::IsKeyPressed(ImGuiKey_PageUp, true))
    direction = -1, rows = int(guide::GuideListPage::kVisibleRows);
  else if (ImGui::IsKeyPressed(ImGuiKey_PageDown, true))
    direction = 1, rows = int(guide::GuideListPage::kVisibleRows);
  if (!console_page_)
    return;
  auto& page = console_page_->page();
  for (int i = 0; i < rows && direction; ++i)
    page.Move(direction);
  if (cancel_pressed) {
    HandleConsoleCancel();
  } else if (activate_pressed) {
    if (const auto choice = page.Activate())
      HandleConsoleChoice(*choice);
  }
}

void GameSourceDialog::OnDraw(ImGuiIO& io) {
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  const auto pad = PollLaunchPad(io, pad_source_, !console_mode_);
  if (console_mode_)
    HandleConsoleInput(pad);
  else
    previous_pad_ = pad;
  if (extracting_.valid() &&
      extracting_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    try {
      const auto result = extracting_.get();
      error_ = result.cancelled ? "Copying was cancelled. Nothing was changed." : result.error;
      if (activity_completed_) {
        guide::GuideActivity activity;
        activity.title = "Copying Game Files";
        activity.status = result.cancelled ? "Cancelled" : result ? "Completed" : "Failed";
        activity.details = result ? "Game files copied to this PC." : error_;
        activity_completed_(std::move(activity));
      }
      if (result) {
        validated_ = result.folder;
        const auto text = Utf8(validated_);
        if (text.size() < path_.size()) {
          path_.fill(0);
          std::memcpy(path_.data(), text.data(), text.size());
        }
        if (console_mode_)
          ShowConsoleScreen(ConsoleScreen::kCopied, "Game Files Copied",
                            "The game files are ready on this PC.\r\n\r\n" + text,
                            {"Use This Source", "Choose Another Source", "Leave Game"}, 0);
      } else if (console_mode_) {
        ShowConsoleScreen(ConsoleScreen::kError, "Game Files Not Copied", error_,
                          {"Choose Another Source", "Leave Game"}, 0);
      }
    } catch (const std::exception& error) {
      error_ = error.what();
      if (console_mode_)
        ShowConsoleScreen(ConsoleScreen::kError, "Game Files Not Copied", error_,
                          {"Choose Another Source", "Leave Game"}, 0);
    }
  }
  if (checking_.valid() &&
      checking_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    const bool cancelled = check_cancel_->load();
    try {
      auto result = checking_.get();
      error_ = result.error;
      const auto shown = Utf8(checking_path_);
      if (result) {
        validated_ = result.source_path;
        if (console_mode_ || console_waiting_for_guide_) {
          const bool can_extract =
              !extraction_root_.empty() && (filesystem::IsOpticalDiscPath(validated_) ||
                                            std::filesystem::is_regular_file(validated_));
          std::vector<guide::GuideListRow> rows{
              {"Use This Source",
               {},
               shown + "\r\n\r\nThese game files match this build. They will be used every "
                       "time the game starts."}};
          if (can_extract)
            rows.push_back({"Extract to This PC",
                            {},
                            shown + "\r\n\r\nCopy the game files to this PC, then use the copy."});
          rows.push_back(
              {"Choose Another Source", {}, "Choose a different disc image, folder or drive."});
          ShowConsolePage(ConsoleScreen::kVerified, "Game Files Verified", std::move(rows), 0,
                          "Back");
        }
      } else if (cancelled && (console_mode_ || console_waiting_for_guide_)) {
        ReturnFromCheck();
      } else if (console_mode_ || console_waiting_for_guide_) {
        ShowConsoleScreen(ConsoleScreen::kError, "Game Files Not Recognized",
                          shown + "\r\n\r\n" + error_, {"Choose Another Source", "Leave Game"}, 0);
      }
    } catch (const std::exception& error) {
      error_ = error.what();
      if (console_mode_ || console_waiting_for_guide_)
        ShowConsoleScreen(ConsoleScreen::kError, "Game Files Not Recognized", error_,
                          {"Choose Another Source", "Leave Game"}, 0);
    }
  }
  if (!console_initialized_) {
    if (expected_.title_id && !expected_.executable_checksum.empty()) {
      ShowSourceScreen();
    } else {
      ShowConsoleScreen(ConsoleScreen::kError, "Game Files",
                        "This build has no source fingerprint. Regenerate its code to enable "
                        "first-run source selection.",
                        {"Leave Game"}, 0);
    }
  }
  if (console_waiting_for_guide_)
    ShowConsolePage(console_screen_, console_title_, console_rows_, console_initial_choice_,
                    console_legend_b_, console_empty_details_);
  if (console_waiting_for_guide_) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    constexpr ImGuiWindowFlags kLoadingFlags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("##GameSourceGuideLoading", nullptr, kLoadingFlags);
    ImGui::GetWindowDrawList()->AddRectFilled(
        viewport->Pos,
        ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y), kGuideDim);
    ImGui::End();
    return;
  }
  if (console_mode_ && !initial_source_.empty()) {
    auto initial = std::move(initial_source_);
    initial_source_.clear();
    BeginSourceCheck(std::move(initial));
  }
  if (console_mode_) {
    if (extracting_.valid()) {
      const uint64_t total = copy_progress_->total;
      const uint64_t done = copy_progress_->done;
      std::string details = Utf8(validated_) + "\r\n\r\nCopying the game files to this PC.";
      if (total)
        details += "\r\n" + guide::FormatGuideSize(done) + " of " + guide::FormatGuideSize(total);
      details += "\r\n\r\nPress A to cancel.";
      console_page_->page().Show(
          "Extracting Game Files",
          {{Utf8(validated_.filename()),
            total
                ? fmt::format("{}%", unsigned(std::min(100.0, double(done) / double(total) * 100)))
                : "Checking",
            std::move(details)}},
          0, copy_progress_->cancel ? "" : "Cancel", "");
    }
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    constexpr ImGuiWindowFlags kOverlayFlags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNav;
    ImGui::Begin("##GameSourceGuideOverlay", nullptr, kOverlayFlags);
    ImGui::GetWindowDrawList()->AddRectFilled(
        viewport->Pos,
        ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y), kGuideDim);
    if (const auto chosen = console_page_->Draw(io, !checking_.valid()))
      HandleConsoleChoice(*chosen);
    ImGui::End();
    return;
  }
  ImGui::SetNextWindowSize(ImVec2(650, 0), ImGuiCond_FirstUseEver);
  ImGui::Begin("Choose game files", nullptr, ImGuiWindowFlags_NoCollapse);
  ImGui::TextWrapped(
      "Choose your Xbox 360 game files. The executable must match this recomp build.");
  const bool identified = expected_.title_id && !expected_.executable_checksum.empty();
  if (!identified)
    ImGui::TextWrapped(
        "This build has no source fingerprint. Regenerate its code to enable first-run source "
        "selection.");
  ImGui::BeginDisabled(checking_.valid() || extracting_.valid());
  if (ImGui::Button("Choose an extracted folder"))
    BrowseNative(true);
  ImGui::SameLine();
  if (ImGui::Button("Choose a disc image (ISO)"))
    BrowseNative(false);
  if (ImGui::Button("Read from disc drive"))
    ImGui::OpenPopup("Optical drives");
  if (ImGui::BeginPopup("Optical drives")) {
    bool found = false;
    const DWORD drives = GetLogicalDrives();
    for (int i = 0; i < 26; ++i) {
      if (!(drives & (1u << i)))
        continue;
      const std::wstring drive{wchar_t(L'A' + i), L':', L'\\'};
      if (GetDriveTypeW(drive.c_str()) != DRIVE_CDROM)
        continue;
      found = true;
      const std::string label = std::string(1, char('A' + i)) + ":";
      if (ImGui::Selectable(label.c_str())) {
        const std::string source = "\\\\.\\" + label;
        path_.fill(0);
        std::memcpy(path_.data(), source.data(), source.size());
        validated_.clear();
        error_.clear();
      }
    }
    if (!found)
      ImGui::TextUnformatted("No disc drive was found.");
    ImGui::EndPopup();
  }
  if (ImGui::InputText("Source path", path_.data(), path_.size())) {
    validated_.clear();
    error_.clear();
  }
  ImGui::BeginDisabled(!identified || !path_[0]);
  if (ImGui::Button("Check source")) {
    validated_.clear();
    checking_path_ = rex::to_path(path_.data());
    const auto expected = expected_;
    *check_cancel_ = false;
    checking_ = std::async(std::launch::async, [path = checking_path_, expected,
                                                executable = executable_, cancel = check_cancel_] {
      return system::InspectGameSource(path, executable, expected,
                                       [cancel] { return cancel->load(); });
    });
  }
  ImGui::EndDisabled();
  ImGui::EndDisabled();
  if (checking_.valid()) {
    ImGui::TextUnformatted("Checking game identity and executable checksum...");
    if (ImGui::Button("Cancel check"))
      *check_cancel_ = true;
  }
  if (!error_.empty())
    ImGui::TextWrapped("%s", error_.c_str());
  if (!validated_.empty())
    ImGui::TextUnformatted("Game files match this build.");
  if (!extracting_.valid() && !checking_.valid() && !validated_.empty() &&
      !extraction_root_.empty() &&
      (filesystem::IsOpticalDiscPath(validated_) || std::filesystem::is_regular_file(validated_))) {
    if (ImGui::Button("Extract to this PC")) {
      copy_progress_ = std::make_shared<CopyProgress>();
      const auto destination = extraction_root_ / ("game-" + expected_.executable_checksum);
      extracting_ =
          std::async(std::launch::async, [source = validated_, destination, expected = expected_,
                                          progress = copy_progress_, executable = executable_] {
            return system::ExtractGameSource(
                source, destination, expected,
                [progress](uint64_t done, uint64_t total) {
                  progress->done = done;
                  progress->total = total;
                },
                [progress] { return progress->cancel.load(); }, executable);
          });
    }
  }
  if (extracting_.valid()) {
    const uint64_t total = copy_progress_->total;
    const uint64_t done = copy_progress_->done;
    if (!console_downloads_)
      console_downloads_ = GameSourceConsoleDownloads::Create(visuals_);
    if (console_downloads_) {
      guide::GuideActivity activity;
      activity.title = "Copying Game Files";
      activity.status =
          total
              ? std::to_string(unsigned(std::min(100.0, double(done) / double(total) * 100))) + "%"
              : "Checking source";
      activity.details = "Copying game files to this PC. Select this item to cancel.";
      activity.cancel = [progress = copy_progress_] { progress->cancel = true; };
      console_downloads_->Draw(io, std::move(activity));
    } else {
      ImGui::ProgressBar(total ? float(double(done) / double(total)) : 0);
      ImGui::Text("Copying game files: %llu / %llu bytes", (unsigned long long)done,
                  (unsigned long long)total);
    }
    if (ImGui::Button("Cancel extraction"))
      copy_progress_->cancel = true;
  }
  ImGui::Checkbox("Remember source", &remember_);
  ImGui::BeginDisabled(validated_.empty() || checking_.valid() || extracting_.valid());
  if (ImGui::Button("Use this source")) {
    const auto path = rex::string::to_utf8(validated_.u16string());
    if (!cvar::SetFlagByName("game_source", path)) {
      error_ = "Unable to use these game files.";
    } else if (remember_ && !cvar::TrySaveConfig(config_)) {
      error_ = "The source could not be saved. Uncheck Remember source to use it for this session.";
    } else {
      selected_ = validated_;
      Close();
    }
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  if (ImGui::Button("Leave Game"))
    Close();
  ImGui::TextWrapped(
      "An ISO can be used directly or copied to this PC. Ordinary PC optical drives cannot read an "
      "Xbox 360 disc's game partition.");
  ImGui::End();
}
ShaderPreparationDialog::ShaderPreparationDialog(ImGuiDrawer* drawer, ProgressQuery progress,
                                                 std::function<void()> finished,
                                                 GameSourceVisualsProvider visuals)
    : ImGuiDialog(drawer),
      progress_(std::move(progress)),
      finished_(std::move(finished)),
      visuals_(std::move(visuals)) {}

ShaderPreparationDialog::~ShaderPreparationDialog() = default;

void ShaderPreparationDialog::OnDraw(ImGuiIO& io) {
  uint32_t done = 0, total = 0;
  if (!progress_ || !progress_(done, total)) {
    if (!finished_called_) {
      finished_called_ = true;
      if (finished_)
        finished_();
      Close();
    }
    return;
  }
  if (!page_ && visuals_) {
    if (auto visuals = visuals_(); visuals && !visuals->loading)
      page_ = GameSourceGuidePage::Create(std::move(*visuals));
  }
  const unsigned percent =
      total ? unsigned(std::min(100.0, double(done) / double(total) * 100.0)) : 0;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  constexpr ImGuiWindowFlags kFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav;
  ImGui::Begin("##ShaderPreparation", nullptr, kFlags);
  ImGui::GetWindowDrawList()->AddRectFilled(
      viewport->Pos, ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y),
      kGuideDim);
  std::string details =
      "Getting the game's graphics ready for this PC. This happens once, and again after an "
      "update.";
  if (total)
    details += fmt::format("\r\n\r\n{} of {} prepared.", done, total);
  if (page_) {
    page_->page().Show(
        "Preparing Shaders",
        {{"Graphics", total ? fmt::format("{}%", percent) : "Starting", std::move(details)}}, 0, "",
        "");
    page_->Draw(io, false);
  } else {
    const ImVec2 size(std::min(viewport->Size.x * 0.5f, 900.0f), 0);
    ImGui::SetCursorScreenPos(ImVec2(viewport->Pos.x + (viewport->Size.x - size.x) * 0.5f,
                                     viewport->Pos.y + viewport->Size.y * 0.45f));
    ImGui::BeginGroup();
    ImGui::TextUnformatted("Preparing shaders");
    ImGui::ProgressBar(total ? float(done) / float(total) : 0.0f, ImVec2(size.x, 0));
    ImGui::TextUnformatted("Getting the game's graphics ready for this PC.");
    ImGui::EndGroup();
  }
  ImGui::End();
}

void GameSourceDialog::OnClose() {
  if (completed_)
    completed_(selected_);
}
GameMediaRecoveryDialog::GameMediaRecoveryDialog(ImGuiDrawer* drawer, bool optical,
                                                 std::string error,
                                                 std::function<void(bool)> completed,
                                                 LaunchPadSource pad_source,
                                                 GameSourceVisualsProvider visuals)
    : ImGuiDialog(drawer),
      optical_(optical),
      error_(std::move(error)),
      completed_(std::move(completed)),
      pad_source_(std::move(pad_source)),
      visuals_(std::move(visuals)) {}
GameMediaRecoveryDialog::~GameMediaRecoveryDialog() {
  if (pad_source_)
    ReleaseLaunchPad(GetIO());
}
void GameMediaRecoveryDialog::OnDraw(ImGuiIO& io) {
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  PollLaunchPad(io, pad_source_);
  ImGui::SetNextWindowSize(ImVec2(600, 0), ImGuiCond_FirstUseEver);
  ImGui::Begin("Game source unavailable", nullptr, ImGuiWindowFlags_NoCollapse);
  ImGui::TextWrapped("%s",
                     optical_ ? "Reinsert the disc." : "Reconnect the drive with the disc image.");
  if (!error_.empty())
    ImGui::TextWrapped("%s", error_.c_str());
  const std::string choices[] = {"Retry", "Leave Game"};
  if (!console_box_)
    console_box_ = GameSourceConsoleBox::Create(
        visuals_, "Game source unavailable",
        error_.empty()
            ? (optical_ ? "Reinsert the disc." : "Reconnect the drive with the disc image.")
            : error_,
        choices, 1);
  std::optional<size_t> chosen;
  if (console_box_)
    chosen = console_box_->Draw(io, true);
  if ((console_box_ && chosen == 0) || (!console_box_ && ImGui::Button("Retry"))) {
    retry_ = true;
    Close();
  }
  if (!console_box_) {
    ImGui::SameLine();
    if (focus_leave_)
      ImGui::SetKeyboardFocusHere();
  }
  if ((console_box_ && chosen == 1) || (!console_box_ && ImGui::Button("Leave Game")))
    Close();
  focus_leave_ = false;
  ImGui::End();
}
void GameMediaRecoveryDialog::OnClose() {
  if (completed_)
    completed_(retry_);
}
}  // namespace rex::ui
