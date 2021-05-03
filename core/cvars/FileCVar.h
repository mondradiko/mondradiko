// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <string>

#include "core/cvars/CVarValueInterface.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

class FileCVar : public CVarValueInterface {
 public:
  void setRoot(const std::filesystem::path& root) {
    _root = root;
    _initialized = true;
  }

  const std::filesystem::path& getPath() const { return _path; }

 protected:
  bool _initialized = false;
  std::filesystem::path _root = "";
  std::filesystem::path _path = "";

  // CVarValueInterface implementation
  bool loadConfig(const toml::value& config) final {
    if (!_initialized) {
      log_err_fmt("Root path has not been set");
      return false;
    }

    std::string value = config.as_string();
    _path = std::filesystem::absolute(_root / value);

    if (!std::filesystem::exists(_path)) {
      log_wrn_fmt("File %s does not exist", _path.c_str());
    }

    return true;
  }
};

}  // namespace core
}  // namespace mondradiko
