#pragma once
#include "Task.hpp"
#include "Data/Transforms.hpp"
#include "Data/Visuals.hpp"
#include "Data/Lights.hpp"
#include "Data/Geometries.hpp"
#include "Data/PhysicsState.hpp"
#include "Data/DeviceState.hpp"

namespace DEV {

struct InitScene
{
	Transforms& transforms;
	Visuals& visuals;
	Lights& lights;
	Geometries& geometries;
	PhysicsState& state;
	DeviceState& device;

	void run();
};

} // namespace DEV