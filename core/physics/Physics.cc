// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/physics/Physics.h"

#include "core/world/World.h"
#include "log/log.h"

namespace mondradiko {

Physics::Physics(World* world) : world(world) {
  log_zone;

  broadphase = new btDbvtBroadphase();
  collision_configuration = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collision_configuration);
  solver = new btSequentialImpulseConstraintSolver;

  dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver,
                                               collision_configuration);
  dynamics_world->setGravity(btVector3(0, -9.8, 0));

  log_inf_fmt("Bullet Physics: %.2f %s precision", 0.01f * btGetVersion(),
              btIsDoublePrecision() ? "double" : "single");
}

Physics::~Physics() {
  log_zone;

  if (dynamics_world != nullptr) delete dynamics_world;
  if (solver != nullptr) delete solver;
  if (broadphase != nullptr) delete broadphase;
  if (dispatcher != nullptr) delete dispatcher;
  if (collision_configuration != nullptr) delete collision_configuration;
}

void Physics::update(double dt) {
  log_zone;

  dynamics_world->stepSimulation(dt);
}

}  // namespace mondradiko
