#include "PhysicsState.hpp"

namespace Devora {

using namespace std;

void InitPhysics(PhysicsState& state)
{
	state.collisionConfiguration = unique_ptr<btDefaultCollisionConfiguration>
		(new btDefaultCollisionConfiguration() );
	state.dispatcher = unique_ptr<btCollisionDispatcher>
		(new btCollisionDispatcher(state.collisionConfiguration.get()) );
	state.broadphase = unique_ptr<btDbvtBroadphase>
		(new btDbvtBroadphase() );
	state.solver = unique_ptr<btSequentialImpulseConstraintSolver>
		(new btSequentialImpulseConstraintSolver() );

	state.dynamicsWorld = unique_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(state.dispatcher.get(),
		state.broadphase.get(), state.solver.get(), state.collisionConfiguration.get()) );
	state.dynamicsWorld->getSolverInfo().m_splitImpulse = true;
	state.dynamicsWorld->getSolverInfo().m_numIterations = 20;
	state.dynamicsWorld->getDispatchInfo().m_useContinuous = true;
	state.dynamicsWorld->setGravity(btVector3(0, 0, -10));

	state.plane = unique_ptr<btCollisionShape>
		(new btStaticPlaneShape(btVector3(0, 0, 1), 0)) ;
	state.sphere = unique_ptr<btCollisionShape>
		(new btSphereShape(1) );

	btTransform groundTransform;
	groundTransform.setIdentity();

	btScalar mass(0);
	btVector3 localInertia(0, 0, 0);
		
	btMotionState* motionState = new btDefaultMotionState(groundTransform);
	state.planeBody = unique_ptr<btRigidBody>
		( new btRigidBody(mass, motionState, state.plane.get(), localInertia) );

	state.dynamicsWorld->addRigidBody(state.planeBody.get());
}

} // namespace Devora