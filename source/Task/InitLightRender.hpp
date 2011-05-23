#pragma once
#include "Task.hpp"
#include "Data/Camera.hpp"
#include "Data/LightRenderInfo.hpp"
#include "Data/DeviceState.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitLightRender
{
	InOut (LightRenderInfo) info;
	InOut (DeviceState) device;
	InOut (ShaderCache) cache;
	In (Camera) camera;

	void run();
};

} // namespace DEV