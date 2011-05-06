#ifndef __Devora_Lights__
#define __Devora_Lights__

#include <LinearMath/btAlignedObjectArray.h>

namespace Devora {

struct Light
{
	Vector3f colour;
	int index;
};

struct Lights
{
	btAlignedObjectArray<Light> dir;
	btAlignedObjectArray<Light> point;
};

} // namespace Devora

#endif