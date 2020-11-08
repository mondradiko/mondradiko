/**
 * @file MaterialAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains textures and settings for a mesh material.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_ASSETS_MATERIALASSET_H_
#define SRC_ASSETS_MATERIALASSET_H_

#include <string>

#include "assets/Asset.h"
#include "assets/AssetHandle.h"
#include "assets/TextureAsset.h"
#include "src/api_headers.h"

namespace mondradiko {

class MeshPipeline;  // Forward declaration because of codependence

struct MaterialUniform {
  uint32_t albedo_texture;
};

class MaterialAsset : public Asset {
 public:
  MaterialAsset(MeshPipeline*, std::string, const aiScene*, aiMaterial*);
  ~MaterialAsset();

  void updateUniform(MaterialUniform*);

  AssetHandle<TextureAsset> albedo_texture;

  /**
   * @brief Stores the index into the CURRENT FRAME's material array.
   * 
   */
  uint32_t index = 0;

 private:
  MeshPipeline* mesh_pipeline;
};

}  // namespace mondradiko

#endif  // SRC_ASSETS_MATERIALASSET_H_
