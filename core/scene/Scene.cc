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

#include "core/assets/TextureAsset.h"
#include "core/components/MeshRendererComponent.h"
#include "core/components/TransformComponent.h"
#include "core/displays/DisplayInterface.h"
#include "core/filesystem/Filesystem.h"
#include "core/gpu/GpuInstance.h"
#include "core/renderer/Renderer.h"
#include "log/log.h"

namespace mondradiko {

Scene::Scene(DisplayInterface* display, Filesystem* fs, GpuInstance* gpu,
             Renderer* renderer)
    : display(display), fs(fs), gpu(gpu), renderer(renderer), asset_pool(fs) {
  log_zone;
}

Scene::~Scene() {
  log_zone;

  asset_pool.unloadAll<MeshAsset>();
  asset_pool.unloadAll<MaterialAsset>();
  asset_pool.unloadAll<TextureAsset>();
}

void Scene::testInitialize() {
  EntityId parent_entity;

  {
    MeshRendererComponent mesh_renderer{
        .mesh_asset = asset_pool.loadAsset<MeshAsset>(0xdeadbeef, gpu),
        .material_asset =
            asset_pool.loadAsset<MaterialAsset>(0xAAAAAAAA, &asset_pool, gpu)};

    TransformComponent transform;
    transform.parent = NullEntity;
    transform.local_transform = glm::mat4(1.0);

    parent_entity = registry.create();
    registry.emplace<MeshRendererComponent>(parent_entity, mesh_renderer);
    registry.emplace<TransformComponent>(parent_entity, transform);
  }

  for (uint32_t i = 0; i < 10; i++) {
    MeshRendererComponent mesh_renderer{
        .mesh_asset = asset_pool.loadAsset<MeshAsset>(0xdeadbeef, gpu),
        .material_asset =
            asset_pool.loadAsset<MaterialAsset>(0xAAAAAAAB, &asset_pool, gpu)};

    TransformComponent transform;
    transform.parent = parent_entity;
    transform.local_transform = glm::mat4(1.0);
    transform.local_transform =
        glm::translate(transform.local_transform, glm::vec3(0.0, -2.0, 0.0));

    auto test_entity = registry.create();
    registry.emplace<MeshRendererComponent>(test_entity, mesh_renderer);
    registry.emplace<TransformComponent>(test_entity, transform);

    parent_entity = test_entity;
  }
}

bool Scene::update() {
  DisplayPollEventsInfo poll_info;
  poll_info.renderer = renderer;

  display->pollEvents(&poll_info);
  if (poll_info.should_quit) return false;

  if (poll_info.should_run) {
    DisplayBeginFrameInfo frame_info;
    display->beginFrame(&frame_info);

    {
      log_zone_named("Update TransformComponent self-reference");

      auto transform_view = registry.view<TransformComponent>();

      for (EntityId e : transform_view) {
        transform_view.get(e).this_entity = e;
      }
    }

    {
      log_zone_named("Sort transform hierarchy");

      registry.sort<TransformComponent>(
          [](const auto& parent, const auto& child) {
            // Sort children after parents
            return parent.this_entity >= child.parent;
          });
    }

    {
      log_zone_named("Transform children under parents");

      auto transform_view = registry.view<TransformComponent>();

      for (EntityId e : transform_view) {
        TransformComponent& transform = transform_view.get(e);

        glm::mat4 parent_transform;
        if (transform.parent == NullEntity) {
          parent_transform = glm::mat4(1.0);
        } else {
          parent_transform = registry.get<TransformComponent>(transform.parent)
                                 .world_transform;
        }

        transform.world_transform =
            parent_transform * transform.local_transform;
      }
    }

    if (frame_info.should_render) {
      renderer->renderFrame(registry, &asset_pool);
    }

    display->endFrame(&frame_info);
  }

  FrameMark;
  return true;
}

}  // namespace mondradiko
