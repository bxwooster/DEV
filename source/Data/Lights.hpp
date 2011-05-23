#pragma once
#include "Matrix.hpp"
#include <LinearMath/btAlignedObjectArray.h>

namespace DEV {

struct Light
{
	Vector3f colour;
	int transform;
};

struct Lights
{
	btAlignedObjectArray<Light> dir;
	btAlignedObjectArray<Light> point;
};

} // namespace DEV
