// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>

#include "types/assets/AssetTypes.h"
#include "types/assets/SerializedAsset_generated.h"
#include "types/assets/types_generated.h"

namespace mondradiko {
namespace core {

class AssetLump {
 public:
  explicit AssetLump(const std::filesystem::path&);
  ~AssetLump();

  bool assertFileSize(size_t);
  bool assertHash(assets::LumpHashMethod, assets::LumpHash);

  void decompress(assets::LumpCompressionMethod);

  bool loadAsset(const assets::SerializedAsset**, size_t, size_t);

 private:
  std::filesystem::path lump_path;

  size_t loaded_size;
  char* loaded_data = nullptr;
};

}  // namespace core
}  // namespace mondradiko
