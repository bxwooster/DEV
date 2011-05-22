#pragma once
#include "Data/PlayerState.hpp"

namespace DEV {

struct InitPlayer
{
	PlayerState& state;

	void run();
};

} // namespace DEV