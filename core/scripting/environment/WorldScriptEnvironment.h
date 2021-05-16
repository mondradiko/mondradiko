// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/environment/ScriptEnvironment.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

// Forward declarations
class World;
class WorldScript;

class WorldScriptEnvironment : public ScriptEnvironment {
 public:
  WorldScriptEnvironment(World*, const types::string&);
  ~WorldScriptEnvironment();

  void update(double);

 private:
  World* const world;

  WorldScript* _instance = nullptr;
};

}  // namespace core
}  // namespace mondradiko
