/**
 * @file Scene.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Scene configuration, updates and stores Entities, loads
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
class DisplayInterface;
class Filesystem;
class GpuInstance;
class Renderer;

class Scene {
 public:
  Scene(DisplayInterface*, Filesystem*, GpuInstance*, Renderer*);
  ~Scene();

  void testInitialize();
  bool update();

  DisplayInterface* display;
  Filesystem* fs;
  GpuInstance* gpu;
  Renderer* renderer;

  // private:
  entt::registry registry;
  AssetPool asset_pool;
};

}  // namespace mondradiko
