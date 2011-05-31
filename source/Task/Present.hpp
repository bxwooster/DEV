#pragma once
#include "Tasking.hpp"
#include "Data/DeviceState.hpp"

namespace DEV {

struct Present
{
	InOut (DeviceState) device;

	__End__;

	void run();
};

} // namespace DEV