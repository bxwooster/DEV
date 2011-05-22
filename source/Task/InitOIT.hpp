#pragma once
#include "Task.hpp"
#include "Data/Camera.hpp"
#include "Data/DeviceState.hpp"
#include "Data/UBuffer.hpp"

namespace DEV {

struct InitOIT
{
	DeviceState& device;
	Camera& camera;
	UBuffer& oit_start;
	UBuffer& oit_scattered;
	UBuffer& oit_consolidated;

	void run();
};

} // namespace DEV