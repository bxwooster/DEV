#pragma once
#include "Task.hpp"
#include "Data/InputData.hpp"

namespace DEV {

struct GetInput
{
	InOut (InputData) input;

	void run();
};

} // namespace DEV