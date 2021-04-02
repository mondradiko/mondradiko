// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GpuImage;
class MeshPass;

class TextureAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::TextureAsset);

  // Asset lifetime implementation
  TextureAsset() : mesh_pass(nullptr) {}
  explicit TextureAsset(MeshPass* mesh_pass) : mesh_pass(mesh_pass) {}
  ~TextureAsset();

  GpuImage* getImage() const { return image; }

 protected:
  // Asset implementation
  bool _load(const assets::SerializedAsset*) final;

 private:
  MeshPass* mesh_pass;

  GpuImage* image = nullptr;
};

}  // namespace core
}  // namespace mondradiko
