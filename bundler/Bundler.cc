/**
 * @file Bundler.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Bundles a directory and its contents into a loadable
 * AssetBundle.
 * @date 2021-01-22
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "bundler/Bundler.h"

#include <string>

#include "assets/saving/AssetBundleBuilder.h"
#include "converter/convert_gltf.h"
#include "converter/wasm_converter.h"
#include "log/log.h"

namespace mondradiko {

Bundler::Bundler(const std::filesystem::path& _manifest_path)
    : manifest_path(_manifest_path) {
  if (std::filesystem::is_directory(manifest_path)) {
    manifest_path = manifest_path / "bundler-manifest.toml";
  }

  if (!std::filesystem::exists(manifest_path)) {
    log_ftl("Manifest path %s does not exist", manifest_path.c_str());
  }

  source_root = manifest_path.parent_path();
  bundle_root = "./";

  log_dbg("Bundler manifest: %s", manifest_path.c_str());
  log_dbg("Bundler source dir: %s", source_root.c_str());
  log_dbg("Bundler bundle dir: %s", bundle_root.c_str());

  {
    log_inf("Loading manifest");

    manifest = toml::parse(manifest_path);
  }
}

Bundler::~Bundler() {}

void Bundler::bundle() {
  assets::AssetBundleBuilder bundle_builder(bundle_root);

  const auto assets = toml::find<toml::array>(manifest, "assets");

  for (const auto& asset_table : assets) {
    const auto& asset = asset_table.as_table();

    const auto& asset_file = asset.at("file").as_string();
    auto asset_path = source_root / asset_file.str;

    // TODO(marceline-cramer) Convert other asset types
    convert_gltf(&bundle_builder, asset_path);
  }

  bundle_builder.buildBundle("registry.bin");
}

}  // namespace mondradiko
