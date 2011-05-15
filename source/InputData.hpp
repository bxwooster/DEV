#pragma once

#include <set>
#include <vector>

namespace DEV {

struct InputData
{
	typedef int Key;
	struct Mouse
	{
		long x;
		long y;
	};

	Mouse mouse;
	std::set<Key> keys_down;
	std::set<Key> keys_up;
	std::set<Key> keys_held;
};

} // namespace DEV
