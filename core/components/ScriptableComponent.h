// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/SynchronizedComponent.h"

namespace mondradiko {
namespace core {

template <typename Inheritor, typename DataType>
class ScriptableComponent : public SynchronizedComponent<Inheritor, DataType> {
 public:
  ScriptableComponent() {}
  explicit ScriptableComponent(const DataType& data)
      : SynchronizedComponent<Inheritor, DataType>(data) {}

  // Defined in generated API linker
  static void linkScriptApi(ScriptEnvironment*, World*);
};

}  // namespace core
}  // namespace mondradiko
