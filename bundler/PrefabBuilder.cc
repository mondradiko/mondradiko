// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bundler/PrefabBuilder.h"

#include <string>

#include "bundler/Bundler.h"
#include "types/assets/PrefabAsset_generated.h"

namespace mondradiko {

assets::AssetId PrefabBuilder::buildPrefab(Bundler* bundler,
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
    // TODO(marceline-cramer) Load these from the TOML
    assets::Vec3 position(0.0, 0.0, 0.0);
    assets::Vec3 intensity(100.0, 0.0, 100.0);

    assets::PointLightPrefab point_light(position, intensity);
    prefab_builder.add_point_light(&point_light);
  }

  if (prefab.find("transform") != prefab.end()) {
    // TODO(marceline-cramer) Load these from the TOML
    assets::Vec3 position(0.0, 0.0, 0.0);
    assets::Quaternion orientation(1.0, 0.0, 0.0, 0.0);

    assets::TransformPrefab transform(position, orientation);
    prefab_builder.add_transform(&transform);
  }

  auto prefab_offset = prefab_builder.Finish();

  assets::SerializedAssetBuilder asset(fbb);
  asset.add_type(assets::AssetType::PrefabAsset);
  asset.add_prefab(prefab_offset);
  auto asset_offset = asset.Finish();

  return bundler->addAsset(&fbb, asset_offset);
}

}  // namespace mondradiko
