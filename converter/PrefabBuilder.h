// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/toml_headers.h"
#include "types/assets/SerializedAsset_generated.h"

namespace mondradiko {

// Forward declarations
class BundlerInterface;

class PrefabBuilder {
 public:
  assets::AssetId buildPrefab(BundlerInterface*, const toml::table&);
};

}  // namespace mondradiko
