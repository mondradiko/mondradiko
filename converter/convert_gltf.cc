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

#include <string>
#include <vector>

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <lib/tiny_gltf.h>

#include "assets/format/MaterialAsset.h"
#include "assets/format/MeshAsset.h"
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
  assets::MutableAsset asset;

  assets::MaterialHeader header;
  header.albedo_factor = glm::vec4(1.0);
  header.albedo_texture = load_dummy_texture(builder);
  asset << header;

  assets::AssetId asset_id;
  builder->addAsset(&asset_id, &asset);
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
  assets::MutableAsset mesh_asset;

  // TODO(marceline-cramer) Load subnodes

  const tinygltf::Mesh &mesh = model.meshes[node.mesh];

  std::vector<assets::MeshVertex> vertices;
  std::vector<assets::MeshIndex> indices;

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
      assets::MeshVertex vertex;
      vertex.position = glm::make_vec3(pos_accessor.get<float>(v));
      vertex.normal =
          glm::normalize(glm::make_vec3(norm_accessor.get<float>(v)));
      vertex.color = glm::vec3(1.0);
      vertex.tex_coord = glm::make_vec3(tex_accessor.get<float>(v));

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

  assets::MeshHeader header;
  header.vertex_count = vertices.size();
  header.index_count = indices.size();
  mesh_asset << header;

  mesh_asset.writeData(reinterpret_cast<const char *>(vertices.data()),
                       vertices.size() * sizeof(assets::MeshVertex));
  mesh_asset.writeData(reinterpret_cast<const char *>(indices.data()),
                       indices.size() * sizeof(assets::MeshIndex));

  assets::AssetId asset_id;
  builder->addAsset(&asset_id, &mesh_asset);
  log_dbg("Added mesh asset 0x%0lx", asset_id);
  return asset_id;
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
  log_ftl("Couldn't find any mesh nodes");
}

}  // namespace mondradiko
