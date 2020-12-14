/**
 * @file MaterialAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-13
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "glm/glm.hpp"

namespace mondradiko {
namespace assets {

struct MaterialHeader {
  glm::vec4 albedo_factor;
};

}  // namespace assets
}  // namespace mondradiko
