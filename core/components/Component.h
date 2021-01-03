/**
 * @file Component.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Component common definitions.
 * @date 2020-12-16
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/common/api_headers.h"
#include "flatbuffers/flatbuffers.h"

namespace mondradiko {

// Forward declarations
class ScriptEnvironment;
class World;
namespace protocol {
struct UpdateComponentsBuilder;
}

// Forward declare template for UpdateComponents building
template <class ProtocolComponentType>
void buildUpdateComponents(
    protocol::UpdateComponentsBuilder*,
    flatbuffers::Offset<flatbuffers::Vector<const ProtocolComponentType*>>);

template <typename DataType>
class Component {
 public:
  // Make data type public so other classes can use it
  using SerializedType = DataType;

  Component() {}
  explicit Component(const SerializedType& data) : _data(data) {}

  void markDirty() { dirty = true; }
  bool isDirty() { return dirty; }
  void markClean() { dirty = false; }

  const SerializedType& getData() const { return _data; }
  void writeData(const SerializedType& data) { _data = data; }

  // Overridden by inherited components
  // Implemented by generated component API linker
  static void linkScriptApi(ScriptEnvironment*, World*) = delete;

 protected:
  SerializedType _data;

 private:
  bool dirty = true;
};

}  // namespace mondradiko
