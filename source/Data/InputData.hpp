#pragma once
#include <unordered_set>
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
	std::unordered_set<Key> keys_down;
	std::unordered_set<Key> keys_up;
	std::unordered_set<Key> keys_held;
};

} // namespace DEV
