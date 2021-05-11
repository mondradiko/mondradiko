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

ComponentScriptEnvironment::ComponentScriptEnvironment(World* world)
    : asset_pool(world->asset_pool), world(world) {
  log_zone;

  asset_pool->initializeAssetType<ScriptAsset>(this);

  world->registry.on_destroy<ScriptComponent>()
      .connect<&onScriptComponentDestroy>();

  linkEnvironment(this, world);
}

ComponentScriptEnvironment::~ComponentScriptEnvironment() { log_zone; }

void ComponentScriptEnvironment::linkEnvironment(ScriptEnvironment* scripts,
                                                 World* world) {
  PointLightComponent::linkScriptApi(scripts, world);
  TransformComponent::linkScriptApi(scripts, world);
  ScriptEntity::linkScriptApi(scripts, world);
}

void ComponentScriptEnvironment::update(double dt) {
  log_zone;

  auto script_view = world->registry.view<ScriptComponent>();

  for (auto& e : script_view) {
    auto& script = script_view.get(e);

    if (script._script_instance == nullptr) continue;
    if (!script._script_instance->getAsset()) continue;

    script._script_instance->update(dt);
  }
}

void ComponentScriptEnvironment::instantiateScript(EntityId entity,
                                                   AssetId script_id,
                                                   const types::string& impl) {
  log_zone;

  EntityRegistry* registry = &world->registry;

  if (!registry->valid(entity)) {
    log_err_fmt("Cannot instantiate script on nonexistent entity %d", entity);
    return;
  }

  if (registry->has<ScriptComponent>(entity)) {
    log_err_fmt("Entity %d already has a ScriptComponent", entity);
    return;
  }

  log_msg_fmt("%lu", sizeof(ComponentScript));

  auto asset = asset_pool->load<ScriptAsset>(script_id);
  auto instance = new ComponentScript(this, world, asset, entity, impl);

  auto& component = registry->emplace<ScriptComponent>(entity);
  component._script_impl = impl;
  component._script_instance = instance;
}

void ComponentScriptEnvironment::onScriptComponentDestroy(
    EntityRegistry& registry, EntityId id) {
  auto& script = registry.get<ScriptComponent>(id);

  if (script._script_instance != nullptr) {
    delete script._script_instance;
    script._script_instance = nullptr;
  }
}

}  // namespace core
}  // namespace mondradiko
