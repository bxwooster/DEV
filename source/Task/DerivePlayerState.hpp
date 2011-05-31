#pragma once
#include "Tasking.hpp"
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
	__End__;

	void run();
};

} // namespace DEV