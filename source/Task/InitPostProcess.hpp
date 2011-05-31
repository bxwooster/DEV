#pragma once
#include "Tasking.hpp"
#include "Data/DeviceState.hpp"
#include "Data/PostProcessInfo.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitPostProcess
{
	InOut (PostProcessInfo) info;
	InOut (DeviceState) device;
	InOut (ShaderCache) cache;

	__End__;

	void run();
};

} // namespace DEV