/**
 * @file Scene.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Scene configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/scene/Scene.h"

#include <iostream>

#include "core/filesystem/Filesystem.h"
#include "log/log.h"

namespace mondradiko {

Scene::Scene(AssetPool* asset_pool, DisplayInterface* display, Filesystem* fs,
             GpuInstance* gpu, Renderer* renderer)
    : asset_pool(asset_pool),
      display(display),
      fs(fs),
      gpu(gpu),
      renderer(renderer) {
  log_zone;
}

Scene::~Scene() {
  log_zone;

  Assimp::DefaultLogger::kill();
}

bool Scene::update() {
  DisplayPollEventsInfo poll_info;
  poll_info.renderer = renderer;

  display->pollEvents(&poll_info);
  if (poll_info.should_quit) return false;

  if (poll_info.should_run) {
    DisplayBeginFrameInfo frame_info;
    display->beginFrame(&frame_info);

    if (frame_info.should_render) {
      renderer->renderFrame(registry, asset_pool);
    }

    display->endFrame(&frame_info);
  }

  FrameMark;
  return true;
}

}  // namespace mondradiko
