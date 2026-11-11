/**
 * @file        ui/guide/tests/device_details_test.cpp
 * @brief       The guide's descriptions of audio outputs and displays
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <string>

#include <catch2/catch_test_macros.hpp>

#include <rex/ui/guide/device_details.h>

using namespace rex::ui::guide;

TEST_CASE("A surround output lists its speakers and the formats it decodes",
          "[guide][device_details]") {
  rex::audio::AudioOutput output;
  output.channels = 8;
  output.channel_mask = 0x63F;
  output.sample_rate = 48000;
  output.bits_per_sample = 24;
  output.spatial_sound = true;
  output.formats.dolby_digital = rex::audio::Support::kYes;
  output.formats.dolby_digital_plus = rex::audio::Support::kYes;
  output.formats.dolby_truehd = rex::audio::Support::kYes;
  output.formats.dts = rex::audio::Support::kNo;
  output.formats.dts_hd = rex::audio::Support::kNo;
  CHECK(DescribeAudioOutput(output) ==
        "7.1 surround, 48 kHz, 24-bit.\r\n"
        "The game plays in 5.1 surround here.\r\n"
        "Spatial sound is on for this output in Windows.\r\n"
        "\r\n"
        "Decodes Dolby Digital, Dolby Digital Plus and Dolby TrueHD (Atmos).\r\n"
        "Not DTS or DTS-HD.\r\n"
        "Those are for films and other apps; the game sends its surround as PCM.");
}

TEST_CASE("A stereo output folds the game's 5.1 and skips unknown formats",
          "[guide][device_details]") {
  rex::audio::AudioOutput output;
  output.channels = 2;
  output.channel_mask = 0x3;
  output.sample_rate = 44100;
  output.bits_per_sample = 16;
  output.formats.dolby_digital = rex::audio::Support::kNo;
  CHECK(DescribeAudioOutput(output) ==
        "Stereo, 44.1 kHz, 16-bit.\r\n"
        "The game's 5.1 sound is mixed down to stereo here.\r\n"
        "\r\n"
        "Doesn't decode Dolby Digital.");
}

TEST_CASE("A display lists its mode, resolutions, HDR and colour", "[guide][device_details]") {
  rex::ui::DisplayInfo display;
  display.primary = true;
  display.shows_game = true;
  display.width = 3840;
  display.height = 2160;
  display.refresh_hz = 144;
  display.native_width = 3840;
  display.native_height = 2160;
  display.resolutions = {{3840, 2160, 144}, {2560, 1440, 144}, {3440, 1440, 100}};
  display.hdr_supported = true;
  display.peak_nits = 702.6f;
  display.bits_per_color = 10;
  CHECK(DescribeDisplay(display) ==
        "The main display. The game is on it.\r\n"
        "Now 3840 x 2160 at 144 Hz.\r\n"
        "\r\n"
        "4K UHD, native: up to 144 Hz\r\n"
        "1440p QHD: up to 144 Hz\r\n"
        "Ultrawide QHD: up to 100 Hz\r\n"
        "\r\n"
        "HDR: supported, off in Windows\r\n"
        "Peak brightness: about 703 nits\r\n"
        "Colour: 10 bits per channel\r\n"
        "\r\n"
        "The game draws in SDR.");
}

TEST_CASE("A display without HDR says so", "[guide][device_details]") {
  rex::ui::DisplayInfo display;
  display.width = 1920;
  display.height = 1080;
  display.refresh_hz = 60;
  display.resolutions = {{1600, 900, 60}};
  const std::string text = DescribeDisplay(display);
  CHECK(text.find("Now 1920 x 1080 at 60 Hz.") == 0);
  CHECK(text.find("1600 x 900: up to 60 Hz") != std::string::npos);
  CHECK(text.find("HDR: not supported") != std::string::npos);
  CHECK(text.find("Peak brightness") == std::string::npos);
}
