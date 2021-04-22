// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/environment/ComponentScriptEnvironment.h"

#include "core/assets/ScriptAsset.h"
#include "core/components/internal/ScriptComponent.h"
#include "core/components/scriptable/PointLightComponent.h"
#include "core/components/scriptable/TransformComponent.h"
#include "core/scripting/instance/ComponentScript.h"
#include "core/scripting/instance/ScriptInstance.h"
#include "core/world/ScriptEntity.h"
#include "core/world/World.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

ComponentScriptEnvironment::ComponentScriptEnvironment(AssetPool* asset_pool,
                                                       World* world)
    : asset_pool(asset_pool), world(world) {
  log_zone;

  asset_pool->initializeAssetType<ScriptAsset>(this);

  PointLightComponent::linkScriptApi(this, world);
  TransformComponent::linkScriptApi(this, world);
  ScriptEntity::linkScriptApi(world);
}

ComponentScriptEnvironment::~ComponentScriptEnvironment() {
  log_zone;

  auto script_view = world->registry.view<ScriptComponent>();

  for (auto& e : script_view) {
    auto& script = script_view.get(e);

    if (script.script_instance != nullptr) {
      delete script.script_instance;
      script.script_instance = nullptr;
    }
  }
}

void ComponentScriptEnvironment::update(double dt) {
  auto script_view = world->registry.view<ScriptComponent>();

  for (auto& e : script_view) {
    auto& script = script_view.get(e);

    if (!script.getScriptAsset()) continue;

    script.script_instance->update(e, dt);
  }
}

void ComponentScriptEnvironment::updateScript(EntityId entity,
                                              AssetId script_id,
                                              const uint8_t* data,
                                              size_t data_size) {
  log_zone;

  EntityRegistry* registry = &world->registry;

  // Ensure the entity exists
  if (!registry->valid(entity)) {
    entity = registry->create(entity);
  }

  bool needs_initialization = false;

  const AssetHandle<ScriptAsset>& script_asset =
      asset_pool->load<ScriptAsset>(script_id);

  // Destroy the old ScriptInstance if necessary
  if (registry->has<ScriptComponent>(entity)) {
    ScriptComponent& old_script = registry->get<ScriptComponent>(entity);
    if (old_script.script_asset != script_asset) {
      delete old_script.script_instance;
      needs_initialization = true;
    }
  } else {
    needs_initialization = true;
  }

  ScriptComponent& script_component =
      registry->get_or_emplace<ScriptComponent>(entity);

  if (needs_initialization) {
    script_component.script_instance = script_asset->createInstance(world);
  }

  script_component.script_asset = script_asset;
  script_component.script_instance->updateData(data, data_size);
}

}  // namespace core
}  // namespace mondradiko
