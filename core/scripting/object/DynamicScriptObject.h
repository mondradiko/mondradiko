// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/environment/ScriptEnvironment.h"

namespace mondradiko {
namespace core {

template <class T>
class DynamicScriptObject {
 public:
  // Defined in generated API linker
  static void linkScriptApi(ScriptEnvironment*);

  explicit DynamicScriptObject(ScriptEnvironment* scripts) : scripts(scripts) {
    _object_id = scripts->storeInRegistry(static_cast<T*>(this));
  }

  ~DynamicScriptObject() { scripts->removeFromRegistry(_object_id); }

  uint32_t getObjectKey() { return _object_id; }

 protected:
  ScriptEnvironment* const scripts;

 private:
  uint32_t _object_id;
};

}  // namespace core
}  // namespace mondradiko
