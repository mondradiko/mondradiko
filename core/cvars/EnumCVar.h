// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/cvars/CVarValueInterface.h"
#include "log/log.h"
#include "types/containers/string.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

class EnumCVar : public CVarValueInterface {
 public:
  using EnumRange = types::vector<types::string>;

  explicit EnumCVar(const EnumRange& range) : range(range) {}

  const types::string& str() const { return value; }
  const char* c_str() const { return value.c_str(); }

  bool test(const char* value_case) const {
    bool found = false;
    for (auto& candidate : range) {
      if (candidate == value_case) {
        found = true;

        if (value == candidate) {
          return true;
        }
      }
    }

    if (!found) {
      log_wrn_fmt("Tested enum value %s is not within range", value_case);
    }

    return false;
  }

 private:
  EnumRange range;
  types::string value;

  // CVarValueInterface implementation
  bool loadConfig(const toml::value& config) final {
    value = config.as_string();

    bool found = false;
    for (auto& candidate : range) {
      if (candidate == value) {
        found = true;
        break;
      }
    }

    if (!found) {
      log_err_fmt("Invalid enum member %s", value.c_str());
      log_err_fmt("Candidates are:");

      for (auto& candidate : range) {
        log_err_fmt("  %s", candidate.c_str());
      }

      return false;
    } else {
      return true;
    }
  }
};

}  // namespace core
}  // namespace mondradiko
