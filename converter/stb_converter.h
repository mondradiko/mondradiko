/**
 * @file stb_converter.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates a TextureAsset from an STB-loaded image.
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "converter/converter.h"

namespace mondradiko {

assets::AssetId stb_convert(assets::AssetBundleBuilder*, std::filesystem::path);

}
