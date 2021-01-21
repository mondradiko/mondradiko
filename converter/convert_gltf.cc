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

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "assets/format/MaterialAsset_generated.h"
#include "assets/format/MeshAsset_generated.h"
#include "assets/format/PrefabAsset_generated.h"
#include "converter/stb_converter.h"
#include "log/log.h"

namespace mondradiko {

assets::AssetId load_dummy_texture(assets::AssetBundleBuilder *builder) {
  std::filesystem::path albedo_path = "../test-folder/Default_albedo.jpg";
  log_inf("Loading albedo texture from %s", albedo_path.c_str());

  return stb_convert(builder, albedo_path);
}

assets::AssetId load_image(assets::AssetBundleBuilder *builder,
                           const tinygltf::Model &model,
                           const tinygltf::Image &image) {
  flatbuffers::FlatBufferBuilder fbb;

  log_inf("Loading GLTF image");
  log_inf("Name:\t\t\"%s\"", image.name.c_str());
  log_inf("Component#:\t%d", image.component);
  log_inf("Bits/channel:\t%d", image.bits);
  log_inf("Size:\t\t%dx%d", image.width, image.height);

  auto data_offset =
      fbb.CreateVector(reinterpret_cast<const uint8_t *>(image.image.data()),
                       image.image.size());

  assets::RawTextureBuilder raw_texture(fbb);

  raw_texture.add_components(image.component);
  raw_texture.add_bit_depth(image.bits);

  {
    using Components = assets::TextureComponentType;
    std::unordered_map<int, assets::TextureComponentType> types;
    types.emplace(TINYGLTF_COMPONENT_TYPE_BYTE, Components::Byte);
    types.emplace(TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, Components::UByte);
    types.emplace(TINYGLTF_COMPONENT_TYPE_SHORT, Components::Short);
    types.emplace(TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, Components::UShort);
    types.emplace(TINYGLTF_COMPONENT_TYPE_INT, Components::Int);
    types.emplace(TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, Components::UInt);
    types.emplace(TINYGLTF_COMPONENT_TYPE_FLOAT, Components::Float);
    types.emplace(TINYGLTF_COMPONENT_TYPE_DOUBLE, Components::Double);

    auto iter = types.find(image.pixel_type);

    if (iter == types.end()) {
      log_ftl("Unrecognized GLTF image component type %d", image.component);
    }

    raw_texture.add_component_type(iter->second);
  }

  raw_texture.add_width(image.width);
  raw_texture.add_height(image.height);
  raw_texture.add_data(data_offset);
  auto raw_texture_offset = raw_texture.Finish();

  assets::TextureAssetBuilder texture(fbb);
  texture.add_format(assets::TextureFormat::Raw);
  texture.add_raw(raw_texture_offset);
  auto texture_offset = texture.Finish();

  assets::SerializedAssetBuilder asset(fbb);
  asset.add_type(assets::AssetType::TextureAsset);
  asset.add_texture(texture_offset);
  auto asset_offset = asset.Finish();

  assets::AssetId asset_id;
  builder->addAsset(&asset_id, &fbb, asset_offset);
  log_dbg("Added GLTF image: 0x%0lx", asset_id);
  return asset_id;
}

assets::AssetId load_texture(assets::AssetBundleBuilder *builder,
                             const tinygltf::Model &model,
                             const tinygltf::TextureInfo &texture_info) {
  const tinygltf::Texture &texture = model.textures[texture_info.index];
  const tinygltf::Image &image = model.images[texture.source];

  // TODO(marceline-cramer) Add sampler support

  return load_image(builder, model, image);
}

assets::AssetId load_material(assets::AssetBundleBuilder *builder,
                              const tinygltf::Model &model,
                              const tinygltf::Material &material) {
  flatbuffers::FlatBufferBuilder fbb;

  assets::MaterialAssetBuilder material_builder(fbb);

  assets::Vec3 albedo_factor(1.0, 1.0, 1.0);
  if (material.pbrMetallicRoughness.baseColorFactor.size() >= 3) {
    const auto &factor = material.pbrMetallicRoughness.baseColorFactor;
    albedo_factor.mutate_x(factor[0]);
    albedo_factor.mutate_y(factor[1]);
    albedo_factor.mutate_z(factor[2]);
  }
  material_builder.add_albedo_factor(&albedo_factor);

  const auto &base_color = material.pbrMetallicRoughness.baseColorTexture;
  assets::AssetId base_color_id = load_texture(builder, model, base_color);
  material_builder.add_albedo_texture(base_color_id);

  auto material_offset = material_builder.Finish();

  assets::SerializedAssetBuilder asset_builder(fbb);
  asset_builder.add_type(assets::AssetType::MaterialAsset);
  asset_builder.add_material(material_offset);
  auto asset_offset = asset_builder.Finish();

  assets::AssetId asset_id;
  builder->addAsset(&asset_id, &fbb, asset_offset);
  log_dbg("Added material asset: 0x%0lx", asset_id);
  return asset_id;
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

assets::AssetId load_primitive(assets::AssetBundleBuilder *builder,
                               const tinygltf::Model &model,
                               const tinygltf::Primitive &primitive) {
  std::vector<assets::MeshVertex> vertices;
  std::vector<uint32_t> indices;

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

  // TODO(marceline-cramer) Generate indices if they're not there
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
    log_inf("Writing primitive mesh data");

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

    assets::AssetId mesh_id;
    builder->addAsset(&mesh_id, &fbb, asset_offset);
    log_dbg("Added primitive mesh 0x%0lx", mesh_id);
    return mesh_id;
  }
}

assets::AssetId load_node(assets::AssetBundleBuilder *builder,
                          const tinygltf::Model &model,
                          const tinygltf::Node &node) {
  std::vector<uint32_t> children;

  glm::vec3 node_translation;
  glm::quat node_orientation;

  if (node.translation.size() == 3) {
    node_translation = glm::make_vec3(node.translation.data());
  } else {
    node_translation = glm::vec3(0.0, 0.0, 0.0);
  }

  if (node.rotation.size() == 4) {
    node_orientation = glm::make_quat(node.rotation.data());

    // GLTF stores quaternions as xyzw, so we need to swizzle
    auto temp = node_orientation.w;
    node_orientation.w = node_orientation.z;
    node_orientation.z = node_orientation.y;
    node_orientation.y = node_orientation.x;
    node_orientation.x = temp;
  } else {
    node_orientation = glm::quat(1.0, 0.0, 0.0, 0.0);
  }

  // TODO(marceline-cramer) Process glTF node scaling factor

  if (node.mesh != -1) {
    log_inf("Creating mesh");

    const auto &mesh = model.meshes[node.mesh];

    for (const auto &primitive : mesh.primitives) {
      assets::AssetId mesh_id = load_primitive(builder, model, primitive);

      const tinygltf::Material &material = model.materials[primitive.material];
      assets::AssetId material_id = load_material(builder, model, material);

      flatbuffers::FlatBufferBuilder fbb;

      assets::MeshRendererPrefabBuilder mesh_renderer(fbb);
      mesh_renderer.add_mesh(mesh_id);
      mesh_renderer.add_material(material_id);
      auto mesh_renderer_offset = mesh_renderer.Finish();

      assets::TransformPrefab transform;
      transform.mutable_position().mutate_x(node_translation.x);
      transform.mutable_position().mutate_y(node_translation.y);
      transform.mutable_position().mutate_z(node_translation.z);

      transform.mutable_orientation().mutate_w(node_orientation.w);
      transform.mutable_orientation().mutate_x(node_orientation.x);
      transform.mutable_orientation().mutate_y(node_orientation.y);
      transform.mutable_orientation().mutate_z(node_orientation.z);

      assets::PrefabAssetBuilder prefab(fbb);
      prefab.add_mesh_renderer(mesh_renderer_offset);
      prefab.add_transform(&transform);
      auto prefab_offset = prefab.Finish();

      assets::SerializedAssetBuilder asset(fbb);
      asset.add_type(assets::AssetType::PrefabAsset);
      asset.add_prefab(prefab_offset);
      auto asset_offset = asset.Finish();

      assets::AssetId asset_id;
      builder->addAsset(&asset_id, &fbb, asset_offset);
      log_dbg("Added primitive prefab 0x%0lx", mesh_id);

      children.push_back(static_cast<uint32_t>(asset_id));
    }
  }

  {
    log_inf("Creating childen");

    for (auto child_index : node.children) {
      const auto &child_node = model.nodes[child_index];
      assets::AssetId child_id = load_node(builder, model, child_node);
      children.push_back(static_cast<uint32_t>(child_id));
    }
  }

  {
    log_inf("Creating prefab");

    flatbuffers::FlatBufferBuilder fbb;

    auto children_offset = fbb.CreateVector(children);

    assets::PrefabAssetBuilder prefab_asset(fbb);
    prefab_asset.add_children(children_offset);
    auto prefab_offset = prefab_asset.Finish();

    assets::SerializedAssetBuilder asset(fbb);
    asset.add_type(assets::AssetType::PrefabAsset);
    asset.add_prefab(prefab_offset);
    auto asset_offset = asset.Finish();

    assets::AssetId prefab_id;
    builder->addAsset(&prefab_id, &fbb, asset_offset);
    log_dbg("Added node prefab 0x%0lx", prefab_id);
    return prefab_id;
  }
}

assets::AssetId load_scene(assets::AssetBundleBuilder *builder,
                           const tinygltf::Model &model,
                           const tinygltf::Scene &scene) {
  flatbuffers::FlatBufferBuilder fbb;

  std::vector<uint32_t> children;

  for (auto node_index : scene.nodes) {
    const tinygltf::Node &node = model.nodes[node_index];
    // TODO(marceline-cramer) Load everything else
    if (node.mesh > -1) {
      auto child_id = load_node(builder, model, node);
      children.push_back(static_cast<uint32_t>(child_id));
    }
  }

  auto children_offset = fbb.CreateVector(children);

  assets::PrefabAssetBuilder prefab_asset(fbb);
  prefab_asset.add_children(children_offset);
  auto prefab_offset = prefab_asset.Finish();

  assets::SerializedAssetBuilder asset(fbb);
  asset.add_type(assets::AssetType::PrefabAsset);
  asset.add_prefab(prefab_offset);
  auto asset_offset = asset.Finish();

  assets::AssetId prefab_id;
  builder->addAsset(&prefab_id, &fbb, asset_offset);
  log_dbg("Added GLTF scene prefab 0x%0lx", prefab_id);
  return prefab_id;
}

void convert_gltf(assets::AssetBundleBuilder *builder,
                  const std::filesystem::path &gltf_path) {
  tinygltf::Model gltf_model;
  tinygltf::TinyGLTF gltf_context;
  std::string error;
  std::string warning;

  std::string filename = gltf_path;
  bool binary = false;
  size_t extpos = filename.rfind('.', filename.length());
  if (extpos != std::string::npos) {
    auto extension = filename.substr(extpos + 1, filename.length() - extpos);
    binary = (extension == "glb") || (extension == "vrm");
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

  for (const auto &scene : gltf_model.scenes) {
    assets::AssetId initial_prefab = load_scene(builder, gltf_model, scene);
    builder->addInitialPrefab(initial_prefab);
    log_dbg("Added initial prefab asset 0x%0lx", initial_prefab);
  }
}

}  // namespace mondradiko
