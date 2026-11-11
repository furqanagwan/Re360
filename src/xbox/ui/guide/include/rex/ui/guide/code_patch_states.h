/**
 * @file        ui/guide/include/rex/ui/guide/code_patch_states.h
 * @brief       Saving the player's switchable code patch choices (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <string>

#include <rex/image_info.h>

namespace rex::ui::guide {

void ApplySavedCodePatches(const PPCSwitchablePatch* patches);

std::string SaveCodePatchStates(const PPCSwitchablePatch* patches);

}
