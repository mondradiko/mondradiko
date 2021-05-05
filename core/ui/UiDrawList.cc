// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UiDrawList.h"

#include <utility>

#include "core/gpu/GpuVector.h"

namespace mondradiko {
namespace core {

////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////

void UiDrawList::clear() {
  _vertices.clear();
  _indices.clear();
}

UiDrawList::Index UiDrawList::makeVertex(const Vertex& vertex) {
  Index index = _vertices.size();
  _vertices.push_back(vertex);
  return index;
}

UiDrawList::Index UiDrawList::makeVertex(const glm::vec2& position,
                                         const glm::vec4& color) {
  Vertex vertex;
  vertex.position = position;
  vertex.color = color;
  return makeVertex(vertex);
}

void UiDrawList::drawTriangle(Index vertex1, Index vertex2, Index vertex3) {
  _indices.push_back(vertex1);
  _indices.push_back(vertex2);
  _indices.push_back(vertex3);
}

////////////////////////////////////////////////////////////////////////////////
// Render commands
////////////////////////////////////////////////////////////////////////////////

UiDrawList::Index UiDrawList::writeData(GpuVector* vertex_vector,
                                        GpuVector* index_vector) {
  vertex_vector->writeData(0, _vertices);
  index_vector->writeData(0, _indices);
  return _indices.size();
}

}  // namespace core
}  // namespace mondradiko
