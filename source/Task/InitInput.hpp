#pragma once
#include "Data/InputData.hpp"

namespace DEV {

struct InitInput
{
	InputData& input;

	void run();
};

} // namespace DEV