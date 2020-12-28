/**
 * @file MeshAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Describes the data stored in a MeshAsset.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "glm/glm.hpp"

namespace mondradiko {
namespace assets {

/**
 * MeshAssets are laid out like this:
 *
 * MeshHeader header;
 * MeshVertex vertices[header.vertex_count];
 * MeshIndex indices[header.index_count];
 * (EOF)
 *
 */

struct MeshHeader {
  uint32_t vertex_count;
  uint32_t index_count;
};

struct MeshVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 tex_coord;
};

using MeshIndex = uint32_t;

}  // namespace assets
}  // namespace mondradiko
