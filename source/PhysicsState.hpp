#pragma once

#include <LinearMath/btAlignedObjectArray.h>
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <memory>

namespace DEV {

struct PhysicsState
{
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btDbvtBroadphase> broadphase;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btCollisionShape> plane;
	std::unique_ptr<btCollisionShape> sphere;
	btRigidBody* bodies;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
};

} // namespace DEV
