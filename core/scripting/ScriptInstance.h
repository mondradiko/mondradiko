// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later
//
// After a Wasm module is loaded and created by ScriptAsset, it can create
// ScriptInstances, which represent individual WebAssembly processes, each with
// their own execution environment.
//
// When a ScriptInstance is created, it calls the instantiate() function of the
// instance, if it exists. If it does, then instantiate() returns a pointer to a
// structure inside of Wasm memory, that needs to be synchronized over the
// network. This is represented by a byte array, copied directly from the Wasm
// store, and packaged in the ScriptComponent flatbuffer.
//
// Then, the callbacks exported from from the module are registered, and the
// ScriptEnvironment is ready to start calling them on world events.
//
///////////////////////////////////////////////////////////////////////////////
// To-do list:
//
// TODO(marceline-cramer) Actually implement dynamic allocation
// TODO(marceline-cramer) Find way to refcount ScriptInstances from ScriptAsset
// so that ScriptAssets aren't unloaded before ScriptInstances are
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <unordered_map>

#include "core/world/Entity.h"
#include "lib/include/wasm_headers.h"

namespace mondradiko {

// Forward declarations
class ScriptEnvironment;

class ScriptInstance {
 public:
  ScriptInstance(ScriptEnvironment*, wasm_module_t*);
  ~ScriptInstance();

  // TODO(marceline-cramer) Make observers in ScriptEnvironment for events
  // TODO(marceline-cramer) Define entrypoint classes and their sizes
  // TODO(marceline-cramer) Dynamic ScriptInstance Wasm allocation
  // TODO(marceline-cramer) Pass EntityIds to scripts
  void addCallback(const std::string&, wasm_func_t*);
  bool hasCallback(const std::string&);
  void runCallback(const std::string&, EntityId);

  // TODO(marceline-cramer) Actually update instance data
  void updateData(const uint8_t*, size_t) {}

 private:
  ScriptEnvironment* scripts;

  wasm_instance_t* module_instance = nullptr;
  std::unordered_map<std::string, wasm_func_t*> callbacks;
};

}  // namespace mondradiko
