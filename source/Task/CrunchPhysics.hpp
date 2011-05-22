#pragma once
#include "Data/PhysicsState.hpp"
#include "Data/PlayerState.hpp"
#include "Data/TimingData.hpp"
#include "Data/Transforms.hpp"

namespace DEV {

struct CrunchPhysics
{
	PhysicsState& state;
	Transforms& transforms;
	PlayerState& player;
	TimingData& timing;

	void run();
};

} // namespace DEV