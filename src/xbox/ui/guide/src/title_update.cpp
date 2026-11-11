/**
 * @file        ui/guide/src/title_update.cpp
 * @brief       Optional title updates: packages, sources, install and launch (RG-GDK-057)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/title_update.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <mutex>
#include <thread>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <rex/cvar.h>
#include <rex/filesystem.h>
#include <rex/logging.h>
#include <rex/string.h>

#include "crypto/TinySHA1.hpp"

#include <windows.h>
#include <winhttp.h>

REXCVAR_DEFINE_STRING(title_update_sources, "", "Content",
                      "More title update download sources, tried after Xbox Unity: URL "
                      "templates separated by ';' with {title_id}, {media_id}, {version} and "
                      "{content_id}");

REXCVAR_DEFINE_INT32(title_update, 0, "Content",
                     "The title update to run, turned on in the guide's Manage Game; 0 runs "
                     "the original. Optional: without the update or its executable the original "
                     "runs");
REXCVAR_DEFINE_BOOL(title_update_handoff, false, "Content",
                    "Set when another executable of this title started this one for the "
                    "title_update choice");

namespace rex::ui::guide {
namespace fs = std::filesystem;

namespace {

uint32_t Be32(const uint8_t* p) {
  return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | p[3];
}

std::string Upper(std::string_view text) {
  std::string out(text);
  std::transform(out.begin(), out.end(), out.begin(),
                 [](unsigned char c) { return char(std::toupper(c)); });
  return out;
}

std::wstring Wide(std::string_view text) {
  const std::u16string utf16 = rex::string::to_utf16(text);
  return std::wstring(utf16.begin(), utf16.end());
}

struct Handle {
  HINTERNET h = nullptr;
  ~Handle() {
    if (h) {
      WinHttpCloseHandle(h);
    }
  }
};

struct Request {
  Handle session, connection, request;
  DWORD status = 0;
  uint64_t length = 0;
};

std::string Open(const std::string& url, Request& r) {
  URL_COMPONENTS parts = {sizeof(parts)};
  wchar_t host[256] = {};
  wchar_t path[2048] = {};
  wchar_t extra[2048] = {};
  parts.lpszHostName = host;
  parts.dwHostNameLength = DWORD(std::size(host));
  parts.lpszUrlPath = path;
  parts.dwUrlPathLength = DWORD(std::size(path));
  parts.lpszExtraInfo = extra;
  parts.dwExtraInfoLength = DWORD(std::size(extra));
  const std::wstring wide = Wide(url);
  if (!WinHttpCrackUrl(wide.c_str(), 0, 0, &parts)) {
    return "not a URL";
  }
  r.session.h = WinHttpOpen(L"rexglue", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
                            WINHTTP_NO_PROXY_BYPASS, 0);
  if (!r.session.h) {
    return fmt::format("could not start a download (error {})", GetLastError());
  }
  WinHttpSetTimeouts(r.session.h, 15000, 15000, 30000, 30000);
  r.connection.h = WinHttpConnect(r.session.h, host, parts.nPort, 0);
  if (!r.connection.h) {
    return fmt::format("could not reach the server (error {})", GetLastError());
  }
  const std::wstring object = std::wstring(path) + extra;
  r.request.h =
      WinHttpOpenRequest(r.connection.h, L"GET", object.c_str(), nullptr, WINHTTP_NO_REFERER,
                         WINHTTP_DEFAULT_ACCEPT_TYPES,
                         parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
  if (!r.request.h ||
      !WinHttpSendRequest(r.request.h, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0,
                          0, 0) ||
      !WinHttpReceiveResponse(r.request.h, nullptr)) {
    return fmt::format("the server didn't answer (error {})", GetLastError());
  }
  DWORD size = sizeof(r.status);
  WinHttpQueryHeaders(r.request.h, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                      WINHTTP_HEADER_NAME_BY_INDEX, &r.status, &size, WINHTTP_NO_HEADER_INDEX);
  if (r.status != 200) {
    return fmt::format("the server answered {}", r.status);
  }
  wchar_t length[32] = {};
  size = sizeof(length);
  if (WinHttpQueryHeaders(r.request.h, WINHTTP_QUERY_CONTENT_LENGTH, WINHTTP_HEADER_NAME_BY_INDEX,
                          length, &size, WINHTTP_NO_HEADER_INDEX)) {
    r.length = std::wcstoull(length, nullptr, 10);
  }
  return {};
}

std::string Fill(std::string_view pattern, const PPCTitleUpdate& update, uint32_t title_id) {
  std::string out(pattern);
  auto replace = [&](std::string_view key, const std::string& value) {
    for (size_t at; (at = out.find(key)) != std::string::npos;) {
      out.replace(at, key.size(), value);
    }
  };
  replace("{title_id}", fmt::format("{:08X}", title_id));
  replace("{media_id}", Upper(update.media_id ? update.media_id : ""));
  replace("{version}", std::to_string(update.version));
  replace("{content_id}", Upper(update.content_id ? update.content_id : ""));
  return out;
}

}

std::string HexString(std::span<const uint8_t> bytes) {
  std::string out;
  for (uint8_t b : bytes) {
    out += fmt::format("{:02X}", b);
  }
  return out;
}

std::optional<TitleUpdatePackage> ReadTitleUpdatePackage(std::span<const uint8_t> head,
                                                         std::string* error) {
  auto fail = [&](std::string why) -> std::optional<TitleUpdatePackage> {
    if (error) {
      *error = std::move(why);
    }
    return std::nullopt;
  };
  if (head.size() < 0x1000) {
    return fail("the file is too small to be a package");
  }
  const std::string_view magic(reinterpret_cast<const char*>(head.data()), 4);
  if (magic != "LIVE" && magic != "PIRS" && magic != "CON ") {
    return fail("it isn't an Xbox 360 package");
  }
  TitleUpdatePackage p;
  p.content_type = Be32(&head[0x344]);
  p.media_id = Be32(&head[0x354]);
  p.base_version = Be32(&head[0x35C]);
  p.title_id = Be32(&head[0x360]);
  std::copy_n(&head[0x32C], 20, p.content_id.begin());

  std::u16string name;
  for (size_t at = 0x411; at + 1 < 0x411 + 0x80 && at + 1 < head.size(); at += 2) {
    const char16_t c = char16_t((head[at] << 8) | head[at + 1]);
    if (!c) {
      break;
    }
    name.push_back(c);
  }
  p.display_name = rex::string::to_utf8(name);
  const uint32_t header_size = Be32(&head[0x340]);
  const size_t end = (size_t(header_size) + 0xFFF) & ~size_t(0xFFF);
  if (header_size >= 0x344 && end <= head.size()) {
    sha1::SHA1 hash;
    hash.processBytes(&head[0x344], end - 0x344);
    uint8_t digest[20];
    hash.finalize(digest);
    p.content_id_valid = std::equal(std::begin(digest), std::end(digest), p.content_id.begin());
  }
  return p;
}

std::optional<TitleUpdatePackage> ReadTitleUpdatePackage(const fs::path& path, std::string* error) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    if (error) {
      *error = "the file can't be opened";
    }
    return std::nullopt;
  }
  std::vector<uint8_t> head(0x20000);
  file.read(reinterpret_cast<char*>(head.data()), std::streamsize(head.size()));
  head.resize(size_t(file.gcount()));
  return ReadTitleUpdatePackage(head, error);
}

std::string CheckTitleUpdatePackage(const TitleUpdatePackage& package,
                                    const PPCTitleUpdate& expected, uint32_t title_id) {
  if (package.content_type != kTitleUpdateContentType) {
    return "it isn't a title update";
  }
  if (package.title_id != title_id) {
    return fmt::format("it's for another game (title {:08X})", package.title_id);
  }
  const std::string media = Upper(expected.media_id ? expected.media_id : "");
  if (!media.empty() && fmt::format("{:08X}", package.media_id) != media) {
    return fmt::format("it's for another disc (media {:08X})", package.media_id);
  }
  if (expected.base_version && package.base_version != expected.base_version) {
    return "it updates a different version of the game";
  }
  if (!package.content_id_valid) {
    return "it is damaged (its content ID doesn't match its contents)";
  }
  const std::string content = Upper(expected.content_id ? expected.content_id : "");
  if (!content.empty() && HexString(package.content_id) != content) {
    return fmt::format("it isn't title update {}", expected.version);
  }
  return {};
}

fs::path TitleUpdateFolder(const fs::path& local_dir, uint32_t version) {
  return local_dir / "title_updates" / std::to_string(version);
}

fs::path FindInstalledTitleUpdate(const fs::path& local_dir, uint32_t version) {
  std::error_code ec;
  for (const auto& entry : fs::directory_iterator(TitleUpdateFolder(local_dir, version), ec)) {
    if (entry.is_regular_file(ec) && entry.path().extension() != ".part") {
      return entry.path();
    }
  }
  return {};
}

std::string InstallTitleUpdate(const fs::path& package_file, const PPCTitleUpdate& expected,
                               uint32_t title_id, const fs::path& local_dir) {
  std::string error;
  auto package = ReadTitleUpdatePackage(package_file, &error);
  if (!package) {
    return error;
  }
  if (auto why = CheckTitleUpdatePackage(*package, expected, title_id); !why.empty()) {
    return why;
  }
  const fs::path folder = TitleUpdateFolder(local_dir, expected.version);
  std::error_code ec;
  fs::create_directories(folder, ec);

  fs::path name = package_file.filename();
  if (name.extension() == ".part") {
    name.replace_extension();
  }
  const fs::path target = folder / name;
  const fs::path staging = folder / (package_file.filename().string() + ".copy.part");
  if (package_file != staging &&
      !fs::copy_file(package_file, staging, fs::copy_options::overwrite_existing, ec)) {
    return fmt::format("it couldn't be copied ({})", ec.message());
  }

  for (const auto& entry : fs::directory_iterator(folder, ec)) {
    if (entry.path() != staging) {
      fs::remove(entry.path(), ec);
    }
  }
  fs::rename(staging, target, ec);
  if (ec) {
    return fmt::format("it couldn't be installed ({})", ec.message());
  }
  REXLOG_INFO("Title update {} installed: {}", expected.version, target.string());
  return {};
}

std::string DownloadFile(const std::string& url, const fs::path& destination,
                         const DownloadProgress& progress, const std::atomic<bool>& cancel) {
  Request r;
  if (auto why = Open(url, r); !why.empty()) {
    return why;
  }
  std::ofstream out(destination, std::ios::binary | std::ios::trunc);
  if (!out) {
    return "the download can't be saved";
  }
  std::vector<char> buffer(1 << 16);
  uint64_t done = 0;
  for (;;) {
    if (cancel.load()) {
      return "cancelled";
    }
    DWORD read = 0;
    if (!WinHttpReadData(r.request.h, buffer.data(), DWORD(buffer.size()), &read)) {
      return fmt::format("the download stopped (error {})", GetLastError());
    }
    if (!read) {
      break;
    }
    out.write(buffer.data(), read);
    done += read;
    if (progress) {
      progress(done, r.length);
    }
  }
  if (r.length && done != r.length) {
    return "the download was cut short";
  }
  return out ? std::string() : std::string("the download can't be saved");
}

std::optional<std::string> FetchText(const std::string& url, std::string* error) {
  Request r;
  if (auto why = Open(url, r); !why.empty()) {
    if (error) {
      *error = why;
    }
    return std::nullopt;
  }
  std::string text;
  char buffer[8192];
  for (DWORD read = 0; WinHttpReadData(r.request.h, buffer, sizeof(buffer), &read) && read;) {
    text.append(buffer, read);
    if (text.size() > (8u << 20)) {
      break;
    }
  }
  return text;
}

std::optional<std::string> FindXboxUnityUpdateId(std::string_view info_json,
                                                 std::string_view content_id) {
  const nlohmann::json info = nlohmann::json::parse(info_json, nullptr, false);
  if (info.is_discarded() || !info.is_object()) {
    return std::nullopt;
  }
  const std::string wanted = Upper(content_id);
  auto search = [&](const nlohmann::json& updates) -> std::optional<std::string> {
    if (!updates.is_array()) {
      return std::nullopt;
    }
    for (const auto& u : updates) {
      if (u.is_object() && Upper(u.value("hash", "")) == wanted) {
        return u.value("TitleUpdateID", "");
      }
    }
    return std::nullopt;
  };
  if (info.contains("MediaIDS") && info["MediaIDS"].is_array()) {
    for (const auto& media : info["MediaIDS"]) {
      if (media.is_object() && media.contains("Updates")) {
        if (auto id = search(media["Updates"])) {
          return id;
        }
      }
    }
  }
  if (info.contains("Updates")) {
    return search(info["Updates"]);
  }
  return std::nullopt;
}

TitleUpdateSource XboxUnitySource() {
  return {"Xbox Unity",
          [](const PPCTitleUpdate& update, uint32_t title_id,
             std::string* error) -> std::optional<std::string> {
            constexpr std::string_view kBase = "https://xboxunity.net/Resources/Lib/";
            auto info = FetchText(
                fmt::format("{}TitleUpdateInfo.php?titleid={:08X}", kBase, title_id), error);
            if (!info) {
              return std::nullopt;
            }
            auto id = FindXboxUnityUpdateId(*info, update.content_id ? update.content_id : "");
            if (!id || id->empty()) {
              if (error) {
                *error = fmt::format("it doesn't list title update {}", update.version);
              }
              return std::nullopt;
            }
            return fmt::format("{}TitleUpdate.php?tuid={}", kBase, *id);
          }};
}

std::vector<TitleUpdateSource> TitleUpdateSources() {
  std::vector<TitleUpdateSource> sources{XboxUnitySource()};
  const std::string list = REXCVAR_GET(title_update_sources);
  size_t start = 0;
  while (start < list.size()) {
    size_t end = list.find(';', start);
    std::string pattern = list.substr(start, end == std::string::npos ? end : end - start);
    start = end == std::string::npos ? list.size() : end + 1;
    if (pattern.empty()) {
      continue;
    }
    sources.push_back({pattern,
                       [pattern](const PPCTitleUpdate& update, uint32_t title_id,
                                 std::string*) -> std::optional<std::string> {
                         return Fill(pattern, update, title_id);
                       }});
  }
  return sources;
}

bool DownloadTitleUpdate(const PPCTitleUpdate& update, uint32_t title_id, const fs::path& local_dir,
                         const std::vector<TitleUpdateSource>& sources,
                         const DownloadProgress& progress, const std::atomic<bool>& cancel,
                         std::vector<std::string>* errors) {
  const fs::path folder = TitleUpdateFolder(local_dir, update.version);
  std::error_code ec;
  fs::create_directories(folder, ec);
  const fs::path part = folder / fmt::format("title_update_{}.part", update.version);
  for (const auto& source : sources) {
    if (cancel.load()) {
      break;
    }
    std::string why;
    auto url = source.find_url(update, title_id, &why);
    if (url) {
      REXLOG_INFO("Title update {}: downloading from {} ({})", update.version, source.name, *url);
      why = DownloadFile(*url, part, progress, cancel);
      if (why.empty()) {
        why = InstallTitleUpdate(part, update, title_id, local_dir);
        if (why.empty()) {
          return true;
        }
      }
    }
    REXLOG_WARN("Title update {}: {} failed: {}", update.version, source.name, why);
    if (errors) {
      errors->push_back(fmt::format("{}: {}", source.name, why));
    }
  }
  fs::remove(part, ec);
  return false;
}

namespace {

std::mutex jobs_mutex;
std::vector<std::shared_ptr<TitleUpdateJob>>& Jobs() {
  static auto* jobs = new std::vector<std::shared_ptr<TitleUpdateJob>>();
  return *jobs;
}

std::shared_ptr<TitleUpdateJob> StartJob(const PPCTitleUpdate& update, bool from_file,
                                         std::function<void(TitleUpdateJob&)> work) {
  std::lock_guard lock(jobs_mutex);
  for (const auto& job : Jobs()) {
    if (job->update.version == update.version &&
        job->state.load() == TitleUpdateJob::State::kRunning) {
      return job;
    }
  }
  auto job = std::make_shared<TitleUpdateJob>();
  job->update = update;
  job->from_file = from_file;
  Jobs().push_back(job);
  std::thread([job, work = std::move(work)] { work(*job); }).detach();
  return job;
}

}

std::shared_ptr<TitleUpdateJob> StartTitleUpdateDownload(const PPCTitleUpdate& update,
                                                         uint32_t title_id,
                                                         const fs::path& local_dir) {
  return StartJob(update, false, [title_id, local_dir](TitleUpdateJob& job) {
    std::vector<std::string> errors;
    const bool installed = DownloadTitleUpdate(
        job.update, title_id, local_dir, TitleUpdateSources(),
        [&job](uint64_t done, uint64_t total) {
          job.done = done;
          job.total = total;
        },
        job.cancel, &errors);
    job.errors = std::move(errors);
    job.state = installed           ? TitleUpdateJob::State::kInstalled
                : job.cancel.load() ? TitleUpdateJob::State::kCancelled
                                    : TitleUpdateJob::State::kFailed;
  });
}

std::shared_ptr<TitleUpdateJob> StartTitleUpdateInstall(const PPCTitleUpdate& update,
                                                        uint32_t title_id,
                                                        const fs::path& local_dir,
                                                        const fs::path& package_file) {
  return StartJob(update, true, [title_id, local_dir, package_file](TitleUpdateJob& job) {
    std::error_code ec;
    job.total = fs::file_size(package_file, ec);
    const std::string why = InstallTitleUpdate(package_file, job.update, title_id, local_dir);
    if (!why.empty()) {
      REXLOG_WARN("Title update {}: installing {} failed: {}", job.update.version,
                  path_to_utf8(package_file), why);
      job.errors.push_back(fmt::format("{}: {}", path_to_utf8(package_file.filename()), why));
    }
    job.done = job.total.load();
    job.state = why.empty() ? TitleUpdateJob::State::kInstalled : TitleUpdateJob::State::kFailed;
  });
}

std::shared_ptr<TitleUpdateJob> FindTitleUpdateJob(uint32_t version) {
  std::lock_guard lock(jobs_mutex);
  for (auto it = Jobs().rbegin(); it != Jobs().rend(); ++it) {
    if ((*it)->update.version == version) {
      return *it;
    }
  }
  return nullptr;
}

std::vector<std::shared_ptr<TitleUpdateJob>> TitleUpdateJobs() {
  std::lock_guard lock(jobs_mutex);
  return Jobs();
}

fs::path TitleUpdateExecutable(const fs::path& exe_path, uint32_t built, uint32_t version) {
  std::string stem = exe_path.stem().string();
  if (built) {
    const std::string suffix = fmt::format("_tu{}", built);
    if (stem.size() > suffix.size() && stem.ends_with(suffix)) {
      stem.resize(stem.size() - suffix.size());
    }
  }
  if (version) {
    stem += fmt::format("_tu{}", version);
  }
  return exe_path.parent_path() / (stem + exe_path.extension().string());
}

LaunchChoice ChooseLaunch(uint32_t built, uint32_t wanted, const fs::path& exe_path,
                          const fs::path& local_dir, bool handed_off) {
  std::error_code ec;
  LaunchChoice choice;

  uint32_t target = wanted;
  if (target) {
    if (FindInstalledTitleUpdate(local_dir, target).empty()) {
      choice.note =
          fmt::format("title update {} is on but not installed; running the original", target);
      target = 0;
    } else if (!fs::is_regular_file(TitleUpdateExecutable(exe_path, built, target), ec)) {
      choice.note =
          fmt::format("title update {} is on but this build has no executable for it", target);
      target = 0;
    }
  }
  if (target == built || handed_off) {
    if (built) {
      choice.update = FindInstalledTitleUpdate(local_dir, built);
      if (choice.update.empty()) {
        const fs::path original = TitleUpdateExecutable(exe_path, built, 0);
        if (!handed_off && fs::is_regular_file(original, ec)) {
          choice.hand_off = true;
          choice.executable = original;
        } else {
          choice.cannot_run = true;
        }
      }
    }
    return choice;
  }
  const fs::path executable = TitleUpdateExecutable(exe_path, built, target);
  if (!fs::is_regular_file(executable, ec)) {
    choice.update = FindInstalledTitleUpdate(local_dir, built);
    choice.cannot_run = choice.update.empty();
    return choice;
  }
  choice.hand_off = true;
  choice.executable = executable;
  if (choice.note.empty()) {
    choice.note = target ? fmt::format("title update {} is on", target)
                         : std::string("title update is off; running the original");
  }
  return choice;
}

}
