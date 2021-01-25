/**
 * @file ScriptComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Binds a ScriptAsset and its events to an entity.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * ScriptComponents do not inherit from the Component base class,
 * because they are network-synchronized totally differently. For
 * more info, see ScriptEnvironment.h.
 *
 */

#pragma once

#include "core/assets/ScriptAsset.h"
#include "core/components/Component.h"

namespace mondradiko {

// Forward declarations
class ScriptInstance;

class ScriptComponent {
 public:
  const AssetHandle<ScriptAsset>& getScriptAsset() { return script_asset; }

 private:
  // Systems allowed to access private members directly
  friend class ScriptEnvironment;

  AssetHandle<ScriptAsset> script_asset;
  ScriptInstance* script_instance;
};

}  // namespace mondradiko
