// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/toml_headers.h"

namespace mondradiko {

class CVarValueInterface {
 public:
  virtual ~CVarValueInterface() {}

 protected:
  friend class CVarScope;
  virtual bool loadConfig(const toml::value&) = 0;

 private:
};

}  // namespace mondradiko
