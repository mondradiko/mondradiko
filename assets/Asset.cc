/**
 * @file Asset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores the raw Asset data found in a lump.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/Asset.h"

namespace mondradiko {
namespace assets {

AssetId Asset::generateId(AssetHashMethod) { return 0; }

}  // namespace assets
}  // namespace mondradiko
