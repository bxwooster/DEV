#pragma once
#include "Task.hpp"
#include "Data/InputData.hpp"

namespace DEV {

struct GetInput
{
	InputData& input;

	void run();
};

} // namespace DEV