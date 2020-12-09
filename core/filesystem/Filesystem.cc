/**
 * @file Filesystem.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to the read-only files in an asset archive.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/filesystem/Filesystem.h"

#include <sstream>

#include "core/log/log.h"

namespace mondradiko {

Filesystem::Filesystem(const std::filesystem::path& asset_bundle_root)
    : asset_bundle(asset_bundle_root) {
  log_zone;

  auto result = asset_bundle.loadRegistry("registry.bin");
  if (result != assets::AssetResult::Success) {
    log_err(assets::getAssetResultString(result));
    log_ftl("Failed to load asset bundle registry.");
  }
}

Filesystem::~Filesystem() {}

}  // namespace mondradiko
