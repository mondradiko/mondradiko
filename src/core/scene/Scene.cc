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

#include "core/components/MeshRendererComponent.h"
#include "core/filesystem/Filesystem.h"
#include "log/log.h"

namespace mondradiko {

Scene::Scene(DisplayInterface* display, Filesystem* fs, GpuInstance* gpu,
             Renderer* renderer)
    : display(display), fs(fs), gpu(gpu), renderer(renderer), asset_pool(fs) {
  log_zone;

  MeshRendererComponent mesh_renderer{
      .mesh_asset = asset_pool.loadAsset<MeshAsset>(0xdeadbeef, gpu),
      .material_asset = asset_pool.loadAsset<MaterialAsset>(0xAAAAAAAA, gpu)};

  auto test_entity = registry.create();
  registry.emplace<MeshRendererComponent>(test_entity, mesh_renderer);
}

Scene::~Scene() {
  log_zone;

  asset_pool.unloadAll<MeshAsset>();
  asset_pool.unloadAll<MaterialAsset>();
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
      renderer->renderFrame(registry, &asset_pool);
    }

    display->endFrame(&frame_info);
  }

  FrameMark;
  return true;
}

}  // namespace mondradiko
