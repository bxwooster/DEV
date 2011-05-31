#pragma once
#include "Tasking.hpp"
#include "Data/VisualRenderInfo.hpp"
#include "Data/DeviceState.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitVisualRender
{
	InOut (VisualRenderInfo) info;
	InOut (DeviceState) device;
	InOut (ShaderCache) cache;

	__End__;

	void run();
};

} // namespace DEV