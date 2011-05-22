#pragma once
#include "Data/Transforms.hpp"
#include "Data/PlayerState.hpp"
#include "Data/Camera.hpp"

namespace DEV {

struct DeriveCamera
{
	Transforms& transforms;
	PlayerState& player;
	Camera& camera;

	void run();
};

} // namespace DEV