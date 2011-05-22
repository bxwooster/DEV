#pragma once
#include "Task.hpp"
#include "Data/TimingData.hpp"

namespace DEV {

struct GetTiming
{
	TimingData& timing;

	void run();
};

} // namespace DEV