// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/flatbuffers_headers.h"
#include "types/protocol/ClientEvent_generated.h"

namespace mondradiko {
namespace core {

class Avatar {
 public:
  virtual ~Avatar() {}

  using ProtocolBuffer = flatbuffers::Vector<uint8_t>;
  using ProtocolBufferBuilder = flatbuffers::FlatBufferBuilder;
  using ProtocolBufferOffset = flatbuffers::Offset<ProtocolBuffer>;

  virtual protocol::AvatarType getProtocolType() const = 0;
  virtual ProtocolBufferOffset serialize(ProtocolBufferBuilder*) const = 0;
  virtual void deserialize(const ProtocolBuffer*) = 0;
};

}  // namespace core
}  // namespace mondradiko
