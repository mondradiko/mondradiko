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
#include "converter/assimp_converter.h"
#include "converter/wasm_converter.h"
#include "log/log.h"

using namespace mondradiko;  // NOLINT using is ok because this is an entrypoint

int main(int argc, const char* argv[]) {
  assets::AssetBundleBuilder bundle_builder("../test-folder/");

  const char* assimp_test = "../test-folder/DamagedHelmet.gltf";

  if (!convert_assimp(&bundle_builder, assimp_test)) {
    std::cerr << "whoops" << std::endl;
    return 1;
  }

  auto script_id = wat_convert(&bundle_builder, "../test-folder/helloworld.wat");
  log_dbg("Added ScriptAsset: 0x%0lx", script_id);

  bundle_builder.buildBundle("registry.bin");

  return 0;
}
