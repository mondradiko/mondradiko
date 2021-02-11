// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <cstring>
#include <iostream>

#include "bundler/Bundler.h"
#include "bundler/prefab/BinaryGltfConverter.h"
#include "bundler/prefab/TextGltfConverter.h"
#include "bundler/script/WasmConverter.h"
#include "log/log.h"

using namespace mondradiko;  // NOLINT using is ok because this is an entrypoint

void print_usage(const char* arg1) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "  " << arg1 << " bundler-manifest.toml" << std::endl;
}

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return 1;
  }

  const char* manifest_file = argv[1];

  try {
    Bundler bundler(manifest_file);

    BinaryGltfConverter binary_gltf_converter(&bundler);
    bundler.addConverter("glb", &binary_gltf_converter);
    bundler.addConverter("vrm", &binary_gltf_converter);

    TextGltfConverter text_gltf_converter(&bundler);
    bundler.addConverter("gltf", &text_gltf_converter);

    WasmConverter wasm_converter(&bundler);
    bundler.addConverter("wat", &wasm_converter);

    bundler.bundle();
  } catch (const std::exception& e) {
    log_err_fmt("Mondradiko bundler failed with message: %s", e.what());
    return 1;
  }

  return 0;
}
