/**
 * @file MeshAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/MeshAsset.h"

#include <vector>

#include "assets/format/MeshAsset_generated.h"
#include "core/assets/Asset.h"
#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

MeshAsset::MeshAsset(AssetPool*, GpuInstance* gpu) : gpu(gpu) {}

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

void MeshAsset::unload() {
  if (vertex_buffer != nullptr) delete vertex_buffer;
  if (index_buffer != nullptr) delete index_buffer;

  vertex_buffer = nullptr;
  index_buffer = nullptr;
}

}  // namespace mondradiko
