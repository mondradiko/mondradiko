/**
 * @file Bundler.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Bundles a directory and its contents into a loadable
 * AssetBundle.
 * @date 2021-01-22
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <filesystem>

#include "lib/toml.hpp"

namespace mondradiko {

class Bundler {
 public:
  Bundler(const std::filesystem::path&);
  ~Bundler();

  void bundle();

 private:
  std::filesystem::path manifest_path;
  std::filesystem::path source_root;
  std::filesystem::path bundle_root;

  toml::value manifest;
};

}  // namespace mondradiko
