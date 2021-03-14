// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/physics/Physics.h"

// TODO(humbletim) bullet integration testing
#include <bullet/btBulletDynamicsCommon.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btAlignedObjectArray.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

#include "log/log.h"

namespace mondradiko {

float Physics::get_version() { return 0.01f * btGetVersion(); }

Physics::Physics() {
  log_zone;

  // temporary boilerplate for testing bullet integration
  auto collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
  auto dispatcher = new btCollisionDispatcher(collisionConfiguration);
  auto broadphase = new btDbvtBroadphase();
  auto solver = new btSequentialImpulseConstraintSolver;
  auto dynamicsWorld = new btSoftRigidDynamicsWorld(
    dispatcher, broadphase, solver, collisionConfiguration);
  dynamicsWorld->setGravity(btVector3(0, -10, 0));
  btSoftBodyWorldInfo softBodyWorldInfo;
  softBodyWorldInfo.m_broadphase = broadphase;
  softBodyWorldInfo.m_dispatcher = dispatcher;
  softBodyWorldInfo.m_gravity = dynamicsWorld->getGravity();
  softBodyWorldInfo.m_sparsesdf.Initialize();
  log_msg_fmt("Bullet Physics: %.2f %s precision", 0.01f * btGetVersion(),
    btIsDoublePrecision() ? "double" : "single");
}

Physics::~Physics() { log_zone; }

}  // namespace mondradiko
