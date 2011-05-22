#pragma once
#include "Task.hpp"
#include "Data/Camera.hpp"
#include "Data/LightRenderInfo.hpp"
#include "Data/DeviceState.hpp"
#include "Data/ShaderCache.hpp"

namespace DEV {

struct InitLightRender
{
	LightRenderInfo& info;
	DeviceState& device;
	ShaderCache& cache;
	Camera& camera;

	void run();
};

} // namespace DEV