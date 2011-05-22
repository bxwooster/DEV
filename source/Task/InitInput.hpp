#pragma once
#include "Data/InputData.hpp"

namespace DEV {

struct InitInput
{
	InOut (InputData) input;

	void run();
};

} // namespace DEV