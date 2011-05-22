#pragma once
#include "Task.hpp"
#include "Data/PlayerState.hpp"
#include "Data/TimingData.hpp"
#include "Data/InputData.hpp"

#include <algorithm>

namespace DEV {

struct DerivePlayerState
{
	PlayerState& state;
	InputData& input;
	TimingData& timing;

	void run();
};

} // namespace DEV