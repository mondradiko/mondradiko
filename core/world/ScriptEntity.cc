// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/world/ScriptEntity.h"

#include "core/components/scriptable/PointLightComponent.h"
#include "core/components/scriptable/TransformComponent.h"
#include "core/scripting/environment/ComponentScriptEnvironment.h"
#include "core/world/World.h"

namespace mondradiko {
namespace core {

wasm_functype_t* methodType_Entity() {
  return wasm_functype_new_1_1(wasm_valtype_new_i32(), wasm_valtype_new_i32());
}

template <class ComponentType>
static wasm_trap_t* Entity_hasComponent(World* world, const wasm_val_t args[],
                                        wasm_val_t results[]) {
  results[0].kind = WASM_I32;

  if (world->registry.has<ComponentType>(args[0].of.i32)) {
    results[0].of.i32 = 1;
  } else {
    results[0].of.i32 = 0;
  }

  return nullptr;
}

template <class ComponentType>
static wasm_trap_t* Entity_addComponent(World* world, const wasm_val_t args[],
                                        wasm_val_t results[]) {
  results[0].kind = WASM_I32;
  results[0].of.i32 = args[0].of.i32;

  if (!world->registry.has<ComponentType>(args[0].of.i32)) {
    world->registry.emplace<ComponentType>(args[0].of.i32);
  }

  return nullptr;
}

static wasm_trap_t* Entity_getComponent(World*, const wasm_val_t args[],
                                        wasm_val_t results[]) {
  results[0].kind = WASM_I32;
  results[0].of.i32 = args[0].of.i32;
  return nullptr;
}

using BoundEntityMethod = wasm_trap_t* (*)(World*, const wasm_val_t[],
                                           wasm_val_t[]);

using EntityMethodTypeCallback = wasm_functype_t* (*)();

static void finalizer(void*) {}

template <BoundEntityMethod method>
wasm_trap_t* entityMethodWrapper(const wasmtime_caller_t* caller, void* env,
                                 const wasm_val_t args[],
                                 wasm_val_t results[]) {
  World* world = reinterpret_cast<World*>(env);

  if (!world->registry.valid(args[0].of.i32)) {
    return world->scripts.createTrap("Invalid entity ID");
  }

  return (*method)(world, args, results);
}

template <BoundEntityMethod method>
void linkEntityMethod(ComponentScriptEnvironment* scripts, World* world,
                      const char* symbol,
                      EntityMethodTypeCallback type_callback) {
  wasm_store_t* store = scripts->getStore();

  wasm_functype_t* func_type = (*type_callback)();

  wasmtime_func_callback_with_env_t callback = entityMethodWrapper<method>;

  void* env = static_cast<void*>(world);

  wasm_func_t* func =
      wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  scripts->addBinding(symbol, func);
  wasm_functype_delete(func_type);
}

void ScriptEntity::linkScriptApi(World* world) {
  ComponentScriptEnvironment* scripts = &world->scripts;

  linkEntityMethod<Entity_getComponent>(scripts, world, "Entity_getComponent",
                                        methodType_Entity);
  linkEntityMethod<Entity_hasComponent<PointLightComponent>>(
      scripts, world, "Entity_hasPointLight", methodType_Entity);
  linkEntityMethod<Entity_addComponent<PointLightComponent>>(
      scripts, world, "Entity_addPointLight", methodType_Entity);
  linkEntityMethod<Entity_hasComponent<TransformComponent>>(
      scripts, world, "Entity_hasTransform", methodType_Entity);
  linkEntityMethod<Entity_addComponent<TransformComponent>>(
      scripts, world, "Entity_addTransform", methodType_Entity);
}

}  // namespace core
}  // namespace mondradiko
