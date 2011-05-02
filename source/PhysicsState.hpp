#ifndef __Devora_PhysicsState__
#define __Devora_PhysicsState__

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
	std::unique_ptr<btRigidBody> playerBody;
	std::unique_ptr<btRigidBody> planeBody;
	std::unique_ptr<btCollisionShape> plane;
	std::unique_ptr<btCollisionShape> sphere;
	std::vector<btRigidBody*> bodies;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
};

} // namespace Devora

#endif