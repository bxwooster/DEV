#include "PhysicsState.hpp"

#include "Matrix.h"

#include <btBulletDynamicsCommon.h>

namespace Devora {

void InitPhysics(PhysicsState& state)
{
	state.collisionConfiguration = new btDefaultCollisionConfiguration();
	state.dispatcher = new btCollisionDispatcher(state.collisionConfiguration);
	state.broadphase = new btDbvtBroadphase();
	state.solver = new btSequentialImpulseConstraintSolver();

	state.dynamicsWorld = new btDiscreteDynamicsWorld(state.dispatcher,
		state.broadphase, state.solver, state.collisionConfiguration);
	state.dynamicsWorld->getSolverInfo().m_splitImpulse = true;
	state.dynamicsWorld->getSolverInfo().m_numIterations = 20;
	state.dynamicsWorld->getDispatchInfo().m_useContinuous = true;
	state.dynamicsWorld->setGravity(btVector3(0, 0, -10));

	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);

	btTransform groundTransform;
	groundTransform.setIdentity();

	btScalar mass(0);
	btVector3 localInertia(0, 0, 0);
		
	btMotionState* motionState = new btDefaultMotionState(groundTransform);
	state.planeBody = new btRigidBody(mass, motionState, groundShape, localInertia);
	state.dynamicsWorld->addRigidBody(state.planeBody);

	state.sphere = new btSphereShape(1);

	state.eye = Matrix4f::Identity();
	state.eye.col(3).head<3>() = Vector3f(5, 5, 5);
}

} // namespace Devora