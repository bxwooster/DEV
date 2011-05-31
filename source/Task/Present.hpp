#pragma once
#include "Tasking.hpp"
#include "Data/DeviceState.hpp"
#include "Data/GraphicsState.hpp"

namespace DEV {

struct Present
{
	InOut (DeviceState) device;
	InOut (GraphicsState) graphics;

	__End__;

	void run();
};

} // namespace DEV