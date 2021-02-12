// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <cstring>
#include <iostream>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "bundler/Bundler.h"
#include "bundler/prefab/BinaryGltfConverter.h"
#include "bundler/prefab/TextGltfConverter.h"
#include "bundler/script/WasmConverter.h"
#include "log/log.h"
#include "types/build_config.h"

using namespace mondradiko;  // NOLINT using is ok because this is an entrypoint

struct BundlerArgs {
  std::string manifest_file;

  int parse(int, const char * const[]);
};

int BundlerArgs::parse(int argc, const char * const argv[]) {
  CLI::App app("Mondradiko bundler");

  app.set_version_flag("-v,--version", std::string(MONDRADIKO_VERSION));

  app.add_option("manifest_file", manifest_file, "bundler-manifest.toml")
    ->required()->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);
  return -1;
}

int main(int argc, char* argv[]) {
  BundlerArgs args;
  int parse_result = args.parse(argc, argv);
  if (parse_result != -1) return parse_result;

  log_msg_fmt("%s bundler version %s", MONDRADIKO_NAME, MONDRADIKO_VERSION);
  log_msg_fmt("%s", MONDRADIKO_COPYRIGHT);
  log_msg_fmt("%s", MONDRADIKO_LICENSE);

  try {
    Bundler bundler(args.manifest_file);

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
