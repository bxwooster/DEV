#ifndef __Physics_h__
#define __Physics_h__

#include "Matrix.h"
#include "btBulletDynamicsCommon.h"

namespace Devora {
using namespace common;

class Physics
{
private:
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btDbvtBroadphase* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btRigidBody* playerBody;
	btRigidBody* planeBody;
	btSphereShape* sphere;
	std::vector<btRigidBody*> bodies;

public:
	Physics();
	~Physics();

	void capture(btAlignedObjectArray<Matrix4f>&, Matrix4f& view);
	void step(double);
	void control_player(Vector2f direction, bool jump);
};

} // namespace Devora
#endif