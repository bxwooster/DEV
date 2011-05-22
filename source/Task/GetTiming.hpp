#pragma once
#include "Task.hpp"
#include "Data/TimingData.hpp"

namespace DEV {

struct GetTiming
{
	InOut (TimingData) timing;

	void run();
};

} // namespace DEV