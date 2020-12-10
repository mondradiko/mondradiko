/**
 * @file Filesystem.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to the read-only files in an asset archive.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <filesystem>
#include <vector>

#include "assets/loading/AssetBundle.h"
#include "core/common/api_headers.h"

namespace mondradiko {

class Filesystem {
 public:
  explicit Filesystem(const std::filesystem::path&);
  ~Filesystem();

 private:
  assets::AssetBundle asset_bundle;
};

}  // namespace mondradiko
