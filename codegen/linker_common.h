// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <sstream>
#include <typeinfo>

#include "core/scripting/ScriptEnvironment.h"
#include "core/world/World.h"
#include "lib/include/wasm_headers.h"
#include "log/log.h"

namespace mondradiko {
namespace codegen {

using namespace core;

template <class ClassdefType>
using BoundClassdefMethod = wasm_trap_t* (ClassdefType::*)(ScriptEnvironment*,
                                                           const wasm_val_t[],
                                                           wasm_val_t[]);

using ClassdefMethodCallback = const wasm_functype_t* (*)();

// Forward definition for functions to create Wasm method types
template <class ClassdefType, BoundClassdefMethod<ClassdefType> method>
static const wasm_functype_t* createClassdefMethodType();

static void finalizer(void*) {}

template <class ComponentType, BoundClassdefMethod<ComponentType> method>
static wasm_trap_t* componentMethodWrapper(const wasmtime_caller_t* caller,
                                           void* env, const wasm_val_t args[],
                                           wasm_val_t results[]) {
  World* world = reinterpret_cast<World*>(env);
  EntityId self_id = static_cast<EntityId>(args[0].of.i32);
  ComponentType* self = world->registry.try_get<ComponentType>(self_id);

  if (self == nullptr) {
    std::ostringstream error_format;
    error_format << "Type assertion failed: ";
    error_format << "Entity " << self_id << " does not have a ";
    error_format << typeid(ComponentType).name();

    wasm_name_t error;
    wasm_name_new_from_string(&error, error_format.str().c_str());

    return wasm_trap_new(world->scripts->getStore(), &error);
  }

  return ((*self).*method)(world->scripts, args, results);
}

template <class ComponentType, BoundClassdefMethod<ComponentType> method>
void linkComponentMethod(ScriptEnvironment* scripts, World* world,
                         const char* symbol,
                         ClassdefMethodCallback type_callback) {
  wasm_store_t* store = scripts->getStore();

  const wasm_functype_t* func_type = (*type_callback)();

  wasmtime_func_callback_with_env_t callback =
      componentMethodWrapper<ComponentType, method>;

  void* env = static_cast<void*>(world);

  wasm_func_t* func =
      wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  scripts->addBinding(symbol, func);
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method>
static wasm_trap_t* dynamicObjectMethodWrapper(const wasmtime_caller_t* caller,
                                               void* env,
                                               const wasm_val_t args[],
                                               wasm_val_t results[]) {
  ScriptEnvironment* scripts = reinterpret_cast<ScriptEnvironment*>(env);

  uint32_t self_id = args[0].of.i32;
  void* self_raw = scripts->getFromRegistry(self_id);

  if (self_raw == nullptr) {
    std::ostringstream error_format;
    error_format << "Registry lookup failed: ";
    error_format << self_id << " is an invalid ID";

    wasm_name_t error;
    wasm_name_new_from_string(&error, error_format.str().c_str());

    return wasm_trap_new(scripts->getStore(), &error);
  }

  ObjectType* self = reinterpret_cast<ObjectType*>(self_raw);

  // TODO(marceline-cramer) Type assertion with ScriptObject base class
  /*if (self == nullptr) {
    std::ostringstream error_format;
    error_format << "Type assertion failed: ";
    error_format << self_id << " is not a ";
    error_format << typeid(ObjectType).name();

    wasm_name_t error;
    wasm_name_new_from_string(&error, error_format.str().c_str());

    return wasm_trap_new(scripts->getStore(), &error);
  }*/

  return ((*self).*method)(scripts, args, results);
}

template <class ObjectType, BoundClassdefMethod<ObjectType> method>
void linkDynamicObjectMethod(ScriptEnvironment* scripts, const char* symbol,
                             ClassdefMethodCallback type_callback) {
  wasm_store_t* store = scripts->getStore();

  const wasm_functype_t* func_type = (*type_callback)();

  wasmtime_func_callback_with_env_t callback =
      dynamicObjectMethodWrapper<ObjectType, method>;

  void* env = static_cast<void*>(scripts);

  wasm_func_t* func =
      wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  scripts->addBinding(symbol, func);
}

}  // namespace codegen
}  // namespace mondradiko
