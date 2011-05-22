#pragma once
#include "Data/DeviceState.hpp"
#include "Data/RayTracingInfo.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitRayTracing
{
	RayTracingInfo& info;
	DeviceState& device;
	ShaderCache& cache;

	void run();
};

} // namespace DEV