/**
 * @file bundler_main.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Bundles a directory and its contents into a loadable
 * AssetBundle.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/saving/AssetBundleBuilder.h"

#include <cstring>

using namespace mondradiko;  // NOLINT using is ok because this is an entrypoint

int main(int argc, const char* argv[]) {
  assets::AssetBundleBuilder bundle_builder("../test-folder/");

  const char test_asset[] = "Despite all known laws of aviation...";
  bundle_builder.addAsset(0xdeadbeef, test_asset, strlen(test_asset));

  bundle_builder.buildBundle("registry.bin");

  return 0;
}
