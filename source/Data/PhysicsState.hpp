#pragma once
#include <LinearMath/btAlignedObjectArray.h>
#include <btBulletDynamicsCommon.h>
#include <vector>
#include "owned_ptr.hpp"

namespace DEV {

struct PhysicsState
{
	struct bodies_deleter
	{
		void operator()(btRigidBody* ptr) const
		{
			btRigidBody::operator delete[] ((void*)ptr);
		}
	};

	owned_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	owned_ptr<btCollisionDispatcher> dispatcher;
	owned_ptr<btDbvtBroadphase> broadphase;
	owned_ptr<btSequentialImpulseConstraintSolver> solver;
	owned_ptr<btCollisionShape> plane;
	owned_ptr<btCollisionShape> sphere;
	owned_ptr<btRigidBody, bodies_deleter> bodies;
	owned_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	float timeToSimulate;
};

} // namespace DEV
