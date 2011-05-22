#pragma once
#include "Task.hpp"
#include "Data/TimingData.hpp"

namespace DEV {

struct InitTiming
{
	TimingData& timing;

	void run();
};

} // namespace DEV