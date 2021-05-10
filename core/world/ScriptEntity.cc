// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/world/ScriptEntity.h"

#include "core/components/scriptable/PointLightComponent.h"
#include "core/components/scriptable/TransformComponent.h"
#include "core/scripting/environment/ComponentScriptEnvironment.h"
#include "core/scripting/instance/ComponentScript.h"
#include "core/world/World.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

static wasm_functype_t* methodType_Entity() {
  return wasm_functype_new_1_1(wasm_valtype_new_i32(), wasm_valtype_new_i32());
}

static wasm_functype_t* methodType_Entity_spawnChildAt() {
  wasm_valtype_t* ps[] = {wasm_valtype_new_i32(), wasm_valtype_new_f64(),
                          wasm_valtype_new_f64(), wasm_valtype_new_f64()};

  wasm_valtype_vec_t p;
  wasm_valtype_vec_new(&p, 4, ps);

  wasm_valtype_t* rs[] = {wasm_valtype_new_i32()};
  wasm_valtype_vec_t r;
  wasm_valtype_vec_new(&r, 1, rs);

  return wasm_functype_new(&p, &r);
}

static wasm_functype_t* methodType_Entity_spawnScriptedChild() {
  wasm_valtype_t* ps[] = {wasm_valtype_new_i32(), wasm_valtype_new_i32()};

  wasm_valtype_vec_t p;
  wasm_valtype_vec_new(&p, 2, ps);

  wasm_valtype_t* rs[] = {wasm_valtype_new_i32()};
  wasm_valtype_vec_t r;
  wasm_valtype_vec_new(&r, 1, rs);

  return wasm_functype_new(&p, &r);
}

static wasm_functype_t* methodType_Entity_spawnScriptedChildAt() {
  wasm_valtype_t* ps[] = {wasm_valtype_new_i32(), wasm_valtype_new_i32(),
                          wasm_valtype_new_f64(), wasm_valtype_new_f64(),
                          wasm_valtype_new_f64()};

  wasm_valtype_vec_t p;
  wasm_valtype_vec_new(&p, 5, ps);

  wasm_valtype_t* rs[] = {wasm_valtype_new_i32()};
  wasm_valtype_vec_t r;
  wasm_valtype_vec_new(&r, 1, rs);

  return wasm_functype_new(&p, &r);
}

// Helper function
static wasm_trap_t* spawnChild(ComponentScript* instance,
                               const wasm_val_t& self_arg,
                               EntityId* new_entity) {
  World* world = instance->world;

  EntityId self_id = self_arg.of.i32;
  if (!world->registry.valid(self_id)) {
    return world->scripts.createTrap("Invalid entity ID");
  }

  *new_entity = world->registry.create();
  world->adopt(self_id, *new_entity);

  return nullptr;
}

static wasm_trap_t* Entity_spawnChild(ComponentScript* instance,
                                      const wasm_val_t args[],
                                      wasm_val_t results[]) {
  EntityId new_entity;
  wasm_trap_t* trap = spawnChild(instance, args[0], &new_entity);
  if (trap != nullptr) return trap;

  results[0].kind = WASM_I32;
  results[0].of.i32 = new_entity;

  return nullptr;
}

static wasm_trap_t* Entity_spawnChildAt(ComponentScript* instance,
                                        const wasm_val_t args[],
                                        wasm_val_t results[]) {
  World* world = instance->world;

  EntityId new_entity;
  wasm_trap_t* trap = spawnChild(instance, args[0], &new_entity);
  if (trap != nullptr) return trap;

  results[0].kind = WASM_I32;
  results[0].of.i32 = new_entity;

  auto pos = glm::vec3(args[1].of.f64, args[2].of.f64, args[3].of.f64);
  auto ori = glm::quat();
  world->registry.emplace<TransformComponent>(new_entity, pos, ori);

  return nullptr;
}

static wasm_trap_t* Entity_spawnScriptedChild(ComponentScript* instance,
                                              const wasm_val_t args[],
                                              wasm_val_t results[]) {
  World* world = instance->world;

  EntityId new_entity;
  wasm_trap_t* trap = spawnChild(instance, args[0], &new_entity);
  if (trap != nullptr) return trap;

  results[0].kind = WASM_I32;
  results[0].of.i32 = new_entity;

  ComponentScriptEnvironment& scripts = world->scripts;

  types::string impl;
  if (!instance->AS_getString(args[1].of.i32, &impl)) {
    return scripts.createTrap("script_impl is not a valid string");
  }

  scripts.instantiateScript(new_entity, instance->getAsset().getId(), impl);

  return nullptr;
}

static wasm_trap_t* Entity_spawnScriptedChildAt(ComponentScript* instance,
                                                const wasm_val_t args[],
                                                wasm_val_t results[]) {
  World* world = instance->world;

  EntityId new_entity;
  wasm_trap_t* trap = spawnChild(instance, args[0], &new_entity);
  if (trap != nullptr) return trap;

  results[0].kind = WASM_I32;
  results[0].of.i32 = new_entity;

  auto pos = glm::vec3(args[2].of.f64, args[3].of.f64, args[4].of.f64);
  auto ori = glm::quat();
  world->registry.emplace<TransformComponent>(new_entity, pos, ori);

  ComponentScriptEnvironment& scripts = world->scripts;

  types::string impl;
  if (!instance->AS_getString(args[1].of.i32, &impl)) {
    return scripts.createTrap("script_impl is not a valid string");
  }

  scripts.instantiateScript(new_entity, instance->getAsset().getId(), impl);

  return nullptr;
}

template <class ComponentType>
static wasm_trap_t* Entity_hasComponent(ComponentScript* instance,
                                        const wasm_val_t args[],
                                        wasm_val_t results[]) {
  World* world = instance->world;

  EntityId self_id = args[0].of.i32;
  if (!world->registry.valid(self_id)) {
    return world->scripts.createTrap("Invalid entity ID");
  }

  results[0].kind = WASM_I32;

  if (world->registry.has<ComponentType>(self_id)) {
    results[0].of.i32 = 1;
  } else {
    results[0].of.i32 = 0;
  }

  return nullptr;
}

template <class ComponentType>
static wasm_trap_t* Entity_addComponent(ComponentScript* instance,
                                        const wasm_val_t args[],
                                        wasm_val_t results[]) {
  World* world = instance->world;

  EntityId self_id = args[0].of.i32;
  if (!world->registry.valid(self_id)) {
    return world->scripts.createTrap("Invalid entity ID");
  }

  results[0].kind = WASM_I32;
  results[0].of.i32 = self_id;

  if (!world->registry.has<ComponentType>(self_id)) {
    world->registry.emplace<ComponentType>(self_id);
  }

  return nullptr;
}

template <class ComponentType>
static wasm_trap_t* Entity_getComponent(ComponentScript* instance,
                                        const wasm_val_t args[],
                                        wasm_val_t results[]) {
  World* world = instance->world;

  EntityId self_id = args[0].of.i32;
  if (!world->registry.valid(self_id)) {
    return world->scripts.createTrap("Invalid entity ID");
  }

  if (!world->registry.has<ComponentType>(self_id)) {
    world->registry.emplace<ComponentType>(self_id);
  }

  results[0].kind = WASM_I32;
  results[0].of.i32 = self_id;
  return nullptr;
}

using BoundEntityMethod = wasm_trap_t* (*)(ComponentScript*, const wasm_val_t[],
                                           wasm_val_t[]);

using EntityMethodTypeCallback = wasm_functype_t* (*)();

template <BoundEntityMethod method>
wasm_trap_t* entityMethodWrapper(const wasmtime_caller_t* caller, void* env,
                                 const wasm_val_t args[],
                                 wasm_val_t results[]) {
  ComponentScript* instance = reinterpret_cast<ComponentScript*>(env);

  World* world = instance->world;

  if (!world->registry.valid(args[0].of.i32)) {
    return world->scripts.createTrap("Invalid entity ID");
  }

  return (*method)(instance, args, results);
}

static void finalizer(void*) {}

template <BoundEntityMethod method, EntityMethodTypeCallback type_callback>
wasm_func_t* createEntityMethod(ScriptInstance* instance) {
  ScriptEnvironment* scripts = instance->scripts;

  wasm_store_t* store = scripts->getStore();

  wasm_functype_t* func_type = (*type_callback)();

  wasmtime_func_callback_with_env_t callback = entityMethodWrapper<method>;

  void* env = static_cast<void*>(instance);

  wasm_func_t* func =
      wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  wasm_functype_delete(func_type);

  scripts->collectFunc(func);

  return func;
}

template <BoundEntityMethod method, EntityMethodTypeCallback type_callback>
void linkEntityMethod(ComponentScriptEnvironment* scripts, World* world,
                      const types::string& symbol) {
  ScriptBindingFactory factory = createEntityMethod<method, type_callback>;
  scripts->addBindingFactory(symbol, factory);
}

// Helper function to link a component API
template <class ComponentType>
void linkComponentApi(World* world, const char* symbol) {
  ComponentScriptEnvironment* scripts = &world->scripts;
  linkEntityMethod<Entity_getComponent<ComponentType>, methodType_Entity>(
      scripts, world, types::string("Entity_get") + symbol);
  linkEntityMethod<Entity_hasComponent<ComponentType>, methodType_Entity>(
      scripts, world, types::string("Entity_has") + symbol);
  linkEntityMethod<Entity_addComponent<ComponentType>, methodType_Entity>(
      scripts, world, types::string("Entity_add") + symbol);
}

void ScriptEntity::linkScriptApi(World* world) {
  ComponentScriptEnvironment* scripts = &world->scripts;

  linkEntityMethod<Entity_spawnChild, methodType_Entity>(scripts, world,
                                                         "Entity_spawnChild");
  linkEntityMethod<Entity_spawnChildAt, methodType_Entity_spawnChildAt>(
      scripts, world, "Entity_spawnChildAt");
  linkEntityMethod<Entity_spawnScriptedChild,
                   methodType_Entity_spawnScriptedChild>(
      scripts, world, "Entity_spawnScriptedChild");
  linkEntityMethod<Entity_spawnScriptedChildAt,
                   methodType_Entity_spawnScriptedChildAt>(
      scripts, world, "Entity_spawnScriptedChildAt");

  linkComponentApi<PointLightComponent>(world, "PointLight");
  linkComponentApi<TransformComponent>(world, "Transform");
}

}  // namespace core
}  // namespace mondradiko
