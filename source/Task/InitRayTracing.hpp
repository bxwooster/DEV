#pragma once
#include "Tasking.hpp"
#include "Data/DeviceState.hpp"
#include "Data/RayTracingInfo.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitRayTracing
{
	InOut (RayTracingInfo) info;
	InOut (DeviceState) device;
	InOut (ShaderCache) cache;

	__End__;

	void run();
};

} // namespace DEV