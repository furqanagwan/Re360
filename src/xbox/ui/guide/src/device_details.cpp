/**
 * @file        ui/guide/src/device_details.cpp
 * @brief       What an audio output or display supports, as the guide's pages describe it
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/device_details.h>

#include <vector>

#include <fmt/format.h>

namespace rex::ui::guide {
namespace {

constexpr std::string_view kLineBreak = "\r\n";

std::string JoinedList(const std::vector<std::string>& items, std::string_view last_joint) {
  std::string joined;
  for (size_t i = 0; i < items.size(); ++i) {
    if (i > 0) {
      joined += i + 1 == items.size() ? last_joint : ", ";
    }
    joined += items[i];
  }
  return joined;
}

std::string Kilohertz(uint32_t sample_rate) {
  return sample_rate % 1000 ? fmt::format("{:.1f} kHz", double(sample_rate) / 1000.0)
                            : fmt::format("{} kHz", sample_rate / 1000);
}

void AddLine(std::string& text, std::string_view line) {
  if (!text.empty()) {
    text += kLineBreak;
  }
  text += line;
}

}

std::string DescribeAudioOutput(const audio::AudioOutput& output) {
  std::string text;
  std::vector<std::string> format;
  if (const std::string layout = audio::SpeakerLayoutName(output.channels, output.channel_mask);
      !layout.empty()) {
    format.push_back(layout);
  }
  if (output.sample_rate) {
    format.push_back(Kilohertz(output.sample_rate));
  }
  if (output.bits_per_sample) {
    format.push_back(fmt::format("{}-bit", output.bits_per_sample));
  }
  if (!format.empty()) {
    std::string line = format.front();
    for (size_t i = 1; i < format.size(); ++i) {
      line += ", " + format[i];
    }
    AddLine(text, line + ".");
  }
  if (output.channels > 2) {
    AddLine(text, "The game plays in 5.1 surround here.");
  } else if (output.channels > 0) {
    AddLine(text, "The game's 5.1 sound is mixed down to stereo here.");
  }
  if (output.spatial_sound) {
    AddLine(text, "Spatial sound is on for this output in Windows.");
  }

  std::vector<std::string> decodes;
  std::vector<std::string> does_not;
  const std::pair<audio::Support, std::string_view> formats[] = {
      {output.formats.dolby_digital, "Dolby Digital"},
      {output.formats.dolby_digital_plus, "Dolby Digital Plus"},
      {output.formats.dolby_truehd, "Dolby TrueHD (Atmos)"},
      {output.formats.dts, "DTS"},
      {output.formats.dts_hd, "DTS-HD"},
  };
  for (const auto& [support, name] : formats) {
    if (support == audio::Support::kYes) {
      decodes.emplace_back(name);
    } else if (support == audio::Support::kNo) {
      does_not.emplace_back(name);
    }
  }
  if (!decodes.empty() || !does_not.empty()) {
    text += kLineBreak;
  }
  if (!decodes.empty()) {
    AddLine(text, "Decodes " + JoinedList(decodes, " and ") + ".");
  }
  if (!does_not.empty()) {
    AddLine(text,
            (decodes.empty() ? "Doesn't decode " : "Not ") + JoinedList(does_not, " or ") + ".");
  }
  if (!decodes.empty()) {
    AddLine(text, "Those are for films and other apps; the game sends its surround as PCM.");
  }
  return text;
}

std::string DescribeDisplay(const DisplayInfo& display) {
  std::string text;
  if (display.primary && display.shows_game) {
    AddLine(text, "The main display. The game is on it.");
  } else if (display.primary) {
    AddLine(text, "The main display.");
  } else if (display.shows_game) {
    AddLine(text, "The game is on this display.");
  }
  if (display.width && display.height) {
    AddLine(text, fmt::format("Now {} x {} at {} Hz.", display.width, display.height,
                              display.refresh_hz));
  }
  if (!display.resolutions.empty()) {
    text += kLineBreak;
  }
  for (const DisplayResolution& resolution : display.resolutions) {
    std::string name = ResolutionName(resolution.width, resolution.height);
    if (name.empty()) {
      name = fmt::format("{} x {}", resolution.width, resolution.height);
    }
    if (resolution.width == display.native_width && resolution.height == display.native_height) {
      name += ", native";
    }
    AddLine(text, fmt::format("{}: up to {} Hz", name, resolution.max_refresh_hz));
  }
  text += kLineBreak;
  AddLine(text, display.hdr_supported
                    ? (display.hdr_on ? "HDR: on" : "HDR: supported, off in Windows")
                    : "HDR: not supported");
  if (display.peak_nits > 0.0f) {
    AddLine(text, fmt::format("Peak brightness: about {} nits", int(display.peak_nits + 0.5f)));
  }
  if (display.bits_per_color) {
    AddLine(text, fmt::format("Colour: {} bits per channel", display.bits_per_color));
  }
  text += kLineBreak;
  AddLine(text, "The game draws in SDR.");
  return text;
}

}
