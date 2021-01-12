/**
 * @file Asset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class for a core asset and its ID.
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "assets/common/AssetTypes.h"
#include "assets/format/SerializedAsset_generated.h"

namespace mondradiko {

using AssetId = assets::AssetId;

class Asset {
 public:
  bool isLoaded() { return loaded; }

 private:
  AssetId id_;
  bool loaded;

  friend class AssetPool;
};

}  // namespace mondradiko
