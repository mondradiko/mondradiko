/**
 * @file library_implementation.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements header-only libraries.
 * @date 2020-10-26
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
