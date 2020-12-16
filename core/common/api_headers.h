/**
 * @file api_headers.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Conveniently #includes all library headers with proper #defines.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "vulkan/vulkan.h"

#define XR_USE_GRAPHICS_API_VULKAN
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

#include "lib/vk_mem_alloc.h"
#include "lib/stb_image.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

#include "lib/entt.hpp"
