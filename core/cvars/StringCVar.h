// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/cvars/CVarValueInterface.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

class StringCVar : public CVarValueInterface {
 public:
  const types::string& str() const { return value; }
  const char* c_str() const { return value.c_str(); }

 protected:
  types::string value;

  // CVarValueInterface implementation
  bool loadConfig(const toml::value& config) final {
    value = config.as_string();
    return true;
  }
};

}  // namespace core
}  // namespace mondradiko
