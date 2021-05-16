// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/world/Entity.h"
#include "lib/include/bullet_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class AssetPool;
class World;

class Physics {
 public:
  explicit Physics(World*);
  ~Physics();

  void initializeAssets(AssetPool*);

  void update(double);

 private:
  World* world;

  btBroadphaseInterface* broadphase = nullptr;
  btDefaultCollisionConfiguration* collision_configuration = nullptr;
  btCollisionDispatcher* dispatcher = nullptr;
  btSequentialImpulseConstraintSolver* solver = nullptr;
  btDiscreteDynamicsWorld* dynamics_world = nullptr;

  btCollisionShape* default_shape = nullptr;

  // Observer to clean up RigidBodyComponents
  static void onRigidBodyComponentDestroy(EntityRegistry&, EntityId);
};

}  // namespace core
}  // namespace mondradiko
