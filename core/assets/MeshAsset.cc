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

#include "assets/format/MeshAsset.h"
#include "core/assets/Asset.h"
#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

MeshAsset::MeshAsset(assets::ImmutableAsset& asset, AssetPool*,
                     GpuInstance* gpu) {
  assets::MeshHeader header;
  asset >> header;

  std::vector<MeshVertex> vertices(header.vertex_count);
  std::vector<MeshIndex> indices(header.index_count);

  for (uint32_t i = 0; i < vertices.size(); i++) {
    assets::MeshVertex vertex;
    asset >> vertex;

    vertices[i].position = vertex.position;
    vertices[i].tex_coord = vertex.tex_coord;
    vertices[i].color = vertex.color;
    vertices[i].normal = vertex.normal;
  }

  for (uint32_t i = 0; i < indices.size(); i++) {
    assets::MeshIndex index;
    asset >> index;

    indices[i] = index;
  }

  size_t vertex_size = sizeof(MeshVertex) * vertices.size();
  vertex_buffer =
      new GpuBuffer(gpu, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  vertex_buffer->writeData(vertices.data());

  size_t index_size = sizeof(indices[0]) * indices.size();
  index_buffer =
      new GpuBuffer(gpu, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  index_buffer->writeData(indices.data());

  index_count = header.index_count;
}

MeshAsset::~MeshAsset() {
  if (vertex_buffer != nullptr) delete vertex_buffer;
  if (index_buffer != nullptr) delete index_buffer;
}

}  // namespace mondradiko
