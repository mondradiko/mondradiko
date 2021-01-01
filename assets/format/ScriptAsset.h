/**
 * @file ScriptAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Defines the data stored in a ScriptAsset.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <cstdint>

namespace mondradiko {
namespace assets {

enum class ScriptType : uint8_t { None = 0, Text, Binary };

struct ScriptHeader {
  ScriptType type;
  uint32_t module_size;
};

}  // namespace assets
}  // namespace mondradiko
