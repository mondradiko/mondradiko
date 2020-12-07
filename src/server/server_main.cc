/**
 * @file server_main.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Entrypoint for a server.
 * @date 2020-12-06
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * 
 */

#include <iostream>

#include "common/api_headers.h"
#include "log/log.h"

// The using statement is fine because
// this is the main entrypoint
using namespace mondradiko;  // NOLINT

bool g_interrupted = false;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
  g_interrupted = true;

  return;
}

int main(int argc, const char* argv[]) {
  TracyMessageL("Hello Tracy!");
  log_inf("Hello internet!");

  return 0;
}
