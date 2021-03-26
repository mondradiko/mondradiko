// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/prefab/TextGltfConverter.h"

#include <string>

#include "log/log.h"

namespace mondradiko {
namespace converter {

ConverterInterface::AssetOffset TextGltfConverter::convert(
    AssetBuilder* fbb, std::filesystem::path model_path) const {
  tinygltf::Model gltf_model;
  tinygltf::TinyGLTF gltf_context;
  std::string error;
  std::string warning;

  bool file_loaded = gltf_context.LoadASCIIFromFile(
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
