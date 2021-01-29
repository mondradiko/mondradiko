// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/cvars/CVarValueInterface.h"
#include "log/log.h"

namespace mondradiko {

class FloatCVar : public CVarValueInterface {
 public:
  FloatCVar(double min_val, double max_val)
      : min_val(min_val), max_val(max_val) {}

  operator double() const { return value; }

 protected:
  double min_val;
  double max_val;
  double value;

  // CVarValueInterface implementation
  bool loadConfig(const toml::value& config) final {
    value = config.as_floating();
    if (value < min_val || value > max_val) {
      log_err("Value is outside of range [%f - %f]", min_val, max_val);
      return false;
    }

    return true;
  }
};

}  // namespace mondradiko
