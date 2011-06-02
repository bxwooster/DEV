#pragma once
#include "Tasking.hpp"
#include "Data/Transforms.hpp"
#include "Data/Visuals.hpp"
#include "Data/Lights.hpp"
#include "Data/Geometries.hpp"
#include "Data/PhysicsState.hpp"
#include "Data/DeviceState.hpp"

namespace DEV {

struct InitScene
{
	InOut (Transforms) transforms;
	InOut (Visuals) visuals;
	InOut (Lights) lights;
	InOut (Geometries) geometries;
	InOut (PhysicsState) state;
	InOut (DeviceState) device;

	__End__;

	void run();
};

} // namespace DEV