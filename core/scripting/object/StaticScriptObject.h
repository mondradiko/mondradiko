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

  StaticScriptObject() : StaticScriptObject(nullptr) {}

  StaticScriptObject(ScriptEnvironment* scripts)  // NOLINT (runtime/explicit)
      : _host_environment(nullptr), _static_symbol(typeid(T).name()) {
    linkToEnvironment(scripts);
  }

  ~StaticScriptObject() { linkToEnvironment(nullptr); }

  bool linkToEnvironment(ScriptEnvironment* scripts) {
    // Remove ourselves from our previous host environment
    if (_host_environment != nullptr) {
      _host_environment->removeStaticObject(_static_symbol);
    }

    // Associate ourselves with the new host environment
    _host_environment = scripts;

    // If the new host turns out to be nullptr, quit
    if (_host_environment == nullptr) return false;

    // Upcast to implementation
    T* impl = static_cast<T*>(this);

    // Otherwise, store ourselves
    if (!_host_environment->storeStaticObject(_static_symbol, impl)) {
      // If an object of our type was already registered, quit
      return false;
    }

    // Link our script API and exit successfully
    linkScriptApi(_host_environment, impl);
    return true;
  }

 private:
  ScriptEnvironment* _host_environment;
  const char* _static_symbol;
};

}  // namespace core
}  // namespace mondradiko
