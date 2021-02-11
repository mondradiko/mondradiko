// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <map>
#include <string>

#include "lib/include/toml_headers.h"
#include "log/log.h"

namespace mondradiko {

// Forward declarations
class CVarValueInterface;

class CVarScope {
 public:
  CVarScope();
  CVarScope(const CVarScope*, const std::string&);
  ~CVarScope();

  CVarScope* addChild(const std::string&);
  const CVarScope* getChild(const std::string&) const;

  void loadConfig(const toml::value&);
  void saveConfig(toml::value*);

  const CVarScope* getParent() const { return parent; }
  const std::string& getScopePath() const { return scope_path; }
  std::string getValuePath(const std::string&) const;

  using KeyType = std::string;

  template <class ValueType, typename... Args>
  void addValue(const KeyType& key, Args&&... args) {
    if (children.find(key) != children.end()) {
      log_ftl_fmt("CVar %s shadows child scope", getValuePath(key).c_str());
    } else if (values.find(key) != values.end()) {
      log_ftl_fmt("Attempted to overwrite CVar %s", getValuePath(key).c_str());
    }

    ValueType* new_value = new ValueType(args...);
    values.emplace(key, new_value);
  }

  template <class ValueType>
  const ValueType& get(const KeyType& key) const {
    auto iter = values.find(key);
    if (iter == values.end()) {
      log_ftl_fmt("CVar %s does not exist", key.c_str());
    }

    const ValueType* value = dynamic_cast<ValueType*>(iter->second);
    if (value == nullptr) {
      log_ftl_fmt("CVar %s is not %s", key.c_str(), typeid(ValueType).name());
    }

    return *value;
  }

 private:
  const CVarScope* parent = nullptr;
  std::string scope_path;
  std::map<std::string, CVarScope*> children;
  std::map<KeyType, CVarValueInterface*> values;
};

}  // namespace mondradiko
