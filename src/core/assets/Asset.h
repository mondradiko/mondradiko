/**
 * @file Asset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements a base class for Assets loaded from disk.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

namespace mondradiko {

class Asset {
 public:
  virtual ~Asset() {}
 private:
  template<class T>
  friend class AssetHandle;

  int ref_count = 0;
};

}  // namespace mondradiko
