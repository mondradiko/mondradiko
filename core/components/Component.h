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
namespace protocol {
struct UpdateComponentsBuilder;
}

// Forward declare template for UpdateComponents building
template <class ProtocolComponentType>
void buildUpdateComponents(
    protocol::UpdateComponentsBuilder&,
    flatbuffers::Offset<flatbuffers::Vector<const ProtocolComponentType*>>&);

}  // namespace mondradiko
