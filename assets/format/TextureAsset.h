/**
 * @file TextureAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

namespace mondradiko {
namespace assets {

enum class TextureFormat : uint8_t { None = 0, KTX };

struct TextureHeader {
  TextureFormat format;
  uint32_t data_length;
};

}  // namespace assets
}  // namespace mondradiko
