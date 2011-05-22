#pragma once
#include <LinearMath/btAlignedObjectArray.h>
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <memory>

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

	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btDbvtBroadphase> broadphase;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btCollisionShape> plane;
	std::unique_ptr<btCollisionShape> sphere;
	std::unique_ptr<btRigidBody, bodies_deleter> bodies;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
};

} // namespace DEV
