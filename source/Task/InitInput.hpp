#pragma once
#include "Tasking.hpp"
#include "Data/InputData.hpp"

namespace DEV {

struct InitInput
{
	InOut (InputData) input;

	__End__;

	void run();
};

} // namespace DEV