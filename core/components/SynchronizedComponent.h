// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/InternalComponent.h"
#include "lib/include/flatbuffers_headers.h"

namespace mondradiko {

// Forward declarations
namespace protocol {
struct UpdateComponentsBuilder;
}  // namespace protocol

namespace core {

// Forward declarations
class AssetPool;
class ScriptEnvironment;
class World;

// Forward declare template for UpdateComponents building
template <class ProtocolComponentType>
void buildUpdateComponents(
    protocol::UpdateComponentsBuilder*,
    flatbuffers::Offset<flatbuffers::Vector<const ProtocolComponentType*>>);

template <typename DataType>
class SynchronizedComponent : public InternalComponent {
 public:
  // Make data type public so other classes can use it
  using SerializedType = DataType;

  SynchronizedComponent() {}
  explicit SynchronizedComponent(const SerializedType& data) : _data(data) {}

  void refresh(AssetPool*) {}

  const SerializedType& getData() const { return _data; }
  void writeData(const SerializedType& data) { _data = data; }

 protected:
  SerializedType _data;
};

}  // namespace core
}  // namespace mondradiko
