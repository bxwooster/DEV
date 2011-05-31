#pragma once
#include "Tasking.hpp"
#include "Data/PhysicsState.hpp"

namespace DEV {

struct InitPhysics
{
	InOut (PhysicsState) state;

	__End__;

	void run();
};

} // namespace DEV