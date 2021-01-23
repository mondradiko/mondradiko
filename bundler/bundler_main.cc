/**
 * @file bundler_main.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Entrypoint for the bundler build system.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include <cstring>
#include <iostream>

#include "bundler/Bundler.h"
#include "log/log.h"

using namespace mondradiko;  // NOLINT using is ok because this is an entrypoint

void print_usage(const char* arg1) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "  " << arg1 << " bundler-manifest.toml" << std::endl;
}

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return 1;
  }

  const char* manifest_file = argv[1];

  try {
    Bundler bundler(manifest_file);
    bundler.bundle();
  } catch (const std::exception& e) {
    log_err("Mondradiko bundler failed with message:");
    log_err(e.what());
    return 1;
  }

  return 0;
}
