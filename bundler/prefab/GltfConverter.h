// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "bundler/ConverterInterface.h"
#include "lib/include/tinygltf_headers.h"

namespace mondradiko {

class GltfConverter : public ConverterInterface {
 public:
  explicit GltfConverter(Bundler*);

 protected:
  Bundler* _bundler;

  // Shorthand types for library objects
  using GltfModel = const tinygltf::Model&;
  using GltfScene = const tinygltf::Scene&;
  using GltfNode = const tinygltf::Node&;
  using GltfPrimitive = const tinygltf::Primitive&;
  using GltfMaterial = const tinygltf::Material&;
  using GltfTextureInfo = const tinygltf::TextureInfo&;
  using GltfImage = const tinygltf::Image&;

  AssetOffset _loadModel(AssetBuilder*, GltfModel) const;
  assets::AssetId _loadScene(GltfModel, GltfScene) const;
  assets::AssetId _loadNode(GltfModel, GltfNode, glm::vec3) const;
  assets::AssetId _loadPrimitive(GltfModel, GltfPrimitive, glm::vec3) const;
  assets::AssetId _loadMaterial(GltfModel, GltfMaterial) const;
  assets::AssetId _loadTexture(GltfModel, GltfTextureInfo, bool) const;
  assets::AssetId _loadImage(GltfModel, GltfImage, bool) const;
};

}  // namespace mondradiko
