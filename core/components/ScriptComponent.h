/**
 * @file ScriptComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Binds a ScriptAsset and its events to an entity.
 * @date 2020-12-25
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * 
 */

#pragma once

#include "core/assets/Asset.h"
#include "core/components/Component.h"

namespace mondradiko {

struct ScriptComponent {
  AssetId script_asset;
};

}  // namespace mondradiko
