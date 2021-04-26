// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/PrefabAsset.h"

#include <memory>

#include "core/components/scriptable/PointLightComponent.h"
#include "core/components/scriptable/TransformComponent.h"
#include "core/components/synchronized/MeshRendererComponent.h"
#include "core/components/synchronized/RigidBodyComponent.h"
#include "core/components/synchronized/ShapeComponent.h"
#include "core/scripting/environment/ComponentScriptEnvironment.h"
#include "core/world/World.h"

namespace mondradiko {
namespace core {

// Helper template function to instantiate components
template <class ComponentType, class PrefabType>
void initComponent(AssetPool* asset_pool, EntityRegistry* registry,
                   EntityId instance_id,
                   const std::unique_ptr<PrefabType>& prefab) {
  if (prefab) {
    auto& c = registry->emplace<ComponentType>(
        instance_id, static_cast<const PrefabType*>(prefab.get()));
    c.refresh(asset_pool);
  }
}

PrefabAsset::~PrefabAsset() {
  if (prefab != nullptr) delete prefab;
}

EntityId PrefabAsset::instantiate(World* world) const {
  EntityRegistry* registry = &world->registry;
  EntityId self_id = registry->create();

  initComponent<MeshRendererComponent>(asset_pool, registry, self_id,
                                       prefab->mesh_renderer);
  initComponent<PointLightComponent>(asset_pool, registry, self_id,
                                     prefab->point_light);
  initComponent<RigidBodyComponent>(asset_pool, registry, self_id,
                                    prefab->rigid_body);
  initComponent<ShapeComponent>(asset_pool, registry, self_id, prefab->shape);
  initComponent<TransformComponent>(asset_pool, registry, self_id,
                                    prefab->transform);

  if (children.size() > 0) {
    for (auto& child : children) {
      if (!child) continue;

      EntityId child_id = child->instantiate(world);
      world->adopt(self_id, child_id);
    }
  }

  if (prefab->script) {
    // Update an entity's script to initialize the ScriptComponent
    world->scripts.updateScript(self_id, prefab->script->script, nullptr,
                                static_cast<size_t>(0));
  }

  return self_id;
}

bool PrefabAsset::_load(const assets::SerializedAsset* asset) {
  const assets::PrefabAsset* prefab_asset = asset->prefab();

  prefab = new assets::PrefabAssetT;
  prefab_asset->UnPackTo(prefab);

  children.resize(0);

  for (auto& child : prefab->children) {
    children.push_back(asset_pool->load<PrefabAsset>(child));
  }

  return true;
}

}  // namespace core
}  // namespace mondradiko
