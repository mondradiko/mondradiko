/**
 * @file wasm_converter.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates ScriptAssets from compiled Wasm modules.
 * @date 2020-12-25
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * 
 */

#pragma once

#include <filesystem>

#include "assets/format/AssetTypes.h"
#include "assets/saving/AssetBundleBuilder.h"
#include "converter/converter.h"

namespace mondradiko {

assets::AssetId wat_convert(assets::AssetBundleBuilder*, std::filesystem::path);

}  // namespace mondradiko
