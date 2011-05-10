#include "PhysicsState.hpp"
#include "PlayerState.hpp"
#include "TimingData.hpp"
#include "Transforms.hpp"

namespace Devora {

void CrunchPhysics(PhysicsState& state, Transforms& transforms,
	PlayerState& player, TimingData& timing)
{
	btVector3 vel = state.bodies[0].getLinearVelocity();
	if (player.jump) vel.setZ( 5 );

	float mult = player.sprint ? 15.0f : 5.0f;

	state.bodies[0].activate(true);
	state.bodies[0].setLinearVelocity( btVector3(mult * player.mov.x(), mult * player.mov.y(), vel.z()));
	
	state.dynamicsWorld->stepSimulation(timing.delta, 6);
}

} // namespace Devora