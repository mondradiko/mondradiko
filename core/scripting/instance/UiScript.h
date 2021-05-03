// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/instance/ScriptInstance.h"

namespace mondradiko {
namespace core {

// Forward declarations
class UiPanel;

class UiScript : public ScriptInstance {
 public:
  UiScript(ScriptEnvironment*, wasm_module_t*);

  void handleMessage(const types::string&);

 private:
};

}  // namespace core
}  // namespace mondradiko
