/**
 * @file        regpu/plugin_smoke_test.cpp
 * @brief       rexgpu-regpu runs a command stream, shaded draws and a present through the plugin
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <future>
#include <string>
#include <thread>
#include <vector>

#include <winsock2.h>

#include <windows.h>

#include <fmt/format.h>

#include <rex/graphics/xenos.h>
#include <rex/logging.h>
#include <rex/system/interfaces/graphics.h>
#include <rex/system/mmio_handler.h>

#include "gpu_fixture.h"
#include "guest_draw.h"

namespace {

namespace xenos = rex::graphics::xenos;
using rex::testing::GpuFixture;
using namespace rex::testing::guest_draw;  // NOLINT

constexpr uint32_t kGpuRegisterBase = 0x7FC80000;
constexpr uint32_t kCpRbWptr = 0x01C5;
constexpr uint32_t kEdramTiming = 0x0F00;
constexpr uint32_t kSize = 32;
uint32_t frame_width = 1280;
uint32_t frame_height = 720;
constexpr uint32_t kShotWidth = 640;
constexpr uint32_t kShotHeight = 360;
constexpr uint16_t kDebugPort = 7349;
constexpr int kSkipped = 4;

int failures = 0;

void Check(bool passed, const std::string& what) {
  std::printf("%s: %s\n", passed ? "ok" : "FAILED", what.c_str());
  std::fflush(stdout);
  if (!passed) {
    ++failures;
  }
}

[[noreturn]] void Finish(const char* name) {
  std::printf("regpu %s test %s\n", name, failures ? "FAILED" : "passed");
  std::fflush(stdout);
  TerminateProcess(GetCurrentProcess(), failures ? 1 : 0);
  std::_Exit(failures ? 1 : 0);
}

void CheckDraw(GpuFixture& fixture, const Surface& surface, uint32_t color, const char* what) {
  uint32_t dest = fixture.AllocPhysical(kSize * kSize * 4);
  SetupDraw(fixture, surface, kSize, kSize);
  DrawRect(fixture, 0, 0, kSize, kSize, color);
  Resolve(fixture, surface, kSize, kSize, xenos::CopySampleSelect::k0, dest);
  bool flushed = fixture.Flush(std::chrono::seconds(60));
  uint32_t centre = ReadTexel(fixture, dest, kSize / 2, kSize / 2);
  uint32_t corner = ReadTexel(fixture, dest, 1, 1);
  std::printf("%s: centre %08X corner %08X, expected %08X\n", what, centre, corner, color);
  Check(flushed && centre == color && corner == color, what);
}

int DrawWithoutWindow() {
  _putenv_s("REGPU_HEADLESS", "1");
  _putenv_s("REGPU_RESOLVE_WRITEBACK", "32x32");

  std::string error;
  auto fixture = GpuFixture::Create(&error, {}, "regpu");
  if (!fixture) {
    std::printf("skipped: %s\n", error.c_str());
    return kSkipped;
  }
  Check(true, "the command processor runs a stream to its fence through the plugin");

  uint32_t scratch = fixture->AllocPhysical(0x1000);
  fixture->Submit(GpuFixture::MemWrite(scratch, {0x52653336, 0x30475055}, xenos::Endian::k8in32));
  Check(fixture->Flush() && fixture->ReadDword(scratch) == 0x52653336 &&
            fixture->ReadDword(scratch + 4) == 0x30475055,
        "MEM_WRITE lands in guest physical memory");

  auto* mmio = rex::runtime::MMIOHandler::global_handler();
  uint32_t value = 0;
  Check(
      mmio->CheckLoad(kGpuRegisterBase + kCpRbWptr * 4, &value) && value == fixture->write_index(),
      "a register load returns what the guest stored");
  Check(mmio->CheckLoad(kGpuRegisterBase + kEdramTiming * 4, &value) && value == 0x08100748,
        "the EDRAM timing register reads as the console's");

  uint32_t writeback = fixture->EnableReadPointerWriteBack(2);
  fixture->Flush();
  Check(fixture->ReadDword(writeback) == fixture->write_index(),
        "the read pointer write-back reaches the write pointer");

  CheckDraw(*fixture, {xenos::MsaaSamples::k1X, kSize}, 0xFF5599CC,
            "a shaded rectangle resolves to guest memory");
  CheckDraw(*fixture, {xenos::MsaaSamples::k1X, kSize}, 0xFF102030,
            "a second colour replaces the first in the same surface");
  CheckDraw(*fixture, {xenos::MsaaSamples::k4X, kSize * 2}, 0xFF336699,
            "a 4x MSAA surface resolves one sample");

  fixture->graphics_system()->FlushCaches();
  Finish("draw");
}

HWND CreateTitleWindow() {
  std::promise<HWND> created;
  std::thread([&created] {
    WNDCLASSW window_class{};
    window_class.lpfnWndProc = DefWindowProcW;
    window_class.hInstance = GetModuleHandleW(nullptr);
    window_class.lpszClassName = L"re360_regpu_present_test";
    RegisterClassW(&window_class);
    RECT rect{0, 0, 960, 540};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND window =
        CreateWindowW(window_class.lpszClassName, L"Re360 ReGPU present test", WS_OVERLAPPEDWINDOW,
                      40, 40, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr,
                      window_class.hInstance, nullptr);
    ShowWindow(window, SW_SHOWNOACTIVATE);
    created.set_value(window);
    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
      TranslateMessage(&message);
      DispatchMessageW(&message);
    }
  }).detach();
  return created.get_future().get();
}

std::string AskDebugServer(const std::string& command) {
  WSADATA startup;
  WSAStartup(MAKEWORD(2, 2), &startup);
  SOCKET connection = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(kDebugPort);
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (connect(connection, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
    closesocket(connection);
    return {};
  }
  std::string line = command + "\n";
  send(connection, line.data(), int(line.size()), 0);
  std::string reply;
  char buffer[512];
  while (reply.find('\n') == std::string::npos) {
    int received = recv(connection, buffer, sizeof(buffer), 0);
    if (received <= 0) {
      break;
    }
    reply.append(buffer, size_t(received));
  }
  closesocket(connection);
  return reply;
}

void SubmitFrame(GpuFixture& fixture, uint32_t front_buffer, uint32_t color) {
  const Surface surface{xenos::MsaaSamples::k1X, frame_width};
  SetupDraw(fixture, surface, frame_width, frame_height);
  DrawRect(fixture, 0, 0, frame_width, frame_height, color);
  Resolve(fixture, surface, frame_width, frame_height, xenos::CopySampleSelect::k0, front_buffer,
          frame_width, 0, {}, frame_height);
  fixture.Submit({xenos::MakePacketType3(xenos::PM4_XE_SWAP, 4), xenos::kSwapSignature,
                  front_buffer, frame_width, frame_height});
  fixture.Flush(std::chrono::seconds(60));
}

int PresentIntoTitleWindow() {
  _putenv_s("REGPU_DEBUG", "1");
  _putenv_s("REGPU_DEBUG_PORT", std::to_string(kDebugPort).c_str());
  HWND window = CreateTitleWindow();

  std::string error;
  auto fixture = GpuFixture::Create(&error, {}, "regpu", window);
  if (!fixture) {
    std::printf("skipped: %s\n", error.c_str());
    return kSkipped;
  }
  uint32_t front_buffer = fixture->AllocPhysical(frame_width * frame_height * 4);
  constexpr uint32_t kColor = 0xFF2060C0;
  SubmitFrame(*fixture, front_buffer, kColor);

  std::filesystem::path shot = std::filesystem::temp_directory_path() /
                               fmt::format("re360_regpu_present_{}.raw", GetCurrentProcessId());
  std::atomic<bool> answered{false};
  std::string reply;
  std::thread asker([&] {
    reply = AskDebugServer("shot " + shot.string());
    answered = true;
  });
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(20);
  uint32_t frames = 1;
  while (!answered && std::chrono::steady_clock::now() < deadline) {
    SubmitFrame(*fixture, front_buffer, kColor);
    ++frames;
  }
  asker.join();
  std::printf("%u frames; debug server: %s", frames, reply.empty() ? "no reply\n" : reply.c_str());
  Check(reply.find("\"ok\":true") != std::string::npos,
        "a frame is presented into the title window after XE_SWAP");

  std::vector<uint8_t> pixels(kShotWidth * kShotHeight * 4);
  bool read = false;
  if (FILE* file = _wfopen(shot.c_str(), L"rb")) {
    read = std::fread(pixels.data(), 1, pixels.size(), file) == pixels.size();
    std::fclose(file);
  }
  std::error_code ignored;
  std::filesystem::remove(shot, ignored);
  const uint8_t* centre = &pixels[((kShotHeight / 2) * kShotWidth + kShotWidth / 2) * 4];
  const uint8_t* corner = &pixels[((kShotHeight - 8) * kShotWidth + kShotWidth - 8) * 4];
  std::printf(
      "%ux%u frame presented: centre R %u G %u B %u, bottom right R %u G %u B %u "
      "(the frame drew R 192 G 96 B 32)\n",
      frame_width, frame_height, centre[0], centre[1], centre[2], corner[0], corner[1], corner[2]);
  auto drawn = [](const uint8_t* pixel) {
    return pixel[0] == 192 && pixel[1] == 96 && pixel[2] == 32;
  };
  Check(read && drawn(centre), "the presented picture is the colour the frame drew");
  Check(read && drawn(corner), "the frame fills the picture to its bottom right corner");
  Finish("present");
}

}

int main(int argc, char** argv) {
  if (argc > 1 && std::string(argv[1]) == "--present") {
    if (argc > 3) {
      frame_width = uint32_t(std::atoi(argv[2]));
      frame_height = uint32_t(std::atoi(argv[3]));
    }
    return PresentIntoTitleWindow();
  }
  return DrawWithoutWindow();
}
