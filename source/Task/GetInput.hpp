#pragma once
#include "Tasking.hpp"
#include "Data/InputData.hpp"

namespace DEV {

struct GetInput
{
	InOut (InputData) input;
	__End__;

	void run();
};

} // namespace DEV