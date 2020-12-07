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

#include "filesystem/Filesystem.h"
#include "renderer/Renderer.h"
#include "network/NetworkClient.h"

namespace mondradiko {

class Scene {
 public:
  Scene(DisplayInterface*, Filesystem*, GpuInstance*, Renderer*);
  ~Scene();

  void run(NetworkClient*);

  void update(double);
  void signalExit() { g_should_quit = true; }

  bool loadModel(const char*);

  DisplayInterface* display;
  Filesystem* fs;
  GpuInstance* gpu;
  Renderer* renderer;

  // TODO(marceline-cramer) Move session lifetime to Scene
  entt::registry registry;

 private:
  Assimp::Importer importer;

  bool g_should_quit;
};

}  // namespace mondradiko
