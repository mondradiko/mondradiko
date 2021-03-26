// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <unordered_map>

namespace mondradiko {
namespace types {

template <typename K, typename V>
using unordered_map = std::unordered_map<K, V>;

}  // namespace types
}  // namespace mondradiko
