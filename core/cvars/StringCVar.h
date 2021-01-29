// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>

#include "core/cvars/CVarValueInterface.h"

namespace mondradiko {

class StringCVar : public CVarValueInterface {
 public:
  const std::string& str() const { return value; }
  const char* c_str() const { return value.c_str(); }

 protected:
  std::string value;

  // CVarValueInterface implementation
  bool loadConfig(const toml::value& config) final {
    value = config.as_string();
    return true;
  }
};

}  // namespace mondradiko
