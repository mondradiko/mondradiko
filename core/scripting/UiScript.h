// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/ScriptInstance.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class UiPanel;

class UiScript : public ScriptInstance {
 public:
  UiScript(ScriptEnvironment*, wasm_module_t*);

  uint32_t bindPanel(UiPanel*);
  void selectAt(const glm::vec2&);
  void update(double);

 private:
};

}  // namespace core
}  // namespace mondradiko
