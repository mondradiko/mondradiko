/**
 * @file World.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains world configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <map>

#include "core/assets/AssetPool.h"

namespace mondradiko {

// Forward declarations
class Filesystem;
class GpuInstance;

class World {
 public:
  World(Filesystem*, GpuInstance*);
  ~World();

  void testInitialize();
  bool update();

  Filesystem* fs;
  GpuInstance* gpu;

  // private:
  entt::registry registry;
  AssetPool asset_pool;
};

}  // namespace mondradiko
