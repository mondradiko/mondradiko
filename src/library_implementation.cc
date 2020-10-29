/**
 * @file library_implementation.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements header-only libraries.
 * @date 2020-10-26
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
 * along with Mondradiko.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#define VMA_IMPLEMENTATION
#include "lib/vk_mem_alloc.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
