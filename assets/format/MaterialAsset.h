/**
 * @file MaterialAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-13
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <glm/glm.hpp>

#include "assets/format/AssetTypes.h"

namespace mondradiko {
namespace assets {

struct MaterialHeader {
  glm::vec4 albedo_factor;
  AssetId albedo_texture;
};

}  // namespace assets
}  // namespace mondradiko
