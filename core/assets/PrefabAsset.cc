/**
 * @file PrefabAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates new entities with components as saved in an asset.
 * @date 2021-14-01
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/PrefabAsset.h"

#include <memory>

#include "core/components/MeshRendererComponent.h"
#include "core/components/TransformComponent.h"

namespace mondradiko {

// Helper template function to instantiate components
template <class ComponentType, class PrefabType>
void initComponent(EntityRegistry* registry, EntityId instance_id,
                   const std::unique_ptr<PrefabType>& prefab) {
  if (prefab) {
    registry->emplace<ComponentType>(
        instance_id, static_cast<const PrefabType*>(prefab.get()));
  }
}

void PrefabAsset::load(const assets::SerializedAsset* asset) {
  const assets::PrefabAsset* prefab_asset = asset->prefab();

  prefab = new assets::PrefabAssetT;
  prefab_asset->UnPackTo(prefab);
}

PrefabAsset::~PrefabAsset() { delete prefab; }

EntityId PrefabAsset::instantiate(EntityRegistry* registry,
                                  const TransformComponent& transform) const {
  EntityId instance_id = registry->create();

  initComponent<MeshRendererComponent>(registry, instance_id,
                                       prefab->mesh_renderer);

  registry->emplace<TransformComponent>(instance_id, transform);

  // TODO(marceline-cramer) ScriptPrefab
  // Update an entity's script to initialize the ScriptComponent
  /*scripts.updateScript(registry, &asset_pool, test_entity, 0x55715294,
     nullptr, static_cast<size_t>(0));*/

  return instance_id;
}

}  // namespace mondradiko
