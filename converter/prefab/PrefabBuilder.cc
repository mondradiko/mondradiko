// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/prefab/PrefabBuilder.h"

#include <string>
#include <vector>

#include "converter/BundlerInterface.h"
#include "types/assets/PrefabAsset_generated.h"

namespace mondradiko {
namespace converter {

PrefabBuilder::PrefabBuilder(BundlerInterface* bundler) : bundler(bundler) {}

assets::Vec3 getVec3(const toml::value& src_array) {
  auto src = src_array.as_array();
  assets::Vec3 dst;

  auto v = dst.mutable_v();
  v->Mutate(0, src[0].as_floating());
  v->Mutate(1, src[1].as_floating());
  v->Mutate(2, src[2].as_floating());

  return dst;
}

assets::Quaternion getQuaternion() {
  assets::Quaternion dst;
  assets::GlmToQuat(&dst, glm::quat());
  return dst;
}

ConverterInterface::AssetOffset PrefabBuilder::convert(
    AssetBuilder* fbb, const toml::table& prefab) const {
  std::vector<uint32_t> children;

  auto children_iter = prefab.find("children");
  if (children_iter != prefab.end()) {
    auto children_array = children_iter->second.as_array();
    for (auto child : children_array) {
      std::string child_alias = child.as_string();
      assets::AssetId child_id = bundler->getAssetByAlias(child_alias);
      children.push_back(static_cast<uint32_t>(child_id));
    }
  }

  auto children_offset = fbb->CreateVector(children);

  // ScriptPrefab is a table, so it needs to be built before PrefabAsset
  flatbuffers::Offset<assets::ScriptPrefab> script_offset;
  if (prefab.find("script") != prefab.end()) {
    const auto& script = prefab.at("script").as_table();
    std::string script_alias = script.at("script_asset").as_string();
    assets::AssetId script_id = bundler->getAssetByAlias(script_alias);

    std::string script_impl;
    if (script.find("script_impl") != script.end()) {
      script_impl = script.at("script_impl").as_string();
    } else {
      script_impl = script_alias;
    }

    auto impl_offset = fbb->CreateString(script_impl);

    assets::ScriptPrefabBuilder script_prefab(*fbb);
    script_prefab.add_script_asset(script_id);
    script_prefab.add_script_impl(impl_offset);
    script_offset = script_prefab.Finish();
  }

  assets::PrefabAssetBuilder prefab_builder(*fbb);
  prefab_builder.add_children(children_offset);
  prefab_builder.add_script(script_offset);

  if (prefab.find("point_light") != prefab.end()) {
    auto point_light_table = prefab.at("point_light").as_table();

    assets::Vec3 position = getVec3(point_light_table.at("position"));
    assets::Vec3 intensity = getVec3(point_light_table.at("intensity"));

    assets::PointLightPrefab point_light(position, intensity);
    prefab_builder.add_point_light(&point_light);
  }

  if (prefab.find("rigid_body") != prefab.end()) {
    auto rigid_body_table = prefab.at("rigid_body").as_table();

    float mass = rigid_body_table.at("mass").as_floating();

    assets::RigidBodyPrefab rigid_body(mass);
    prefab_builder.add_rigid_body(&rigid_body);
  }

  if (prefab.find("transform") != prefab.end()) {
    auto transform_table = prefab.at("transform").as_table();
    assets::Vec3 position = getVec3(transform_table.at("position"));

    // TODO(marceline-cramer) Load this from the TOML
    assets::Quaternion orientation = getQuaternion();

    assets::TransformPrefab transform_builder(position, orientation);
    prefab_builder.add_transform(&transform_builder);
  }

  auto prefab_offset = prefab_builder.Finish();

  assets::SerializedAssetBuilder asset(*fbb);
  asset.add_type(assets::AssetType::PrefabAsset);
  asset.add_prefab(prefab_offset);
  return asset.Finish();
}

}  // namespace converter
}  // namespace mondradiko
