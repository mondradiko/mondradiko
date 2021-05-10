// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/InternalComponent.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ComponentScript;

class ScriptComponent : public InternalComponent {
 public:
 private:
  // Systems allowed to access private members directly
  friend class ComponentScriptEnvironment;

  types::string _script_impl;
  ComponentScript* _script_instance;
};

}  // namespace core
}  // namespace mondradiko
