// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/gpu/GpuPipeline.h"
#include "lib/include/glm_headers.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GpuVector;

class DebugDrawList {
 public:
  //////////////////////////////////////////////////////////////////////////////
  // Type definitions
  //////////////////////////////////////////////////////////////////////////////

  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;

    static GpuPipeline::VertexBindings getVertexBindings() {
      // VkVertexInputBindingDescription{binding, stride, inputRate}
      return {
          {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX},
      };
    }

    static GpuPipeline::AttributeDescriptions getAttributeDescriptions() {
      // VkVertexInputAttributeDescription{location, binding, format, offset}
      return {
          {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
          {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
      };
    }
  };

  using Index = uint16_t;

  //////////////////////////////////////////////////////////////////////////////
  // Operations
  //////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Destroys all vertices and indices.
   */
  void clear();

  /**
   * @brief Creates a vertex.
   * @param vertex The vertex to create.
   * @return The index of the vertex that was just created.
   */
  Index makeVertex(const Vertex&);

  /**
   * @brief Creates a vertex from attributes.
   * @param position The position of the new vertex.
   * @param color The color of the new vertex.
   * @return The index of the vertex that was just created.
   */
  Index makeVertex(const glm::vec3&, const glm::vec3&);

  /**
   * @brief Draws a line between two vertices.
   * @param start The vertex at the start of the line.
   * @param end The vertex at the end of the line.
   */
  void drawLine(Index, Index);

  /**
   * @brief Draws a line between two positions.
   * @param start The starting position of the line.
   * @param end The ending position of the line.
   * @param color The color of the line.
   */
  void drawLine(const glm::vec3&, const glm::vec3&, const glm::vec3&);

  //////////////////////////////////////////////////////////////////////////////
  // Render commands
  //////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Writes the draw data to GPU-side vectors for rendering.
   * @param vertex_buffer The vector to copy the vertices to.
   * @param index_buffer The vector to copy the indices to.
   * @return The number of indices to draw with these buffers.
   */
  Index writeData(GpuVector*, GpuVector*);

 private:
  types::vector<Vertex> _vertices;
  types::vector<Index> _indices;
};

}  // namespace core
}  // namespace mondradiko
