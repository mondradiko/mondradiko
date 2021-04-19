// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/MeshAsset.h"

#include "core/assets/Asset.h"
#include "core/renderer/MeshPass.h"
#include "core/renderer/Renderer.h"
#include "log/log.h"
#include "types/assets/MeshAsset_generated.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

bool MeshAsset::_load(const assets::SerializedAsset* asset) {
  // Skip loading if we initialized as a dummy
  if (mesh_pass == nullptr) return true;

  const assets::MeshAsset* mesh = asset->mesh();

  types::vector<MeshVertex> vertices(mesh->vertices()->size());
  types::vector<MeshIndex> indices(mesh->indices()->size());

  for (uint32_t i = 0; i < vertices.size(); i++) {
    const assets::MeshVertex* vertex = mesh->vertices()->Get(i);

    vertices[i].position = glm::make_vec3(vertex->position().v()->data());
    vertices[i].tex_coord = glm::make_vec2(vertex->tex_coord().v()->data());
    vertices[i].color = glm::make_vec3(vertex->color().v()->data());
    vertices[i].normal = glm::make_vec3(vertex->normal().v()->data());
    vertices[i].tangent = glm::make_vec3(vertex->tangent().v()->data());
  }

  for (uint32_t i = 0; i < indices.size(); i++) {
    indices[i] = mesh->indices()->Get(i);
  }

  vertex_offset = mesh_pass->allocateVertices(vertices.size());
  index_num = indices.size();
  index_offset = mesh_pass->allocateIndices(index_num);

  Renderer* renderer = mesh_pass->getRenderer();

  renderer->transferDataToBuffer(
      mesh_pass->getVertexPool(), vertex_offset * sizeof(MeshVertex),
      vertices.data(), vertices.size() * sizeof(MeshVertex));
  renderer->transferDataToBuffer(
      mesh_pass->getIndexPool(), index_offset * sizeof(MeshIndex),
      indices.data(), indices.size() * sizeof(MeshIndex));

  return true;
}

}  // namespace core
}  // namespace mondradiko
