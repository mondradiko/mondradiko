// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/instance/ScriptInstance.h"

namespace mondradiko {
namespace core {

// Forward declarations
class World;

class WorldScript : public ScriptInstance {
 public:
  WorldScript(ScriptEnvironment* scripts, World* world)
      : ScriptInstance(scripts), world(world) {}

  // Publically-accessible World instance
  World* const world;
};

}  // namespace core
}  // namespace mondradiko
