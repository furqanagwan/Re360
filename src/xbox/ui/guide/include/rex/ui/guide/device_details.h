/**
 * @file        ui/guide/include/rex/ui/guide/device_details.h
 * @brief       What an audio output or display supports, as the guide's pages describe it
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <string>

#include <rex/audio/audio_outputs.h>
#include <rex/ui/display_info.h>

namespace rex::ui::guide {

std::string DescribeAudioOutput(const audio::AudioOutput& output);

std::string DescribeDisplay(const DisplayInfo& display);

}
