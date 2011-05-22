#include "InitPhysics.hpp"

namespace DEV {

using namespace std;

void InitPhysics::run()
{
	state.collisionConfiguration = unique_ptr<btDefaultCollisionConfiguration>
		(new btDefaultCollisionConfiguration() );
	state.dispatcher = unique_ptr<btCollisionDispatcher>
		(new btCollisionDispatcher(state.collisionConfiguration.get()) );
	state.broadphase = unique_ptr<btDbvtBroadphase>
		(new btDbvtBroadphase() );
	state.solver = unique_ptr<btSequentialImpulseConstraintSolver>
		(new btSequentialImpulseConstraintSolver() );

	state.dynamicsWorld = unique_ptr<btDiscreteDynamicsWorld>
		(new btDiscreteDynamicsWorld(state.dispatcher.get(), state.broadphase.get(),
			 state.solver.get(), state.collisionConfiguration.get()) );
	state.dynamicsWorld->getSolverInfo().m_splitImpulse = true;
	state.dynamicsWorld->getSolverInfo().m_numIterations = 20;
	state.dynamicsWorld->getDispatchInfo().m_useContinuous = true;
	state.dynamicsWorld->setGravity(btVector3(0, 0, -9.81));

	state.plane = unique_ptr<btCollisionShape>
		(new btStaticPlaneShape(btVector3(0, 0, 1), 0)) ;
	state.sphere = unique_ptr<btCollisionShape>
		(new btSphereShape(1) );
}

} // namespace DEV