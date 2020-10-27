/**
 * @file api_headers.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Conveniently #includes all library headers with proper #defines.
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

#ifndef SRC_API_HEADERS_H_
#define SRC_API_HEADERS_H_

#include "vulkan/vulkan.h"

#define XR_USE_GRAPHICS_API_VULKAN
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

#include "shaderc/shaderc.hpp"

#include "lib/vk_mem_alloc.h"

#include "assimp/DefaultLogger.hpp"
#include "assimp/Importer.hpp"
#include "assimp/IOStream.hpp"
#include "assimp/IOSystem.hpp"
#include "assimp/Logger.hpp"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/texture.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "physfs.h"  // NOLINT (PhysFS is not in an include directory)

#endif  // SRC_API_HEADERS_H_
