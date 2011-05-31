#pragma once
#include "Tasking.hpp"
#include "Data/PlayerState.hpp"

namespace DEV {

struct InitPlayer
{
	InOut (PlayerState) state;

	__End__;

	void run();
};

} // namespace DEV