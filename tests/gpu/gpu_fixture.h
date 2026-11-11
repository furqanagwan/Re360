/**
 * @file        gpu_fixture.h
 * @brief       Headless PM4 fixture host for the Xenos GPU plugin (RG-GDK-006)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <rex/graphics/xenos.h>
#include <rex/runtime.h>
#include <rex/ui/d3d12/d3d12_provider.h>

namespace rex::testing {

class GpuFixture {
 public:
  using CvarList = std::initializer_list<std::pair<const char*, const char*>>;

  static std::unique_ptr<GpuFixture> Create(std::string* error, CvarList cvars = {},
                                            std::string_view plugin = "xenos",
                                            void* host_window = nullptr);
  ~GpuFixture();

  memory::Memory* memory() const { return runtime_->memory(); }
  system::IGraphicsSystem* graphics_system() const { return runtime_->graphics_system(); }
  const ui::d3d12::D3D12Provider& provider() const;

  uint32_t AllocPhysical(uint32_t size, uint32_t alignment = 0x1000);

  void WriteDwords(uint32_t address, const std::vector<uint32_t>& dwords);

  void WriteDwordsAsGuest(uint32_t address, const std::vector<uint32_t>& dwords);
  uint32_t ReadDword(uint32_t address) const;

  bool Submit(const std::vector<uint32_t>& dwords);

  uint32_t EnableReadPointerWriteBack(uint32_t block_size_log2);
  uint32_t ring_dwords() const { return kRingDwords; }
  uint32_t write_index() const { return write_index_; }

  bool Flush(std::chrono::milliseconds timeout = std::chrono::seconds(10));

  std::string Metadata() const;

  static std::vector<uint32_t> MemWrite(uint32_t address, std::initializer_list<uint32_t> values,
                                        graphics::xenos::Endian endian);
  static std::vector<uint32_t> SetRegisters(uint32_t first_register,
                                            std::initializer_list<uint32_t> values);
  static std::vector<uint32_t> RegToMem(uint32_t reg, uint32_t address,
                                        graphics::xenos::Endian endian);
  static std::vector<uint32_t> IndirectBuffer(uint32_t address, uint32_t dword_count);

 private:
  GpuFixture() = default;

  static constexpr uint32_t kRingSizeLog2 = 13;
  static constexpr uint32_t kRingDwords = (uint32_t(1) << (kRingSizeLog2 + 3)) / 4;

  std::filesystem::path root_;
  std::unique_ptr<Runtime> runtime_;
  bool plugin_runs_until_exit_ = false;
  uint32_t ring_ = 0;
  uint32_t write_index_ = 0;
  uint32_t fence_address_ = 0;
  uint32_t fence_value_ = 0;
  uint32_t read_pointer_writeback_ = 0;

  uint32_t physical_to_virtual_ = 0;
};

}
