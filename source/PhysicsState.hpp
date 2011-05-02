#ifndef __Devora_PhysicsState__
#define __Devora_PhysicsState__

#include "Matrix.h"
#include <btBulletDynamicsCommon.h>
#include <vector>

namespace Devora {

struct PhysicsState
{
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btDbvtBroadphase* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btRigidBody* playerBody;
	btRigidBody* planeBody;
	btSphereShape* sphere;
	std::vector<btRigidBody*> bodies;
	Matrix4f eye;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif