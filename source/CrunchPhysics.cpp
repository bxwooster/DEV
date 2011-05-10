#include "PhysicsState.hpp"
#include "PlayerState.hpp"
#include "TimingData.hpp"
#include "Transforms.hpp"

#include <algorithm>

namespace Devora {

void internalSingleStepSimulation(btDiscreteDynamicsWorld* world, btScalar timeStep)
{
	world->predictUnconstraintMotion(timeStep);
	world->performDiscreteCollisionDetection();
	world->addSpeculativeContacts(timeStep);
	world->calculateSimulationIslands();
	world->solveConstraints(world->getSolverInfo());
	world->integrateTransforms(timeStep);
	world->updateActivationState(timeStep);
}

void stepSimulation(btDiscreteDynamicsWorld* world, float timeStep)
{
	static float localTime = 0;
	float const fixedTimeStep = 1.0f / 60.0f;
	int const maxSubSteps = 6;

	//fixed timestep with interpolation
	localTime += timeStep;
	int numSimulationSubSteps = std::min(int(localTime / fixedTimeStep), maxSubSteps);
	localTime -= numSimulationSubSteps * fixedTimeStep;

	for (int i = 0; i < world->m_nonStaticRigidBodies.size(); i++)
	{
		btRigidBody* body = world->m_nonStaticRigidBodies[i];
		if (body->isActive())
		{
			body->applyGravity();
		}
	}


	for (int k = 0; k < numSimulationSubSteps; k++)
	{
		internalSingleStepSimulation(world, fixedTimeStep);
	}

	for (int i = 0; i < world->m_nonStaticRigidBodies.size(); i++)
	{
		btRigidBody* body = world->m_nonStaticRigidBodies[i];
		if (body->isActive())
		{
			btTransform interpolatedTransform;
			btTransformUtil::integrateTransform(body->getInterpolationWorldTransform(),
				body->getInterpolationLinearVelocity(),body->getInterpolationAngularVelocity(),
				localTime*body->getHitFraction(),interpolatedTransform);
			body->getMotionState()->setWorldTransform(interpolatedTransform);
		}
		body->clearForces();
	}
}

void CrunchPhysics(PhysicsState& state, Transforms& transforms,
	PlayerState& player, TimingData& timing)
{
	btVector3 vel = state.bodies[0].getLinearVelocity();
	if (player.jump) vel.setZ( 5 );

	float mult = player.sprint ? 15.0f : 5.0f;

	state.bodies[0].activate(true);
	state.bodies[0].setLinearVelocity( btVector3(mult * player.mov.x(), mult * player.mov.y(), vel.z()));
	
	stepSimulation(state.dynamicsWorld.get(), timing.delta);
}

} // namespace Devora