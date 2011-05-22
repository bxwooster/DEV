#include "CrunchPhysics.hpp"

namespace DEV {

void CrunchPhysics::run()
{
	btVector3 vel = state.bodies.get()[0].getLinearVelocity();
	if (player.jump) vel.setZ( 5 );

	float mult = player.sprint ? 15.0f : 5.0f;

	state.bodies.get()[0].activate(true);
	state.bodies.get()[0].setLinearVelocity( btVector3(mult * player.mov.x(), mult * player.mov.y(), vel.z()));
	
	state.dynamicsWorld->stepSimulation(timing.delta, 6);
}

} // namespace DEV