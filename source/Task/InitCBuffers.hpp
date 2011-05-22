#pragma once
#include "Data/DeviceState.hpp"
#include "Data/CBuffer.hpp"

namespace DEV {

struct InitCBuffer
{
	DeviceState& device;
	CBuffer& cb;
	size_t size;

	void run();
};

} // namespace DEV