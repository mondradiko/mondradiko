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
#include "core/filesystem/Filesystem.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

Scene::Scene(Filesystem* fs, GpuInstance* gpu)
    : fs(fs), gpu(gpu), asset_pool(fs) {
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
        .mesh_asset = asset_pool.loadAsset<MeshAsset>(0x84b42359, gpu),
        .material_asset =
            asset_pool.loadAsset<MaterialAsset>(0xd316e038, gpu)};

    TransformComponent transform;
    transform.parent = NullEntity;
    transform.local_transform = glm::mat4(1.0);

    parent_entity = registry.create();
    registry.emplace<MeshRendererComponent>(parent_entity, mesh_renderer);
    registry.emplace<TransformComponent>(parent_entity, transform);
  }

  for (uint32_t i = 0; i < 10; i++) {
    MeshRendererComponent mesh_renderer{
        .mesh_asset = asset_pool.loadAsset<MeshAsset>(0x84b42359, gpu),
        .material_asset =
            asset_pool.loadAsset<MaterialAsset>(0xd316e038, gpu)};

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
  log_zone;

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
        parent_transform =
            registry.get<TransformComponent>(transform.parent).world_transform;
      }

      transform.world_transform = parent_transform * transform.local_transform;
    }
  }

  FrameMark;
  return true;
}

}  // namespace mondradiko
