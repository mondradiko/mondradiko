// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/instance/ComponentScript.h"

#include <array>

#include "core/scripting/environment/ComponentScriptEnvironment.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

ComponentScript::ComponentScript(ComponentScriptEnvironment* scripts,
                                 World* world,
                                 const AssetHandle<ScriptAsset>& asset,
                                 EntityId self_id, const types::string& impl)
    : WorldScript(scripts, world),
      _asset(asset),
      _impl(impl),
      _self_id(self_id) {
  initializeScript(asset->getModule());

  wasm_val_t self_arg;
  self_arg.kind = WASM_I32;
  self_arg.of.i32 = _self_id;

  if (!AS_construct(_impl, &self_arg, 1, &_this_ptr)) {
    log_err_fmt("Failed to construct component script %s", _impl.c_str());
    return;
  }

  AS_pin(_this_ptr);

  types::string update = _impl + "#update";
  _update = getCallback(update);

  if (_update == nullptr) {
    log_wrn_fmt("Component script %s does not export update", _impl.c_str());
  }
}

ComponentScript::~ComponentScript() { AS_unpin(_this_ptr); }

void ComponentScript::update(double dt) {
  if (_update == nullptr) return;

  std::array<wasm_val_t, 2> args;

  auto& this_arg = args[0];
  this_arg.kind = WASM_I32;
  this_arg.of.i32 = _this_ptr;

  auto& dt_arg = args[1];
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;

  runFunction(_update, args.data(), args.size(), nullptr, 0);
}

}  // namespace core
}  // namespace mondradiko
