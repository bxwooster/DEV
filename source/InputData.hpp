#ifndef __Devora_InputData__
#define __Devora_InputData__

#include <set>
#include <vector>

namespace Devora {

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

} // namespace Devora

#endif