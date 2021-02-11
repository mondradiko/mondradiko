// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>

#include "assets/common/AssetTypes.h"
#include "types/assets/SerializedAsset_generated.h"
#include "types/assets/types_generated.h"

namespace mondradiko {
namespace assets {

class AssetLump {
 public:
  explicit AssetLump(const std::filesystem::path&);
  ~AssetLump();

  bool assertFileSize(size_t);
  bool assertHash(LumpHashMethod, LumpHash);

  void decompress(LumpCompressionMethod);

  bool loadAsset(const SerializedAsset**, size_t, size_t);

 private:
  std::filesystem::path lump_path;

  size_t loaded_size;
  char* loaded_data = nullptr;
};

}  // namespace assets
}  // namespace mondradiko
