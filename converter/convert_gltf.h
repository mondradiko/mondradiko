/**
 * @file convert_gltf.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Converts a glTF model into a prefab.
 * @date 2021-01-08
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "converter/converter.h"

namespace mondradiko {

assets::AssetId convert_gltf(assets::AssetBundleBuilder*,
                             const std::filesystem::path&);

}  // namespace mondradiko
