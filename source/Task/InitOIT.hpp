#pragma once
#include "Tasking.hpp"
#include "Data/Camera.hpp"
#include "Data/DeviceState.hpp"
#include "Data/UBuffer.hpp"

namespace DEV {

struct InitOIT
{
	InOut (DeviceState) device;
	In (Camera) camera;
	InOut (UBuffer) oit_start;
	InOut (UBuffer) oit_scattered;
	InOut (UBuffer) oit_consolidated;

	__End__;

	void run();
};

} // namespace DEV