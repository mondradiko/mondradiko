// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/PrefabBuilder.h"

#include <string>

#include "converter/BundlerInterface.h"
#include "types/assets/PrefabAsset_generated.h"

namespace mondradiko {

assets::Vec3 getVec3(const toml::value& src_array) {
  auto src = src_array.as_array();
  assets::Vec3 dst;
  dst.mutate_x(src[0].as_floating());
  dst.mutate_y(src[1].as_floating());
  dst.mutate_z(src[2].as_floating());
  return dst;
}

assets::AssetId PrefabBuilder::buildPrefab(BundlerInterface* bundler,
                                           const toml::table& prefab) {
  ConverterInterface::AssetBuilder fbb;

  // ScriptPrefab is a table, so it needs to be built before PrefabAsset
  flatbuffers::Offset<assets::ScriptPrefab> script_offset;
  if (prefab.find("script") != prefab.end()) {
    const auto& script = prefab.at("script").as_table();
    std::string script_alias = script.at("script_asset").as_string();
    assets::AssetId script_id = bundler->getAssetByAlias(script_alias);

    assets::ScriptPrefabBuilder script_prefab(fbb);
    script_prefab.add_script(script_id);
    script_offset = script_prefab.Finish();
  }

  assets::PrefabAssetBuilder prefab_builder(fbb);
  prefab_builder.add_script(script_offset);

  if (prefab.find("point_light") != prefab.end()) {
    auto point_light_table = prefab.at("point_light").as_table();

    assets::Vec3 position = getVec3(point_light_table.at("position"));
    assets::Vec3 intensity = getVec3(point_light_table.at("intensity"));

    assets::PointLightPrefab point_light(position, intensity);
    prefab_builder.add_point_light(&point_light);
  }

  if (prefab.find("transform") != prefab.end()) {
    auto transform_table = prefab.at("transform").as_table();
    assets::Vec3 position = getVec3(transform_table.at("position"));

    // TODO(marceline-cramer) Load this from the TOML
    assets::Quaternion orientation(1.0, 0.0, 0.0, 0.0);

    assets::TransformPrefab transform_builder(position, orientation);
    prefab_builder.add_transform(&transform_builder);
  }

  auto prefab_offset = prefab_builder.Finish();

  assets::SerializedAssetBuilder asset(fbb);
  asset.add_type(assets::AssetType::PrefabAsset);
  asset.add_prefab(prefab_offset);
  auto asset_offset = asset.Finish();

  return bundler->addAsset(&fbb, asset_offset);
}

}  // namespace mondradiko
