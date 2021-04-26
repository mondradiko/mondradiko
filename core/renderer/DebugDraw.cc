// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/DebugDraw.h"

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
