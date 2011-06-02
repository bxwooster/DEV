#pragma once
#include "Tasking.hpp"
#include "Data/TimingData.hpp"

namespace DEV {

struct GetTiming
{
	InOut (TimingData) timing;
	__End__;

	void run();
};

} // namespace DEV