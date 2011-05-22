#pragma once
#include "Data/InputData.hpp"

namespace DEV {

struct GetInput
{
	InputData& input;

	void run();
};

} // namespace DEV