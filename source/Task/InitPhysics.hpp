#pragma once
#include "Task.hpp"
#include "Data/PhysicsState.hpp"

namespace DEV {

struct InitPhysics
{
	PhysicsState& state;

	void run();
};

} // namespace DEV