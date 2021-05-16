// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <type_traits>

#include "lib/include/toml_headers.h"
#include "log/log.h"
#include "types/containers/map.h"
#include "types/containers/string.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class CVarValueInterface;
class FileCVar;
class Filesystem;

class CVarScope {
 public:
  CVarScope();
  CVarScope(const CVarScope*, const types::string&);
  ~CVarScope();

  CVarScope* addChild(const types::string&);
  const CVarScope* getChild(const types::string&) const;

  void loadConfig(const toml::value&, const std::filesystem::path&);
  void loadConfigFromFile(Filesystem*, const std::filesystem::path&);
  void saveConfig(toml::value*);

  const CVarScope* getParent() const { return parent; }
  const types::string& getScopePath() const { return scope_path; }
  types::string getValuePath(const types::string&) const;

  using KeyType = types::string;

  void addFile(const KeyType&);
  std::filesystem::path getFile(const KeyType&);

  template <class ValueType, typename... Args>
  void addValue(const KeyType& key, Args&&... args) {
    if (children.find(key) != children.end()) {
      log_ftl_fmt("CVar %s shadows child scope", getValuePath(key).c_str());
    } else if (values.find(key) != values.end()) {
      log_ftl_fmt("Attempted to overwrite CVar %s", getValuePath(key).c_str());
    }

    ValueType* new_value = new ValueType(args...);
    values.emplace(key, new_value);

    if constexpr (std::is_base_of<FileCVar, ValueType>::value) {
      _file_cvars.push_back(new_value);
    }
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
  types::string scope_path;
  types::map<types::string, CVarScope*> children;
  types::map<KeyType, CVarValueInterface*> values;
  types::vector<FileCVar*> _file_cvars;
};

}  // namespace core
}  // namespace mondradiko
