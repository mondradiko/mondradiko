// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/DebugDrawList.h"

#include <utility>

#include "core/gpu/GpuVector.h"

namespace mondradiko {
namespace core {

////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////

void DebugDrawList::clear() {
  _vertices.clear();
  _indices.clear();
}

DebugDrawList::Index DebugDrawList::makeVertex(const Vertex& vertex) {
  Index index = _vertices.size();
  _vertices.push_back(vertex);
  return index;
}

DebugDrawList::Index DebugDrawList::makeVertex(const glm::vec3& position,
                                               const glm::vec3& color) {
  Vertex vertex;
  vertex.position = position;
  vertex.color = color;
  return makeVertex(vertex);
}

void DebugDrawList::drawLine(Index start, Index end) {
  _indices.push_back(start);
  _indices.push_back(end);
}

void DebugDrawList::drawLine(const glm::vec3& start, const glm::vec3& end,
                             const glm::vec3& color) {
  Vertex start_vertex;
  start_vertex.position = start;
  start_vertex.color = color;
  Index start_index = makeVertex(std::move(start_vertex));

  Vertex end_vertex;
  end_vertex.position = end;
  end_vertex.color = color;
  Index end_index = makeVertex(std::move(end_vertex));

  drawLine(start_index, end_index);
}

void DebugDrawList::drawIcosahedron(const glm::vec3& center, double radius,
                                    const glm::vec3& color) {
  const float X = 0.525731112119133606;
  const float Z = 0.850650808352039932;
  const float N = 0.0;

  const size_t vertex_num = 12;

  const glm::vec3 vertex_positions[vertex_num] = {
      {-X, N, Z}, {X, N, Z},  {-X, N, -Z}, {X, N, -Z},
      {N, Z, X},  {N, Z, -X}, {N, -Z, X},  {N, -Z, -X},
      {Z, X, N},  {-Z, X, N}, {Z, -X, N},  {-Z, -X, N}};

  const size_t edge_indices[] = {
      0, 1, 0, 4, 0, 6, 0, 9,  0, 11, 1, 4,  1, 6,  1, 8,  1, 10, 2, 3,
      2, 5, 2, 7, 2, 9, 2, 11, 3, 5,  3, 7,  3, 8,  3, 10, 4, 5,  4, 8,
      4, 9, 5, 8, 5, 9, 6, 7,  6, 10, 6, 11, 7, 10, 7, 11, 8, 10, 9, 11};

  Index vertices[vertex_num];
  for (size_t i = 0; i < vertex_num; i++) {
    glm::vec3 position = vertex_positions[i] * glm::vec3(radius) + center;
    vertices[i] = makeVertex(position, color);
  }

  for (const auto& i : edge_indices) {
    _indices.push_back(vertices[i]);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Render commands
////////////////////////////////////////////////////////////////////////////////

DebugDrawList::Index DebugDrawList::writeData(GpuVector* vertex_vector,
                                              GpuVector* index_vector) {
  for (Index i = 0; i < _vertices.size(); i++) {
    vertex_vector->writeElement(i, _vertices[i]);
  }

  for (Index i = 0; i < _indices.size(); i++) {
    index_vector->writeElement(i, _indices[i]);
  }

  return _indices.size();
}

}  // namespace core
}  // namespace mondradiko
