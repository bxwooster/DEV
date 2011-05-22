#pragma once
#include "Task.hpp"
#include "Data/Transforms.hpp"
#include "Data/PlayerState.hpp"
#include "Data/Camera.hpp"

namespace DEV {

struct DeriveCamera
{
	In (Transforms) transforms;
	In (PlayerState) player;
	InOut (Camera) camera;

	void run();
};

} // namespace DEV