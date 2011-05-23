#pragma once
#include "Task.hpp"
#include "Data/DeviceState.hpp"
#include "Data/PostProcessInfo.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitPostProcess
{
	InOut (PostProcessInfo) info;
	InOut (DeviceState) device;
	InOut (ShaderCache) cache;

	void run();
};

} // namespace DEV