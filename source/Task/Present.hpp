#pragma once
#include "Task.hpp"
#include "Data/DeviceState.hpp"

namespace DEV {

struct Present
{
	InOut (DeviceState) device;

	void run();
};

} // namespace DEV