// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

namespace mondradiko {
namespace core {

// Forward declarations
class ComponentScriptEnvironment;
class World;

class ScriptEntity {
 public:
  static void linkScriptApi(World*);
};

}  // namespace core
}  // namespace mondradiko
