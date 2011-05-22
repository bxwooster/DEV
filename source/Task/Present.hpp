#pragma once
#include "Data/DeviceState.hpp"

namespace DEV {

struct Present
{
	DeviceState& device;

	void run();
};

} // namespace DEV