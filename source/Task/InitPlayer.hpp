#pragma once
#include "Task.hpp"
#include "Data/PlayerState.hpp"

namespace DEV {

struct InitPlayer
{
	InOut (PlayerState) state;

	void run();
};

} // namespace DEV