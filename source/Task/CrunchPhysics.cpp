#include "CrunchPhysics.hpp"
#include <algorithm>

namespace DEV {

void CrunchPhysics::run()
{
	btVector3 vel = state.bodies[0].getLinearVelocity();
	if (player.jump) vel.setZ( 5 );

	float mult = player.sprint ? 15.0f : 5.0f;

	state.bodies[0].activate(true);
	state.bodies[0].setLinearVelocity( btVector3(mult * player.mov.x(), mult * player.mov.y(), vel.z()));
	
	//state.dynamicsWorld->stepSimulation(timing.delta, 6);
	//return;

	float const fixedTimeStep = 1.0f / 60.0f;
	int const maxSubSteps = 6;

	//fixed timestep with interpolation
	state.timeToSimulate += timing.delta;
	int subSteps = std::min(int(state.timeToSimulate / fixedTimeStep), maxSubSteps);
	state.timeToSimulate -= subSteps * fixedTimeStep;

	btDiscreteDynamicsWorld* world = state.dynamicsWorld;

	//world->saveKinematicState(fixedTimeStep * subSteps);
	world->applyGravity();

	for (int k = 0; k < subSteps; k++)
	{
		world->predictUnconstraintMotion(fixedTimeStep);
		world->updateAabbs();
		world->m_broadphasePairCache->calculateOverlappingPairs(world->m_dispatcher1);

		btDispatcherInfo& dispatchInfo = world->getDispatchInfo();
		dispatchInfo.m_timeStep = fixedTimeStep;
		dispatchInfo.m_stepCount = 0;

		world->m_dispatcher1->dispatchAllCollisionPairs(
				world->m_broadphasePairCache->getOverlappingPairCache(),
				dispatchInfo, world->m_dispatcher1);	

		//if (getDispatchInfo().m_useContinuous)
		//	world->addSpeculativeContacts(fixedTimeStep);
		world->calculateSimulationIslands();
		
		world->getSolverInfo().m_timeStep = fixedTimeStep;
		world->solveConstraints(world->getSolverInfo());
		world->integrateTransforms(fixedTimeStep);
		//world->updateActivationState(fixedTimeStep);
	}

	world->synchronizeMotionStates();
	world->clearForces();
}

} // namespace DEV
