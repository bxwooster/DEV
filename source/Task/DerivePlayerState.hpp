#pragma once
#include "Task.hpp"
#include "Data/PlayerState.hpp"
#include "Data/TimingData.hpp"
#include "Data/InputData.hpp"

#include <algorithm>

namespace DEV {

struct DerivePlayerState
{
	InOut (PlayerState) state;
	In (InputData) input;
	In (TimingData) timing;

	void run();
};

} // namespace DEV