// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UiDrawList.h"

#include <utility>

#include "core/gpu/GpuVector.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

UiDrawList::UiDrawList() {
  const float delta = M_PI * 2.0 / SPOKE_COUNT;

  float theta = 0.0;
  for (int i = 0; i < SPOKE_COUNT; i++) {
    _spokes[i] = glm::vec2(sin(theta), cos(theta));
    theta += delta;
  }
}

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

void UiDrawList::drawCircle(const glm::vec2& center, float radius,
                            const glm::vec4& color) {
  log_zone;

  Index first_index = _vertices.size();
  _vertices.resize(first_index + SPOKE_COUNT + 1);

  Index center_index = first_index;
  ++first_index;

  {  // Place center vertex
    Vertex& center_vertex = _vertices[center_index];
    center_vertex.position = center;
    center_vertex.color = color;
  }

  {  // Write spoke vertices
    Vertex* spoke_vertex = &_vertices[first_index];

    // Copy spoke positions
    glm::vec2* spoke_position = reinterpret_cast<glm::vec2*>(spoke_vertex);
    memcpy(spoke_position, _spokes.data(), SPOKE_COUNT * sizeof(glm::vec2));

    // Write backwards, calculate spoke positions, and interleave color data
    spoke_position += SPOKE_COUNT - 1;
    spoke_vertex = &_vertices[first_index + (SPOKE_COUNT - 1)];
    for (int i = 0; i < SPOKE_COUNT; i++) {
      spoke_vertex->position = (*spoke_position) * radius + center;
      spoke_vertex->color = color;
      --spoke_vertex;
      --spoke_position;
    }
  }

  {  // Write pie triangles
    size_t first_spoke = _indices.size();
    _indices.resize(first_spoke + (SPOKE_COUNT * 3));
    Index* spoke_index = &_indices[first_spoke];

    Index last_spoke = first_index + (SPOKE_COUNT - 1);
    for (int i = 0; i < SPOKE_COUNT; i++) {
      *(spoke_index++) = center_index;
      *(spoke_index++) = last_spoke;
      last_spoke = i + first_index;
      *(spoke_index++) = last_spoke;
    }
  }
}

void UiDrawList::drawRing(const glm::vec2& center, float inner_radius,
                          float outer_radius, const glm::vec4& color) {
  log_zone;

  Index first_index = _vertices.size();
  _vertices.resize(first_index + SPOKE_COUNT * 2);

  {  // Write edge vertices
    Vertex* spoke_vertex = &_vertices[first_index];

    // Copy spoke positions
    glm::vec2* spoke_position = reinterpret_cast<glm::vec2*>(spoke_vertex);
    memcpy(spoke_position, _spokes.data(), SPOKE_COUNT * sizeof(glm::vec2));

    // Write backwards, calculate spoke positions, and interleave color data
    spoke_position += SPOKE_COUNT - 1;
    spoke_vertex = &_vertices[first_index + (SPOKE_COUNT * 2 - 1)];
    for (int i = 0; i < SPOKE_COUNT; i++) {
      spoke_vertex->position = (*spoke_position) * inner_radius + center;
      spoke_vertex->color = color;
      --spoke_vertex;

      spoke_vertex->position = (*spoke_position) * outer_radius + center;
      spoke_vertex->color = color;
      --spoke_vertex;

      --spoke_position;
    }
  }

  {  // Write edge triangles
    size_t first_indice_index = _indices.size();

    _indices.resize(first_indice_index + (SPOKE_COUNT * 6));
    Index* spoke_index = &_indices[first_indice_index];

    Index last_inner = first_index + (SPOKE_COUNT * 2 - 1);
    Index last_outer = last_inner - 1;
    for (int i = 0; i < SPOKE_COUNT * 2; i += 2) {
      *(spoke_index++) = last_inner;
      *(spoke_index++) = last_outer;
      last_outer = i + first_index;
      *(spoke_index++) = last_outer;

      *(spoke_index++) = last_outer;
      *(spoke_index++) = last_inner;
      last_inner = last_outer + 1;
      *(spoke_index++) = last_inner;
    }
  }
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
