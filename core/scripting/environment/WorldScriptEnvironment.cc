// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/environment/WorldScriptEnvironment.h"

#include "core/scripting/environment/ComponentScriptEnvironment.h"
#include "core/scripting/instance/WorldScript.h"
#include "core/world/ScriptEntity.h"
#include "core/world/World.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

WorldScriptEnvironment::WorldScriptEnvironment(World* world,
                                               const types::string& script_path)
    : world(world) {
  log_zone;

  ComponentScriptEnvironment::linkEnvironment(this, world);

  if (!world->linkToEnvironment(this)) {
    log_wrn("Failed to link to World");
  }

  wasi_config_t* wasi_config = wasi_config_new();
  wasi_config_inherit_env(wasi_config);
  wasi_config_inherit_stdin(wasi_config);
  wasi_config_inherit_stderr(wasi_config);
  wasi_config_inherit_stdout(wasi_config);

  wasm_trap_t* trap = nullptr;
  wasi_instance_t* wasi_instance = wasi_instance_new(
      getStore(), "wasi_snapshot_preview1", wasi_config, &trap);

  if (handleError(nullptr, trap)) {
    log_ftl("Failed to create WASI instance");
  }

  wasmtime_linker_t* linker = wasmtime_linker_new(getStore());
  wasmtime_error_t* error = nullptr;

  error = wasmtime_linker_define_wasi(linker, wasi_instance);
  if (handleError(error, nullptr)) {
    log_ftl("Failed to link WASI");
    wasmtime_linker_delete(linker);
    wasi_instance_delete(wasi_instance);
  }

  types::vector<char> script_data;
  if (!world->fs->loadBinaryFile(script_path, &script_data)) {
    log_ftl("Failed to load world script");
  }

  wasm_module_t* module = loadBinaryModule(script_data);
  if (module == nullptr) {
    log_ftl("Failed to load script module");
    wasmtime_linker_delete(linker);
    wasi_instance_delete(wasi_instance);
  }

  _instance = new WorldScript(this, world);
  _instance->initializeScriptFromLinker(module, linker);
  _instance->runCallback("_start", nullptr, 0, nullptr, 0);
}

WorldScriptEnvironment::~WorldScriptEnvironment() {
  log_zone;

  if (_instance != nullptr) delete _instance;

  world->linkToEnvironment(nullptr);
}

void WorldScriptEnvironment::update(double dt) {
  log_zone;

  wasm_val_t dt_arg;
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;

  _instance->runCallback("on_update", &dt_arg, 1, nullptr, 0);
}

}  // namespace core
}  // namespace mondradiko
