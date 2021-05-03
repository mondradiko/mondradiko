// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <sstream>
#include <typeinfo>

#include "core/scripting/environment/ScriptEnvironment.h"
#include "core/scripting/instance/ComponentScript.h"
#include "core/scripting/instance/ScriptInstance.h"
#include "core/scripting/object/DynamicScriptObject.h"
#include "core/scripting/object/StaticScriptObject.h"
#include "core/world/World.h"
#include "lib/include/wasm_headers.h"
#include "log/log.h"

namespace mondradiko {
namespace codegen {

using namespace core;

template <class ClassdefType>
using BoundClassdefMethod = wasm_trap_t* (ClassdefType::*)(ScriptInstance*,
                                                           const wasm_val_t[],
                                                           wasm_val_t[]);

template <class ClassdefType>
using BoundComponentMethod = wasm_trap_t* (ClassdefType::*)(ComponentScript*,
                                                            const wasm_val_t[],
                                                            wasm_val_t[]);

using ClassdefMethodCallback = wasm_functype_t* (*)();

// Forward definition for functions to create Wasm method types
template <class ClassdefType, BoundClassdefMethod<ClassdefType> method>
static const wasm_functype_t* createClassdefMethodType();

static void finalizer(void*) {}

template <class ComponentType, BoundComponentMethod<ComponentType> method>
static wasm_trap_t* componentMethodWrapper(const wasmtime_caller_t* caller,
                                           void* env, const wasm_val_t args[],
                                           wasm_val_t results[]) {
  ComponentScript* instance = reinterpret_cast<ComponentScript*>(env);
  World* world = instance->world;
  ComponentScriptEnvironment* scripts = &world->scripts;
  EntityId self_id = static_cast<EntityId>(args[0].of.i32);
  ComponentType* self = world->registry.try_get<ComponentType>(self_id);

  if (self == nullptr) {
    std::ostringstream error_format;
    error_format << "Type assertion failed: ";
    error_format << "Entity " << self_id << " does not have a ";
    error_format << typeid(ComponentType).name();
    return scripts->createTrap(error_format.str());
  }

  return ((*self).*method)(instance, args, results);
}

template <class ComponentType, BoundComponentMethod<ComponentType> method,
          ClassdefMethodCallback type_callback>
wasm_func_t* createComponentMethod(ScriptInstance* instance) {
  ScriptEnvironment* scripts = instance->scripts;
  wasm_store_t* store = scripts->getStore();

  wasm_functype_t* func_type = (*type_callback)();

  wasmtime_func_callback_with_env_t callback =
      componentMethodWrapper<ComponentType, method>;

  void* env = static_cast<void*>(instance);

  wasm_func_t* func =
      wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  wasm_functype_delete(func_type);

  scripts->collectFunc(func);

  return func;
}

template <class ComponentType, BoundComponentMethod<ComponentType> method,
          ClassdefMethodCallback type_callback>
void linkComponentMethod(ScriptEnvironment* scripts, World* world,
                         const char* symbol) {
  ScriptBindingFactory factory =
      createComponentMethod<ComponentType, method, type_callback>;
  scripts->addBindingFactory(symbol, factory);
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method>
static wasm_trap_t* dynamicObjectMethodWrapper(const wasmtime_caller_t* caller,
                                               void* env,
                                               const wasm_val_t args[],
                                               wasm_val_t results[]) {
  ScriptInstance* instance = reinterpret_cast<ScriptInstance*>(env);
  ScriptEnvironment* scripts = instance->scripts;

  uint32_t self_id = args[0].of.i32;
  void* self_raw = scripts->getFromRegistry(self_id);

  if (self_raw == nullptr) {
    std::ostringstream error_format;
    error_format << "Registry lookup failed: ";
    error_format << self_id << " is an invalid ID";
    return scripts->createTrap(error_format.str());
  }

  ObjectType* self = reinterpret_cast<ObjectType*>(self_raw);

  return ((*self).*method)(instance, args, results);
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method,
          ClassdefMethodCallback type_callback>
wasm_func_t* createDynamicObjectMethod(ScriptInstance* instance) {
  ScriptEnvironment* scripts = instance->scripts;

  wasm_store_t* store = scripts->getStore();

  wasm_functype_t* func_type = (*type_callback)();

  wasmtime_func_callback_with_env_t callback =
      dynamicObjectMethodWrapper<ObjectType, method>;

  void* env = static_cast<void*>(instance);

  wasm_func_t* func =
      wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  wasm_functype_delete(func_type);

  scripts->collectFunc(func);

  return func;
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method,
          ClassdefMethodCallback type_callback>
void linkDynamicObjectMethod(ScriptEnvironment* scripts, const char* symbol) {
  ScriptBindingFactory factory =
      createDynamicObjectMethod<ObjectType, method, type_callback>;
  scripts->addBindingFactory(symbol, factory);
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method>
static wasm_trap_t* staticObjectMethodWrapper(const wasmtime_caller_t* caller,
                                              void* env,
                                              const wasm_val_t args[],
                                              wasm_val_t results[]) {
  ScriptInstance* instance = reinterpret_cast<ScriptInstance*>(env);
  const char* symbol = typeid(ObjectType).name();
  ObjectType* self =
      reinterpret_cast<ObjectType*>(instance->scripts->getStaticObject(symbol));
  return ((*self).*method)(instance, args, results);
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method,
          ClassdefMethodCallback type_callback>
wasm_func_t* createStaticObjectMethod(ScriptInstance* instance) {
  ScriptEnvironment* scripts = instance->scripts;

  wasm_store_t* store = scripts->getStore();

  wasm_functype_t* func_type = (*type_callback)();

  wasmtime_func_callback_with_env_t callback =
      staticObjectMethodWrapper<ObjectType, method>;

  void* env = static_cast<void*>(instance);

  wasm_func_t* func =
      wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  wasm_functype_delete(func_type);

  scripts->collectFunc(func);

  return func;
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method,
          ClassdefMethodCallback type_callback>
void linkStaticObjectMethod(ScriptEnvironment* scripts, ObjectType* self,
                            const char* symbol) {
  ScriptBindingFactory factory =
      createStaticObjectMethod<ObjectType, method, type_callback>;
  scripts->addBindingFactory(symbol, factory);
}

}  // namespace codegen
}  // namespace mondradiko
