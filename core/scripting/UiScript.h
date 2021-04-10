// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/ScriptInstance.h"

namespace mondradiko {
namespace core {

class UiScript : public ScriptInstance {
 public:
  UiScript(ScriptEnvironment*, wasm_module_t*);

  void update(double);

 private:
};

}  // namespace core
}  // namespace mondradiko
