// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/MeshAsset.h"

#include <vector>

#include "core/assets/Asset.h"
#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"
#include "types/assets/MeshAsset_generated.h"

namespace mondradiko {

void MeshAsset::load(const assets::SerializedAsset* asset) {
  const assets::MeshAsset* mesh = asset->mesh();

  std::vector<MeshVertex> vertices(mesh->vertices()->size());
  std::vector<MeshIndex> indices(mesh->indices()->size());

  for (uint32_t i = 0; i < vertices.size(); i++) {
    const assets::MeshVertex* vertex = mesh->vertices()->Get(i);

    vertices[i].position = assets::Vec3ToGlm(vertex->position());
    vertices[i].tex_coord = assets::Vec2ToGlm(vertex->tex_coord());
    vertices[i].color = assets::Vec3ToGlm(vertex->color());
    vertices[i].normal = assets::Vec3ToGlm(vertex->normal());
    vertices[i].tangent = assets::Vec3ToGlm(vertex->tangent());
  }

  for (uint32_t i = 0; i < indices.size(); i++) {
    indices[i] = mesh->indices()->Get(i);
  }

  size_t vertex_size = sizeof(MeshVertex) * vertices.size();
  vertex_buffer =
      new GpuBuffer(gpu, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  vertex_buffer->writeData(vertices.data());

  size_t index_size = sizeof(indices[0]) * indices.size();
  index_buffer =
      new GpuBuffer(gpu, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  index_buffer->writeData(indices.data());

  index_count = indices.size();
}

MeshAsset::~MeshAsset() {
  if (vertex_buffer != nullptr) delete vertex_buffer;
  if (index_buffer != nullptr) delete index_buffer;
}

}  // namespace mondradiko
