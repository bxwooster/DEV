#pragma once
#include "Data/DeviceState.hpp"
#include "Data/PostProcessInfo.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitPostProcess
{
	PostProcessInfo& info;
	DeviceState& device;
	ShaderCache& cache;

	void run();
};

} // namespace DEV