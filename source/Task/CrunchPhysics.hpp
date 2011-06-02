#pragma once
#include "Tasking.hpp"
#include "Data/PhysicsState.hpp"
#include "Data/PlayerState.hpp"
#include "Data/TimingData.hpp"
#include "Data/Transforms.hpp"

namespace DEV {

struct CrunchPhysics
{
	InOut (PhysicsState) state;
	In (Transforms) transforms;
	In (PlayerState) player;
	In (TimingData) timing;
	__End__;

	void run();
};

} // namespace DEV