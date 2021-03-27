// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/PrefabAsset.h"

#include <memory>

#include "core/components/MeshRendererComponent.h"
#include "core/components/PointLightComponent.h"
#include "core/components/TransformComponent.h"
#include "core/scripting/ScriptEnvironment.h"
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

void PrefabAsset::load(const assets::SerializedAsset* asset) {
  const assets::PrefabAsset* prefab_asset = asset->prefab();

  prefab = new assets::PrefabAssetT;
  prefab_asset->UnPackTo(prefab);

  children.resize(0);

  for (auto& child : prefab->children) {
    children.push_back(asset_pool->load<PrefabAsset>(child));
  }
}

PrefabAsset::~PrefabAsset() {
  if (prefab != nullptr) delete prefab;
}

EntityId PrefabAsset::instantiate(ScriptEnvironment* scripts,
                                  World* world) const {
  EntityRegistry* registry = &world->registry;
  EntityId self_id = registry->create();

  initComponent<MeshRendererComponent>(asset_pool, registry, self_id,
                                       prefab->mesh_renderer);
  initComponent<PointLightComponent>(asset_pool, registry, self_id,
                                     prefab->point_light);
  initComponent<TransformComponent>(asset_pool, registry, self_id,
                                    prefab->transform);

  if (children.size() > 0) {
    for (auto& child : children) {
      if (!child) continue;

      EntityId child_id = child->instantiate(scripts, world);
      world->adopt(self_id, child_id);
    }
  }

  if (prefab->script) {
    // Update an entity's script to initialize the ScriptComponent
    scripts->updateScript(registry, asset_pool, self_id, prefab->script->script,
                          nullptr, static_cast<size_t>(0));
  }

  return self_id;
}

}  // namespace core
}  // namespace mondradiko
