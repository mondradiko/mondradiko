// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/prefab/BinaryGltfConverter.h"

#include <string>

#include "converter/BundlerInterface.h"
#include "log/log.h"

namespace mondradiko {
namespace converter {

ConverterInterface::AssetOffset BinaryGltfConverter::convert(
    AssetBuilder* fbb, const toml::table& asset) const {
  auto model_path = _bundler->getAssetPath(asset);

  tinygltf::Model gltf_model;
  tinygltf::TinyGLTF gltf_context;
  std::string error;
  std::string warning;

  bool file_loaded = gltf_context.LoadBinaryFromFile(
      &gltf_model, &error, &warning, model_path.string().c_str());
  if (!file_loaded) {
    log_ftl_fmt("Failed to load model with error: %s", error.c_str());
  }

  if (warning.length() > 0) {
    log_wrn_fmt("GLTF warning: %s", warning.c_str());
  }

  return _loadModel(fbb, gltf_model);
}

}  // namespace converter
}  // namespace mondradiko
