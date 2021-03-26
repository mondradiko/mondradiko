// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/cvars/CVarScope.h"

#include "core/cvars/CVarValueInterface.h"

namespace mondradiko {
namespace core {

CVarScope::CVarScope() : parent(nullptr), scope_path("") {}

CVarScope::CVarScope(const CVarScope* parent, const types::string& child_name)
    : parent(parent) {
  scope_path = parent->getValuePath(child_name);
}

CVarScope::~CVarScope() {
  for (auto child : children) {
    delete child.second;
  }

  for (auto value : values) {
    delete value.second;
  }
}

types::string CVarScope::getValuePath(const types::string& key) const {
  if (parent != nullptr) {
    return getScopePath() + "." + key;
  } else {
    return key;
  }
}

CVarScope* CVarScope::addChild(const types::string& child_name) {
  if (values.find(child_name) != values.end()) {
    log_ftl_fmt("Child scope %s shadows CVar",
                getValuePath(child_name).c_str());
  } else if (children.find(child_name) != children.end()) {
    log_ftl_fmt("Attempted to overwrite child scope %s",
                getValuePath(child_name).c_str());
  }

  CVarScope* child = new CVarScope(this, child_name);
  children.emplace(child_name, child);
  return child;
}

const CVarScope* CVarScope::getChild(const types::string& child_name) const {
  auto iter = children.find(child_name);
  if (iter == children.end()) {
    log_ftl_fmt("Child scope %s does not exist",
                getValuePath(child_name).c_str());
  }

  return iter->second;
}

void CVarScope::loadConfig(const toml::value& config) {
  const auto& config_values = config.as_table();

  for (auto child : children) {
    if (config_values.find(child.first) == config_values.end()) {
      log_ftl_fmt("Missing config child scope %s",
                  getValuePath(child.first).c_str());
    }
  }

  for (auto value : values) {
    if (config_values.find(value.first) == config_values.end()) {
      log_ftl_fmt("Missing config value %s", getValuePath(value.first).c_str());
    }
  }

  for (auto config_value : config_values) {
    auto value = values.find(config_value.first);
    auto child = children.find(config_value.first);

    if (value != values.end()) {
      bool result = value->second->loadConfig(config_value.second);
      if (result == false) {
        log_ftl_fmt("Failed to load %s",
                    getValuePath(config_value.first).c_str());
      }
    } else if (child != children.end()) {
      child->second->loadConfig(config_value.second);
    } else {
      log_inf_fmt("Ignoring config value %s",
                  getValuePath(config_value.first).c_str());
    }
  }
}

void CVarScope::saveConfig(toml::value*) {}

}  // namespace core
}  // namespace mondradiko
