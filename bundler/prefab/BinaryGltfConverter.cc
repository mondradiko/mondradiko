/**
 * @file BinaryGltfConverter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Converts a binary glTF model into a prefab.
 * @date 2021-01-22
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "bundler/prefab/BinaryGltfConverter.h"

#include "log/log.h"

namespace mondradiko {

ConverterInterface::AssetOffset BinaryGltfConverter::convert(
    AssetBuilder* fbb, std::filesystem::path model_path) const {
  tinygltf::Model gltf_model;
  tinygltf::TinyGLTF gltf_context;
  std::string error;
  std::string warning;

  bool file_loaded = gltf_context.LoadBinaryFromFile(
      &gltf_model, &error, &warning, model_path.c_str());
  if (!file_loaded) {
    log_ftl("Failed to load model with error: %s", error.c_str());
  }

  if (warning.length() > 0) {
    log_wrn("GLTF warning: %s", warning.c_str());
  }

  return _loadModel(fbb, gltf_model);
}

}  // namespace mondradiko
