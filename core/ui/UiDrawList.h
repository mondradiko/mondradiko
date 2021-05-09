// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/gpu/GpuPipeline.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GpuVector;

class UiDrawList {
 public:
  UiDrawList();

  //////////////////////////////////////////////////////////////////////////////
  // Type definitions
  //////////////////////////////////////////////////////////////////////////////

  struct Vertex {
    glm::vec2 position;
    glm::vec4 color;

    static GpuPipeline::VertexBindings getVertexBindings() {
      // VkVertexInputBindingDescription{binding, stride, inputRate}
      return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
    }

    static GpuPipeline::AttributeDescriptions getAttributeDescriptions() {
      // VkVertexInputAttributeDescription{location, binding, format, offset}
      return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)},
              {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)}};
    }
  };

  using Index = uint32_t;

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
  Index makeVertex(const glm::vec2&, const glm::vec4&);

  /**
   * @brief Draws a triangle.
   * @param vertex1 The first vertex of the triangle.
   * @param vertex2 The second vertex of the triangle.
   * @param vertex3 The third vertex of the triangle.
   */
  void drawTriangle(Index, Index, Index);

  /**
   * @brief Draws a circle.
   * @param center The center of the circle.
   * @param radius The radius of the circle.
   * @param color The color of the circle.
   */
  void drawCircle(const glm::vec2&, float, const glm::vec4&);

  /**
   * @brief Draws a ring.
   * @param center The center of the ring.
   * @param inner_radius The inner radius of the ring.
   * @param outer_radius The outer radius of the ring.
   * @param color The color of the ring.
   */
  void drawRing(const glm::vec2&, float, float, const glm::vec4&);

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

  // For drawing circular shapes
  constexpr static int SPOKE_COUNT = 64;
  std::array<glm::vec2, SPOKE_COUNT> _spokes;
};

}  // namespace core
}  // namespace mondradiko
