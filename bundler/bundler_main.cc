/**
 * @file bundler_main.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Bundles a directory and its contents into a loadable
 * AssetBundle.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include <cstring>
#include <iostream>

#include "assets/saving/AssetBundleBuilder.h"
#include "converter/convert_gltf.h"
#include "converter/wasm_converter.h"
#include "log/log.h"

using namespace mondradiko;  // NOLINT using is ok because this is an entrypoint

int main(int argc, const char* argv[]) {
  assets::AssetBundleBuilder bundle_builder("../test-folder/");

  const char* test_gltf = "../test-folder/DamagedHelmet.gltf";
  assets::AssetId gltf_asset = convert_gltf(&bundle_builder, test_gltf);

  if (gltf_asset == assets::AssetId::NullAsset) {
    log_err("Failed to load test GLTF");
    return 1;
  }

  const char* test_script = "../test-folder/helloworld.wat";

  assets::AssetId script_asset = wat_convert(&bundle_builder, test_script);

  if (script_asset == assets::AssetId::NullAsset) {
    log_err("Failed to load test script");
    return 1;
  }

  log_dbg("Added GLTF asset: 0x%0lx", gltf_asset);
  log_dbg("Added ScriptAsset: 0x%0lx", script_asset);

  bundle_builder.buildBundle("registry.bin");

  return 0;
}
