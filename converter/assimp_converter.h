/**
 * @file assimp_converter.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief 
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * 
 */

#pragma once

#include <filesystem>

#include "converter/converter.h"

namespace mondradiko {

bool convert_assimp(assets::AssetBundleBuilder*, std::filesystem::path);

};
