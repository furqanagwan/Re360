/**
 * @file        ui/xui/include/rex/ui/xui/xtt_font.h
 * @brief       The console's XTT system fonts as standard TrueType (RG-GDK-061)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace rex::ui::xui {

std::optional<std::vector<uint8_t>> XttToTrueType(std::span<const uint8_t> xtt, std::string* error);

}
