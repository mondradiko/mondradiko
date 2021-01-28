// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/cvars/CVarValueInterface.h"

namespace mondradiko {

class BoolCVar : public CVarValueInterface {
 public:
  operator bool() const { return value; }

 protected:
  bool value;

  // CVarValueInterface implementation
  bool loadConfig(const toml::value& config) final {
    value = config.as_boolean();
    return true;
  }
};

}  // namespace mondradiko
