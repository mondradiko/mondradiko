/**
 * @file MeshRendererComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all data required to render a mesh (geometry and material).
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_COMPONENTS_MESHRENDERERCOMPONENT_H_
#define SRC_COMPONENTS_MESHRENDERERCOMPONENT_H_

#include "assets/AssetHandle.h"
#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "components/Component.h"
#include "src/api_headers.h"

class MeshPipeline; // Forward declaration because of codependence

class MeshRendererComponent : public Component {
 public:
  MeshRendererComponent(MeshPipeline*, AssetHandle<MeshAsset>&,
                        AssetHandle<MaterialAsset>&);
  ~MeshRendererComponent();

  AssetHandle<MeshAsset> meshAsset;
  AssetHandle<MaterialAsset> materialAsset;

 private:
  MeshPipeline* meshPipeline;
};

#endif  // SRC_COMPONENTS_MESHRENDERERCOMPONENT_H_
