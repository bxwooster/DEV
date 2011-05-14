#ifndef __Devora_PhysicsState__
#define __Devora_PhysicsState__

#include <LinearMath/btAlignedObjectArray.h>
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <memory>

namespace Devora {

struct PhysicsState
{
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btDbvtBroadphase> broadphase;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btCollisionShape> plane;
	std::unique_ptr<btCollisionShape> sphere;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	btRigidBody* bodies;
	float timeToSimulate;
};

} // namespace Devora

#endif