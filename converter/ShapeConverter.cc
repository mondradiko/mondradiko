// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/ShapeConverter.h"

#include "converter/BundlerInterface.h"
#include "log/log.h"

namespace mondradiko {
namespace converter {

ShapeConverter::AssetOffset ShapeConverter::convert(
    AssetBuilder* fbb, const toml::table& asset) const {
  assets::ShapeAssetBuilder shape(*fbb);

  if (asset.find("sphere") != asset.end()) {
    const auto& sphere = asset.at("sphere").as_table();

    assets::SphereShape sphere_shape(sphere.at("radius").as_floating());

    shape.add_type(assets::ShapeType::Sphere);
    shape.add_sphere(&sphere_shape);
  } else {
    log_ftl("Asset has no shape table");
  }

  auto shape_offset = shape.Finish();

  assets::SerializedAssetBuilder serialized_asset(*fbb);
  serialized_asset.add_type(assets::AssetType::ShapeAsset);
  serialized_asset.add_shape(shape_offset);
  auto asset_offset = serialized_asset.Finish();

  return asset_offset;
}

}  // namespace converter
}  // namespace mondradiko
