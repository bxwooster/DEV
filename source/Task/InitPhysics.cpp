#include "InitPhysics.hpp"

namespace DEV {

using namespace std;

void InitPhysics::run()
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
	state.dynamicsWorld->setGravity(btVector3(0, 0, -9.81f));

	state.plane = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
	state.sphere = new btSphereShape(1);

	state.timeToSimulate = 0;
}

} // namespace DEV