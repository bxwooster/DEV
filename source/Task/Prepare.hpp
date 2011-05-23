#pragma once
#include "Task.hpp"
#include "Data/Camera.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/CBuffer.hpp"
#include "Data/UBuffer.hpp"

namespace DEV {

struct Prepare
{
	InOut (GraphicsState) state;
	InOut (CBuffer) cb_frame;
	InOut (ZBuffer) zbuffer;
	InOut (UBuffer) oit_start;
	In (Camera) camera;

	void run();
};

} // namespace DEV