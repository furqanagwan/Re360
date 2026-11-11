// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include <rex/ui/guide/guide_list_page.h>

namespace rex::ui::guide {

class GuideFileBrowser {
 public:
  enum class Pick { kFile, kFolder };
  struct Result {
    enum class Kind { kNone, kOpened, kChosen };
    Kind kind = Kind::kNone;
    std::filesystem::path path;
  };
  static constexpr size_t kMaxEntries = 4096;

  GuideFileBrowser(Pick pick, std::vector<std::wstring> extensions = {});

  void Open(std::filesystem::path folder, const std::filesystem::path& focus = {});

  Result Select(size_t row);

  bool Up();

  const std::filesystem::path& folder() const { return folder_; }
  const std::vector<GuideListRow>& rows() const { return rows_; }
  size_t focus() const { return focus_; }

  std::string empty_details() const;

  std::function<std::vector<std::filesystem::path>()> list_drives;

 private:
  bool use_folder_row() const { return pick_ == Pick::kFolder && !folder_.empty(); }

  Pick pick_;
  std::vector<std::wstring> extensions_;
  std::filesystem::path folder_;
  std::vector<std::filesystem::path> entries_;
  std::vector<GuideListRow> rows_;
  size_t focus_ = 0;
};

std::string FormatGuideSize(uint64_t bytes);

}
