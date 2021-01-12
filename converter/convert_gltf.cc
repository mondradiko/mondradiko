/**
 * @file convert_gltf.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Converts a glTF model into a prefab.
 * @date 2021-01-08
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "converter/convert_gltf.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <lib/tiny_gltf.h>

#include <string>
#include <vector>

#include "assets/format/MaterialAsset_generated.h"
#include "assets/format/MeshAsset_generated.h"
#include "converter/stb_converter.h"
#include "glm/gtc/type_ptr.hpp"
#include "log/log.h"

namespace mondradiko {

assets::AssetId load_dummy_texture(assets::AssetBundleBuilder *builder) {
  // TODO(marceline-cramer) Replace Assimp
  std::filesystem::path albedo_path = "../test-folder/Default_albedo.jpg";
  log_inf("Loading albedo texture from %s", albedo_path.c_str());

  return stb_convert(builder, albedo_path);
}

void load_dummy_material(assets::AssetBundleBuilder *builder) {
  flatbuffers::FlatBufferBuilder fbb;

  assets::MaterialAssetBuilder material_builder(fbb);
  assets::Vec3 albedo_factor(1.0, 1.0, 1.0);
  material_builder.add_albedo_factor(&albedo_factor);
  material_builder.add_albedo_texture(load_dummy_texture(builder));
  auto material_offset = material_builder.Finish();

  assets::SerializedAssetBuilder asset_builder(fbb);
  asset_builder.add_type(assets::AssetType::MaterialAsset);
  asset_builder.add_material(material_offset);
  auto asset_offset = asset_builder.Finish();

  assets::AssetId asset_id;
  builder->addAsset(&asset_id, &fbb, asset_offset);
  log_dbg("Added material asset: 0x%0lx", asset_id);
}

/**
 * @brief Helper class for reading GLTF buffers.
 *
 */
class GltfAccessor {
 public:
  GltfAccessor(const tinygltf::Model &model, uint32_t accessor_index) {
    const tinygltf::Accessor &accessor = model.accessors[accessor_index];
    const tinygltf::BufferView &buffer_view =
        model.bufferViews[accessor.bufferView];
    buffer_data = &(model.buffers[buffer_view.buffer]
                        .data[accessor.byteOffset + buffer_view.byteOffset]);
    stride = accessor.ByteStride(buffer_view);

    if (stride == -1) {
      log_ftl("Invalid GLTF accessor");
    }

    count = accessor.count;
  }

  template <typename ElementType>
  const ElementType *get(uint32_t element_index) {
    return reinterpret_cast<const ElementType *>(buffer_data +
                                                 stride * element_index);
  }

  uint32_t size() { return count; }

 private:
  const unsigned char *buffer_data;
  uint32_t count;
  uint32_t stride;
};

assets::AssetId load_node(assets::AssetBundleBuilder *builder,
                          const tinygltf::Model &model,
                          const tinygltf::Scene &scene,
                          const tinygltf::Node &node) {
  // TODO(marceline-cramer) Load subnodes

  const tinygltf::Mesh &mesh = model.meshes[node.mesh];

  std::vector<assets::MeshVertex> vertices;
  std::vector<uint32_t> indices;

  // TODO(marceline-cramer) Load all mesh primitives
  uint32_t primitive_index = 0;
  const tinygltf::Primitive &primitive = mesh.primitives[primitive_index];
  const auto &attributes = primitive.attributes;

  if (attributes.find("POSITION") == primitive.attributes.end()) {
    log_ftl("GLTF primitive must have position attributes");
  }

  if (attributes.find("NORMAL") == primitive.attributes.end()) {
    log_ftl("GLTF primitive must have normal attributes");
  }

  if (attributes.find("TEXCOORD_0") == primitive.attributes.end()) {
    log_ftl("GLTF primitive must have texture coordinates");
  }

  // TODO(marceline-cramer) Generate indices if they're  not there
  if (primitive.indices <= -1) {
    log_ftl("GLTF primtive must have indices");
  }

  {
    log_inf("Loading vertices");

    GltfAccessor pos_accessor(model, attributes.find("POSITION")->second);
    GltfAccessor norm_accessor(model, attributes.find("NORMAL")->second);
    GltfAccessor tex_accessor(model, attributes.find("TEXCOORD_0")->second);

    for (size_t v = 0; v < pos_accessor.size(); v++) {
      const float *position = pos_accessor.get<float>(v);
      const float *normal_raw = norm_accessor.get<float>(v);
      const float *tex_coord = tex_accessor.get<float>(v);

      // Normalize the normal
      glm::vec3 normal = glm::normalize(
          glm::vec3(normal_raw[0], normal_raw[1], normal_raw[2]));

      assets::Vec3 position_vec(position[0], position[1], position[2]);
      assets::Vec3 normal_vec(normal.x, normal.y, normal.z);
      // TODO(marceline-cramer) Read mesh vertex colors
      assets::Vec3 color_vec(1.0, 1.0, 1.0);
      assets::Vec2 tex_coord_vec(tex_coord[0], tex_coord[1]);

      assets::MeshVertex vertex(position_vec, normal_vec, color_vec,
                                tex_coord_vec);
      vertices.push_back(vertex);
    }
  }

  {
    log_inf("Loading indices");

    const tinygltf::Accessor &accessor =
        model.accessors[primitive.indices > -1 ? primitive.indices : 0];
    const tinygltf::BufferView &bufferView =
        model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

    const void *dataPtr =
        &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

    switch (accessor.componentType) {
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
        const uint32_t *buf = static_cast<const uint32_t *>(dataPtr);
        for (size_t index = 0; index < accessor.count; index++) {
          indices.push_back(buf[index]);
        }
        break;
      }
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
        const uint16_t *buf = static_cast<const uint16_t *>(dataPtr);
        for (size_t index = 0; index < accessor.count; index++) {
          indices.push_back(buf[index]);
        }
        break;
      }
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
        const uint8_t *buf = static_cast<const uint8_t *>(dataPtr);
        for (size_t index = 0; index < accessor.count; index++) {
          indices.push_back(buf[index]);
        }
        break;
      }
      default: {
        log_ftl("Index component type %s", accessor.componentType);
        break;
      }
    }
  }

  {
    log_inf("Writing asset data");

    flatbuffers::FlatBufferBuilder fbb;

    auto vertices_offset = fbb.CreateVectorOfStructs(vertices);
    auto indices_offset = fbb.CreateVector(indices);

    assets::MeshAssetBuilder mesh_asset(fbb);
    mesh_asset.add_vertices(vertices_offset);
    mesh_asset.add_indices(indices_offset);
    auto mesh_offset = mesh_asset.Finish();

    assets::SerializedAssetBuilder asset(fbb);
    asset.add_type(assets::AssetType::MeshAsset);
    asset.add_mesh(mesh_offset);
    auto asset_offset = asset.Finish();

    assets::AssetId asset_id;
    builder->addAsset(&asset_id, &fbb, asset_offset);
    log_dbg("Added mesh asset 0x%0lx", asset_id);
    return asset_id;
  }
}

assets::AssetId convert_gltf(assets::AssetBundleBuilder *builder,
                             const std::filesystem::path &gltf_path) {
  tinygltf::Model gltf_model;
  tinygltf::TinyGLTF gltf_context;
  std::string error;
  std::string warning;

  std::string filename = gltf_path;
  bool binary = false;
  size_t extpos = filename.rfind('.', filename.length());
  if (extpos != std::string::npos) {
    binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
  }

  bool file_loaded = false;

  if (binary) {
    file_loaded = gltf_context.LoadBinaryFromFile(&gltf_model, &error, &warning,
                                                  filename.c_str());
  } else {
    file_loaded = gltf_context.LoadASCIIFromFile(&gltf_model, &error, &warning,
                                                 filename.c_str());
  }

  if (!file_loaded) {
    log_ftl("Failed to load model with error: %s", error.c_str());
  }

  if (warning.length() > 0) {
    log_wrn("GLTF warning: %s", warning.c_str());
  }

  // TODO(marceline-cramer) Load correct materials
  { load_dummy_material(builder); }

  // TODO(marceline-cramer) Handle all scenes in separate prefabs
  uint32_t scene_index =
      gltf_model.defaultScene > -1 ? gltf_model.defaultScene : 0;
  const tinygltf::Scene &scene = gltf_model.scenes[scene_index];

  for (uint32_t i = 0; i < scene.nodes.size(); i++) {
    const tinygltf::Node &node = gltf_model.nodes[scene.nodes[i]];
    // TODO(marceline-cramer) Load everything else
    if (node.mesh > -1) {
      return load_node(builder, gltf_model, scene, node);
    }
  }

  // TODO(marceline-cramer) Load everything
  log_err("Couldn't find any mesh nodes");
  return assets::AssetId::NullAsset;
}

}  // namespace mondradiko
