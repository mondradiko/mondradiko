// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <map>

namespace mondradiko {
namespace types {

template <typename K, typename V>
using map = std::map<K, V>;

}  // namespace types
}  // namespace mondradiko
