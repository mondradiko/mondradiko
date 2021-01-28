// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

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
