#pragma once
#include "Task.hpp"
#include "Data/VisualRenderInfo.hpp"
#include "Data/DeviceState.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitVisualRender
{
	VisualRenderInfo& info;
	DeviceState& device;
	ShaderCache& cache;

	void run();
};

} // namespace DEV