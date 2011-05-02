#ifndef __Devora_Lights__
#define __Devora_Lights__

#include <LinearMath/btAlignedObjectArray.h>

namespace Devora {

namespace LightType
{
	enum Enum
	{
		point,
		directional
	};
};

struct Light
{
	Vector3f colour;
	int index;
	int type;
};

typedef btAlignedObjectArray<Light> Lights;

} // namespace Devora

#endif