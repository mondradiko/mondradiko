/**
 * @file tinygltf_headers.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Conveniently #includes all library headers with proper #defines.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STBI_MSC_SECURE_CRT
#include <lib/third_party/tiny_gltf.h>
