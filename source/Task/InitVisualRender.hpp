#pragma once
#include "Task.hpp"
#include "Data/VisualRenderInfo.hpp"
#include "Data/DeviceState.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitVisualRender
{
	InOut (VisualRenderInfo) info;
	InOut (DeviceState) device;
	InOut (ShaderCache) cache;

	void run();
};

} // namespace DEV