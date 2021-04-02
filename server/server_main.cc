// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <chrono>  // NOLINT [build/c++11]
#include <csignal>
#include <iostream>
#include <memory>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "core/assets/AssetPool.h"
#include "core/cvars/CVarScope.h"
#include "core/cvars/FloatCVar.h"
#include "core/displays/SdlDisplay.h"
#include "core/filesystem/Filesystem.h"
#include "core/gpu/GpuInstance.h"
#include "core/network/NetworkServer.h"
#include "core/renderer/MeshPass.h"
#include "core/world/World.h"
#include "core/world/WorldEventSorter.h"
#include "log/log.h"
#include "types/build_config.h"

// The using statement is fine because
// this is the main entrypoint
using namespace mondradiko::core;  // NOLINT

struct ServerArgs {
  bool version = false;

  std::string server_ip = "127.0.0.1";
  int server_port = 10555;

  std::vector<std::string> bundle_paths = {"./"};

  std::string config_path = "./config.toml";

  int parse(int, const char* const[]);
};

int ServerArgs::parse(int argc, const char* const argv[]) {
  CLI::App app("Mondradiko server");

  app.add_flag("-v,--version", version, "Print version and exit");
  app.add_option("-s,--server", server_ip, "Domain server IP", true);
  app.add_option("-p,--port", server_port, "Domain server port", true);
  app.add_option("-b,--bundle", bundle_paths, "Paths to asset bundles", true);
  app.add_option("-c,--config", config_path, "Path to config file", true);

  CLI11_PARSE(app, argc, argv);
  return -1;
}

bool g_interrupted = false;

void run(const ServerArgs& args) {
  Filesystem fs;
  auto config = fs.loadToml(args.config_path);

  CVarScope cvars;

  CVarScope* server_cvars = cvars.addChild("server");
  server_cvars->addValue<FloatCVar>("max_tps", 1.0, 100.0);
  server_cvars->addValue<FloatCVar>("update_rate", 0.1, 20.0);

  cvars.loadConfig(config);

  for (auto bundle : args.bundle_paths) {
    fs.loadAssetBundle(bundle);
  }

  AssetPool asset_pool(&fs);
  MeshPass::initDummyAssets(&asset_pool);

  ScriptEnvironment scripts;
  World world(&asset_pool, &fs, &scripts);
  WorldEventSorter world_event_sorter(&world);
  NetworkServer server(&fs, &world_event_sorter, args.server_ip.c_str(),
                       args.server_port);

  world.initializePrefabs();

  const double min_frame_time = 1.0 / server_cvars->get<FloatCVar>("max_tps");
  const double min_update_time =
      1.0 / server_cvars->get<FloatCVar>("update_rate");

  using clock = std::chrono::high_resolution_clock;
  std::chrono::time_point last_frame = clock::now();
  std::chrono::time_point last_update = clock::now();

  while (!g_interrupted) {
    auto frame_start = clock::now();

    if (std::chrono::duration<double, std::chrono::seconds::period>(
            frame_start - last_update)
            .count() > min_update_time) {
      server.updateWorld();
      last_update = frame_start;
    }

    server.update();

    {
      log_zone_named("Lock tick rate");

      auto frame_wait =
          frame_start +
          std::chrono::duration<double, std::chrono::seconds::period>(
              min_frame_time);
      std::this_thread::sleep_until(frame_wait);
    }

    double dt = std::chrono::duration<double, std::chrono::seconds::period>(
                    frame_start - last_frame)
                    .count();
    if (!world.update(dt)) break;
    last_frame = frame_start;
  }
}

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
  g_interrupted = true;
  return;
}

int main(int argc, char* argv[]) {
  ServerArgs args;
  int parse_result = args.parse(argc, argv);
  if (parse_result != -1) return parse_result;

  log_msg_fmt("%s server version %s", MONDRADIKO_NAME, MONDRADIKO_VERSION);
  log_msg_fmt("%s", MONDRADIKO_COPYRIGHT);
  log_msg_fmt("%s", MONDRADIKO_LICENSE);

  if (args.version) return 0;

  if (signal(SIGTERM, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGTERM");
  }

  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGINT");
  }

  try {
    run(args);
  } catch (const std::exception& e) {
    log_err_fmt("Mondradiko server failed with message: %s", e.what());
    return 1;
  }

  return 0;
}
