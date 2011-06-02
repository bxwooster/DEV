#pragma once
#include "Tasking.hpp"
#include "Data/Transforms.hpp"
#include "Data/PlayerState.hpp"
#include "Data/Camera.hpp"

namespace DEV {

struct DeriveCamera
{
	In (Transforms) transforms;
	In (PlayerState) player;
	InOut (Camera) camera;
	__End__;

	void run();
};

} // namespace DEV