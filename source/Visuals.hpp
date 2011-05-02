#ifndef __Devora_Visuals__
#define __Devora_Visuals__

#include <vector>

namespace Devora {

struct Visual
{
	int index;
	int type;
};

typedef std::vector<Visual> Visuals;

} // namespace Devora

#endif