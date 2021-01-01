/**
 * @file ScriptComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Binds a ScriptAsset and its events to an entity.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/assets/ScriptAsset.h"
#include "core/components/Component.h"
#include "core/protocol/ScriptComponent_generated.h"

namespace mondradiko {

struct ScriptComponent : public Component<protocol::ScriptComponent> {
 public:
  explicit ScriptComponent(const protocol::ScriptComponent& data)
      : Component(data) {}

  explicit ScriptComponent(AssetId script_asset)
      : Component(protocol::ScriptComponent(
            static_cast<protocol::AssetId>(script_asset))) {}

  bool isLoaded(AssetPool* asset_pool) {
    return asset_pool->isAssetLoaded<ScriptAsset>(getScriptAsset());
  }

  AssetId getScriptAsset() {
    return static_cast<AssetId>(_data.script_asset());
  }
};

}  // namespace mondradiko
