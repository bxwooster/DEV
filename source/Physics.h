#ifndef __Physics_h__
#define __Physics_h__

#include "Matrix.h"
#include "btBulletDynamicsCommon.h"

namespace Devora {
using namespace common;

class Physics
{
public:
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btDbvtBroadphase* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btCollisionShape* groundShape;
	btRigidBody* playerBody;

public:
	Physics();

	void capture(btAlignedObjectArray<Matrix4f>&, Matrix4f& view);
	void step(double);
	void control_player(Vector2f direction, bool jump);
};

} // namespace Devora
#endif