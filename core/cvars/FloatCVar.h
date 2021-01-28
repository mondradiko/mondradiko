// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/cvars/CVarValueInterface.h"
#include "log/log.h"

namespace mondradiko {

class FloatCVar : public CVarValueInterface {
 public:
  FloatCVar(double min, double max) : min(min), max(max) {}

  operator double() const { return value; }

 protected:
  double min;
  double max;
  double value;

  // CVarValueInterface implementation
  bool loadConfig(const toml::value& config) final {
    value = config.as_floating();
    if (value < min || value > max) {
      log_err("Value is outside of range [%f - %f]", min, max);
      return false;
    }

    return true;
  }
};

}  // namespace mondradiko
