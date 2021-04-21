// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/environment/ScriptEnvironment.h"

namespace mondradiko {
namespace core {

template <class T>
class StaticScriptObject {
 public:
  // Defined in generated API linker
  static void linkScriptApi(ScriptEnvironment*, T*);

  StaticScriptObject(ScriptEnvironment* scripts, const char* symbol)
      : scripts(scripts), _static_symbol(symbol) {
    if (scripts != nullptr &&
        scripts->storeStaticObject(_static_symbol, this)) {
      linkScriptApi(scripts, static_cast<T*>(this));
    }
  }

  ~StaticScriptObject() {
    if (scripts != nullptr) scripts->removeStaticObject(_static_symbol);
  }

 private:
  ScriptEnvironment* const scripts;
  const char* _static_symbol;
};

}  // namespace core
}  // namespace mondradiko
