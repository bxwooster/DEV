#ifndef __Physics_h__
#define __Physics_h__

#include "Matrix.h"
#include "btBulletDynamicsCommon.h"

using namespace common;

class Physics
{
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btDbvtBroadphase* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btCollisionShape* groundShape;

public:
	Physics();

	void capture(btAlignedObjectArray<Matrix4f>&);
	void step(double);
};

#endif